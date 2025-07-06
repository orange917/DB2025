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

#include "common/context.h"
#include "execution_defs.h"
#include "execution_manager.h"
#include "executor_abstract.h"
#include "index/ix.h"
#include "system/sm.h"
#include <cstring>
#include <memory>
#include <cmath>

class SeqScanExecutor : public AbstractExecutor {
   private:
    static constexpr float EPSILON = 1e-6; //容差
    std::string tab_name_;              // 表的名称
    std::vector<Condition> conds_;      // scan的条件
    RmFileHandle *fh_;                  // 表的数据文件句柄
    std::vector<ColMeta> cols_;         // scan后生成的记录的字段
    size_t len_;                        // scan后生成的每条记录的长度
    std::vector<Condition> fed_conds_;  // 同conds_，两个字段相同

    Rid rid_;
    std::unique_ptr<RecScan> scan_;     // table_iterator

    SmManager *sm_manager_;

   public:
    SeqScanExecutor(SmManager *sm_manager, std::string tab_name, std::vector<Condition> conds, Context *context) {
        sm_manager_ = sm_manager;
        tab_name_ = std::move(tab_name);
        conds_ = std::move(conds);
        TabMeta &tab = sm_manager_->db_.get_table(tab_name_);
        fh_ = sm_manager_->fhs_.at(tab_name_).get();
        cols_ = tab.cols;
        len_ = cols_.back().offset + cols_.back().len;

        context_ = context;

        fed_conds_ = conds_;
    }

     // 初始化迭代器
     void beginTuple() override {
        // 创建表扫描器
        scan_ = fh_->create_scan();
        // 寻找第一个满足条件的元组
        while (!scan_->is_end()) {
            auto current_rid = scan_->rid();
            
            // 获取记录，根据并发控制模式选择不同的方法
            std::unique_ptr<RmRecord> record;
            if (context_ && context_->txn_mgr_ && 
                context_->txn_mgr_->get_concurrency_mode() == ConcurrencyMode::MVCC) {
                record = fh_->get_record_mvcc(current_rid, context_);
            } else {
                record = fh_->get_record(current_rid, context_);
            }
            
            if (record && eval_conds(record.get(), cols_)) {
                rid_ = current_rid;
                return;
            }
            scan_->next();
        }
    }

    void nextTuple() override {
        // 移动到下一个
        scan_->next();  // 移动到下一个记录
        // 检查一下是否满足条件
        while(!scan_->is_end())
        {
            auto current_rid = scan_->rid();
            
            // 获取记录，根据并发控制模式选择不同的方法
            std::unique_ptr<RmRecord> record;
            if (context_ && context_->txn_mgr_ && 
                context_->txn_mgr_->get_concurrency_mode() == ConcurrencyMode::MVCC) {
                record = fh_->get_record_mvcc(current_rid, context_);
            } else {
                record = fh_->get_record(current_rid, context_);
            }
            
            if (record && eval_conds(record.get(), cols_)) {
                rid_ = current_rid;
                return;
            }
            scan_->next();  // 继续查找下一个记录
        }
    }

    bool is_end() const override {
        return scan_->is_end();
    }


    std::unique_ptr<RmRecord> Next() override {
        auto rec = rid();
        
        // 根据并发控制模式选择不同的方法获取记录
        if (context_ && context_->txn_mgr_ && 
            context_->txn_mgr_->get_concurrency_mode() == ConcurrencyMode::MVCC) {
            return fh_->get_record_mvcc(rec, context_);
        } else {
            return fh_->get_record(rec, context_);
        }
    }

    // 条件判断函数
    bool eval_conds(const RmRecord *rec, const std::vector<ColMeta> &cols) {
        for (auto &cond : conds_) {
            if (!eval_cond(rec, cols, cond)) {
                return false;
            }
        }
        return true;
    }

    bool eval_cond(const RmRecord *rec, const std::vector<ColMeta> &cols, const Condition &cond) {
        // 实现条件判断逻辑
        // 获取左操作数值
        auto lhs_col = get_col(cols, cond.lhs_col);
        const char *lhs_value = rec->data + lhs_col->offset;

        // 根据右操作数是值还是列，进行处理
        if (cond.is_rhs_val) {
            // 右操作数是字面值
            // 对于浮点数字面值，特殊处理，因为它的值已预先解析
            if (lhs_col->type == TYPE_FLOAT) {
                float lhs_val = *(float*)lhs_value;
                float rhs_val = cond.rhs_val.float_val;
                switch (cond.op) {
                    case OP_EQ: return std::fabs(lhs_val - rhs_val) <= EPSILON;
                    case OP_NE: return std::fabs(lhs_val - rhs_val) > EPSILON;
                    case OP_LT: return rhs_val - lhs_val > EPSILON;
                    case OP_GT: return lhs_val - rhs_val > EPSILON;
                    case OP_LE: return lhs_val - rhs_val <= EPSILON;
                    case OP_GE: return rhs_val - lhs_val <= EPSILON;
                    default: return false;
                }
            }
            // 对于其他类型的字面值，使用通用的比较函数
            return compare_values(lhs_col->type, lhs_value, cond.rhs_val.raw->data, cond.op, lhs_col->len);
        } else {
            // 右操作数也是列
            auto rhs_col = get_col(cols, cond.rhs_col);
            const char *rhs_value = rec->data + rhs_col->offset;
            return compare_values(lhs_col->type, lhs_value, rhs_value, cond.op, lhs_col->len);
        }
    }

    // 比较两个值是否符合指定的比较操作符
    bool compare_values(ColType type, const char* lhs, const char* rhs, CompOp op, int len) {
        switch (type) {
            case TYPE_INT: {
                int lhs_val = *(int*)lhs;
                int rhs_val = *(int*)rhs;
                switch (op) {
                    case OP_EQ: return lhs_val == rhs_val;
                    case OP_NE: return lhs_val != rhs_val;
                    case OP_LT: return lhs_val < rhs_val;
                    case OP_GT: return lhs_val > rhs_val;
                    case OP_LE: return lhs_val <= rhs_val;
                    case OP_GE: return lhs_val >= rhs_val;
                    default: return false;
                }
            }
            case TYPE_FLOAT: {
                float lhs_val = *(float*)lhs;
                float rhs_val = *(float*)rhs;
                switch (op) {
                    case OP_EQ: return std::fabs(lhs_val - rhs_val) <= EPSILON;
                    case OP_NE: return std::fabs(lhs_val - rhs_val) > EPSILON;
                    case OP_LT: return rhs_val - lhs_val > EPSILON;
                    case OP_GT: return lhs_val - rhs_val > EPSILON;
                    case OP_LE: return lhs_val - rhs_val <= EPSILON;
                    case OP_GE: return rhs_val - lhs_val <= EPSILON;
                    default: return false;
                }
            }
            case TYPE_STRING: {
                int cmp_result = memcmp(lhs, rhs, len);
                switch (op) {
                    case OP_EQ: return cmp_result == 0;
                    case OP_NE: return cmp_result != 0;
                    case OP_LT: return cmp_result < 0;
                    case OP_GT: return cmp_result > 0;
                    case OP_LE: return cmp_result <= 0;
                    case OP_GE: return cmp_result >= 0;
                    default: return false;
                }
            }
            default:
                return false;
        }
    }

    const std::vector<ColMeta> &cols() const override {
        return cols_;
    }

    size_t tupleLen() const override {
        return len_;
    }

    Rid &rid() override { return rid_; }
};