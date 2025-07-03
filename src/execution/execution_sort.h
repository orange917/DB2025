/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#pragma once
#include "execution_defs.h"
#include "execution_manager.h"
#include "executor_abstract.h"
#include "index/ix.h"
#include "system/sm.h"

class SortExecutor : public AbstractExecutor {
   private:
    std::unique_ptr<AbstractExecutor> prev_;
    ColMeta cols_;                              // 框架中只支持一个键排序，需要自行修改数据结构支持多个键排序
    size_t tuple_num;
    std::vector<size_t> used_tuple;
    std::unique_ptr<RmRecord> current_tuple;
    std::vector<ColMeta> cols_meta_;            // 添加列元数据
    size_t len_;                                // 添加记录长度
    
    // 排序相关
    std::vector<std::unique_ptr<RmRecord>> sorted_records_;  // 排序后的记录
    size_t current_idx_;                        // 当前记录索引
    bool is_initialized_;                       // 是否已初始化

    std::vector<ColMeta> order_by_cols_;
    std::vector<bool> is_desc_;
    int limit_val_;

    public:
    SortExecutor(std::unique_ptr<AbstractExecutor> prev, const std::vector<TabCol>& order_by,
                 const std::vector<bool>& is_desc, int limit_val)
        : prev_(std::move(prev)), limit_val_(limit_val), current_idx_(0), is_initialized_(false) {
        // 复制子执行器的列元数据
        cols_meta_ = prev_->cols();
        len_ = prev_->tupleLen();

        // 确保 is_desc 和 order_by 的大小匹配
        if (order_by.size() != is_desc.size()) {
            throw InternalError("SortExecutor: order_by and is_desc size mismatch");
        }

        // 获取所有排序列的 ColMeta，并按顺序存储 is_desc
        for (size_t i = 0; i < order_by.size(); ++i) {
            auto col_iter = get_col(prev_->cols(), order_by[i]);
            if (col_iter == prev_->cols().end()) {
                throw InternalError("SortExecutor: order by column not found");
            }
            order_by_cols_.push_back(*col_iter);
            is_desc_.push_back(is_desc[i]);
        }
    }

    void beginTuple() override { 
        std::cout << "SortExecutor: beginTuple called" << std::endl;
        prev_->beginTuple();
        current_idx_ = 0;
        is_initialized_ = false;
    }

    void nextTuple() override {
        std::cout << "SortExecutor: nextTuple called" << std::endl;
        if (!is_initialized_) {
            // 第一次调用，执行排序
            perform_sort();
            is_initialized_ = true;
        }
    }

    std::unique_ptr<RmRecord> Next() override {
        std::cout << "SortExecutor: Next called, current_idx_=" << current_idx_ << std::endl;
        if (!is_initialized_) {
            // 第一次调用，执行排序
            perform_sort();
            is_initialized_ = true;
        }
        
        if (current_idx_ >= sorted_records_.size()) {
            std::cout << "SortExecutor: No more records, returning nullptr" << std::endl;
            return nullptr;
        }
        
        // 返回当前记录并准备下一个
        auto record = std::move(sorted_records_[current_idx_]);
        current_idx_++;
        std::cout << "SortExecutor: Returning record " << (current_idx_-1) << " of " << sorted_records_.size() << std::endl;
        return record;
    }

    bool is_end() const override {
        if (!is_initialized_) {
            return false;
        }
        return current_idx_ >= sorted_records_.size();
    }

    const std::vector<ColMeta> &cols() const override {
        return cols_meta_;
    }

    size_t tupleLen() const override {
        return len_;
    }

    Rid &rid() override { return _abstract_rid; }

private:
    void perform_sort() {
        // 收集所有记录
        sorted_records_.clear();
        
        // 从子执行器获取所有记录
        prev_->beginTuple();
        while (!prev_->is_end()) {
            auto record = prev_->Next();
            if (record) {
                sorted_records_.push_back(std::move(record));
            }
            prev_->nextTuple();
        }
        
        // 添加调试信息
        std::cout << "SortExecutor: Collected " << sorted_records_.size() << " records for sorting" << std::endl;
        
        // 根据排序列进行排序
        std::sort(sorted_records_.begin(), sorted_records_.end(), 
                 [this](const std::unique_ptr<RmRecord>& a, const std::unique_ptr<RmRecord>& b) {
                     return compare_records(a, b);
                 });
        
        std::cout << "SortExecutor: Sorting completed" << std::endl;

        if (limit_val_ > 0 && sorted_records_.size() > limit_val_) {
            sorted_records_.resize(limit_val_);
        }
    }
    
    bool compare_records(const std::unique_ptr<RmRecord>& a, const std::unique_ptr<RmRecord>& b) {
        for (size_t i = 0; i < order_by_cols_.size(); ++i) {
            const ColMeta& col = order_by_cols_[i];
            const char* a_val = a->data + col.offset;
            const char* b_val = b->data + col.offset;
            int cmp = 0;
            switch (col.type) {
                case TYPE_INT: {
                    int a_int = *(int*)a_val;
                    int b_int = *(int*)b_val;
                    if (a_int < b_int) cmp = -1;
                    else if (a_int > b_int) cmp = 1;
                    break;
                }
                case TYPE_FLOAT: {
                    float a_float = *(float*)a_val;
                    float b_float = *(float*)b_val;
                    if (a_float < b_float) cmp = -1;
                    else if (a_float > b_float) cmp = 1;
                    break;
                }
                case TYPE_STRING: {
                    cmp = memcmp(a_val, b_val, col.len);
                    break;
                }
                default:
                    break;
            }
            if (cmp != 0) {
                // 升序: cmp<0 返回true; 降序: cmp>0 返回true
                return is_desc_[i] ? (cmp > 0) : (cmp < 0);
            }
        }
        return false; // 全部相等
    }
};