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
    bool is_desc_;
    std::vector<size_t> used_tuple;
    std::unique_ptr<RmRecord> current_tuple;
    std::vector<ColMeta> cols_meta_;            // 添加列元数据
    size_t len_;                                // 添加记录长度
    
    // 排序相关
    std::vector<std::unique_ptr<RmRecord>> sorted_records_;  // 排序后的记录
    size_t current_idx_;                        // 当前记录索引
    bool is_initialized_;                       // 是否已初始化

   public:
    SortExecutor(std::unique_ptr<AbstractExecutor> prev, TabCol sel_cols, bool is_desc) {
        prev_ = std::move(prev);
        
        // 使用正确的方法获取列信息
        auto col_iter = get_col(prev_->cols(), sel_cols);
        cols_ = *col_iter;
        
        is_desc_ = is_desc;
        tuple_num = 0;
        used_tuple.clear();
        
        // 复制子执行器的列元数据
        cols_meta_ = prev_->cols();
        len_ = prev_->tupleLen();
        
        // 初始化排序相关变量
        current_idx_ = 0;
        is_initialized_ = false;
        
        // 添加调试信息
        std::cout << "SortExecutor: Sorting by column " << sel_cols.tab_name << "." << sel_cols.col_name << std::endl;
        std::cout << "SortExecutor: Column offset: " << cols_.offset << ", type: " << cols_.type << ", len: " << cols_.len << std::endl;
        std::cout << "SortExecutor: Sort direction: " << (is_desc_ ? "DESC" : "ASC") << std::endl;
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
        for (prev_->beginTuple(); !prev_->is_end(); prev_->nextTuple()) {
            auto record = prev_->Next();
            if (record) {
                sorted_records_.push_back(std::move(record));
            }
        }
        
        // 添加调试信息
        std::cout << "SortExecutor: Collected " << sorted_records_.size() << " records for sorting" << std::endl;
        
        // 根据排序列进行排序
        std::sort(sorted_records_.begin(), sorted_records_.end(), 
                 [this](const std::unique_ptr<RmRecord>& a, const std::unique_ptr<RmRecord>& b) {
                     return compare_records(a, b);
                 });
        
        std::cout << "SortExecutor: Sorting completed" << std::endl;
    }
    
    bool compare_records(const std::unique_ptr<RmRecord>& a, const std::unique_ptr<RmRecord>& b) {
        // 获取排序列的值
        const char* a_val = a->data + cols_.offset;
        const char* b_val = b->data + cols_.offset;
        
        // 根据列类型进行比较
        switch (cols_.type) {
            case TYPE_INT: {
                int a_int = *(int*)a_val;
                int b_int = *(int*)b_val;
                return is_desc_ ? (a_int > b_int) : (a_int < b_int);
            }
            case TYPE_FLOAT: {
                float a_float = *(float*)a_val;
                float b_float = *(float*)b_val;
                return is_desc_ ? (a_float > b_float) : (a_float < b_float);
            }
            case TYPE_STRING: {
                // 修复字符串比较：去除尾部的空格，使用正确的长度
                std::string a_str(a_val, cols_.len);
                std::string b_str(b_val, cols_.len);
                
                // 去除尾部的空格
                a_str.erase(a_str.find_last_not_of(' ') + 1);
                b_str.erase(b_str.find_last_not_of(' ') + 1);
                
                return is_desc_ ? (a_str > b_str) : (a_str < b_str);
            }
            default:
                return false;
        }
    }
};