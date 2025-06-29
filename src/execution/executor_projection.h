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

class ProjectionExecutor : public AbstractExecutor {
   private:
    std::unique_ptr<AbstractExecutor> prev_;        // 投影节点的儿子节点
    std::vector<ColMeta> cols_;                     // 需要投影的字段
    size_t len_;                                    // 字段总长度
    std::vector<size_t> sel_idxs_;
    int limit_val_;                                 // LIMIT值
    int current_count_;                             // 当前已返回的记录数

   public:
   ProjectionExecutor(std::unique_ptr<AbstractExecutor> prev, const std::vector<TabCol>& sel_cols, int limit_val = -1) {
        if (!prev) {
            throw InternalError("ProjectionExecutor: prev is null");
        }
        prev_ = std::move(prev);
        limit_val_ = limit_val;
        current_count_ = 0;

        auto &prev_cols = prev_->cols();
        if (prev_cols.empty()) {
            throw InternalError("ProjectionExecutor: prev has no columns");
        }

        size_t curr_offset = 0;
        for (auto &sel_col : sel_cols) {
            auto pos = get_col(prev_cols, sel_col);
            sel_idxs_.push_back(pos - prev_cols.begin());
            auto col = *pos;
            col.offset = curr_offset;
            curr_offset += col.len;
            cols_.push_back(col);
        }
        len_ = curr_offset;
    }

    void beginTuple() override {
        // 初始化前一个执行器
        prev_->beginTuple();
        current_count_ = 0;
    }
    
    void nextTuple() override {
        // 移动前一个执行器到下一条记录
        prev_->nextTuple();
    }
    
    bool is_end() const override {
        // 检查前一个执行器是否结束，或者是否达到limit限制
        return prev_->is_end() || (limit_val_ > 0 && current_count_ >= limit_val_);
    }
    
    std::unique_ptr<RmRecord> Next() override {
        // 如果前一个执行器已经结束，返回空
        if (prev_->is_end()) {
            return nullptr;
        }
        
        // 检查是否达到limit限制
        if (limit_val_ > 0 && current_count_ >= limit_val_) {
            return nullptr;
        }
        
        // 从前一个执行器获取记录
        auto prev_record = prev_->Next();
        if (!prev_record) {
            return nullptr;
        }
        
        // 创建一个新记录，只包含要投影的列
        auto proj_record = std::make_unique<RmRecord>(len_);
        
        // 将选定的列从前一个记录拷贝到新记录中
        auto &prev_cols = prev_->cols();
        for (size_t i = 0; i < sel_idxs_.size(); i++) {
            size_t prev_idx = sel_idxs_[i];
            const ColMeta &prev_col = prev_cols[prev_idx];
            const ColMeta &proj_col = cols_[i];
            
            // 从前一个记录中拷贝字段数据到新记录
            memcpy(proj_record->data + proj_col.offset,
                   prev_record->data + prev_col.offset,
                   prev_col.len);
        }
        
        // 增加计数
        current_count_++;
        
        return proj_record;
    }

    const std::vector<ColMeta> &cols() const override {
        return cols_;
    }

    size_t tupleLen() const override {
        return len_;
    }

    Rid &rid() override { return _abstract_rid; }
};