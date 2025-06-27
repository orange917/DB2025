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
#include "index/ix_index_handle.h"

class IndexScanExecutor : public AbstractExecutor {
   private:
    std::string tab_name_;                      // 表名称
    TabMeta tab_;                               // 表的元数据
    std::vector<Condition> conds_;              // 扫描条件
    RmFileHandle *fh_;                          // 表的数据文件句柄
    std::vector<ColMeta> cols_;                 // 需要读取的字段
    size_t len_;                                // 选取出来的一条记录的长度
    std::vector<Condition> fed_conds_;          // 扫描条件，和conds_字段相同

    std::vector<std::string> index_col_names_;  // index scan涉及到的索引包含的字段
    IndexMeta index_meta_;                      // index scan涉及到的索引元数据

    Rid rid_;
    std::unique_ptr<RecScan> scan_;

    SmManager *sm_manager_;

   public:
    IndexScanExecutor(SmManager *sm_manager, std::string tab_name, std::vector<Condition> conds, std::vector<std::string> index_col_names,
                    Context *context) {
        sm_manager_ = sm_manager;
        context_ = context;
        tab_name_ = std::move(tab_name);
        tab_ = sm_manager_->db_.get_table(tab_name_);
        conds_ = std::move(conds);
        // index_no_ = index_no;
        index_col_names_ = index_col_names;
        index_meta_ = *(tab_.get_index_meta(index_col_names_));
        fh_ = sm_manager_->fhs_.at(tab_name_).get();
        cols_ = tab_.cols;
        len_ = cols_.back().offset + cols_.back().len;
        std::map<CompOp, CompOp> swap_op = {
            {OP_EQ, OP_EQ}, {OP_NE, OP_NE}, {OP_LT, OP_GT}, {OP_GT, OP_LT}, {OP_LE, OP_GE}, {OP_GE, OP_LE},
        };

        for (auto &cond : conds_) {
            if (cond.lhs_col.tab_name != tab_name_) {
                // lhs is on other table, now rhs must be on this table
                assert(!cond.is_rhs_val && cond.rhs_col.tab_name == tab_name_);
                // swap lhs and rhs
                std::swap(cond.lhs_col, cond.rhs_col);
                cond.op = swap_op.at(cond.op);
            }
        }
        fed_conds_ = conds_;
    }

    void beginTuple() override {
        // 构建索引查询的起始键和终止键
        IxIndexHandle *ih = sm_manager_->ihs_.at(sm_manager_->get_ix_manager()->get_index_name(tab_name_, index_col_names_)).get();

        // 初始化索引扫描的键值范围
        char *lower_key = nullptr;
        char *upper_key = nullptr;
        bool lower_inclusive = false;
        bool upper_inclusive = false;

        // 分析条件，构建查询范围
        int total_len = 0;
        for (const auto &col : index_meta_.cols) {
            total_len += col.len;
        }

        // 临时存储各列的条件范围
        struct KeyRange {
            char *lower_key;
            char *upper_key;
            bool lower_inclusive;
            bool upper_inclusive;
            bool has_lower;
            bool has_upper;

            KeyRange() : lower_key(nullptr), upper_key(nullptr),
                        lower_inclusive(false), upper_inclusive(false),
                        has_lower(false), has_upper(false) {}
        };

        std::map<std::string, KeyRange> col_ranges;

        // 从条件中提取每一列的查询范围
        for (auto &cond : conds_) {
            // 只处理与索引列相关的条件
            bool is_index_col = false;
            for (const auto &index_col : index_col_names_) {
                if (cond.lhs_col.col_name == index_col && cond.lhs_col.tab_name == tab_name_) {
                    is_index_col = true;
                    break;
                }
            }

            if (!is_index_col || !cond.is_rhs_val) {
                continue;
            }

            // 获取列元数据
            auto col_meta = tab_.get_col(cond.lhs_col.col_name);
            auto &range = col_ranges[col_meta->name];

            // 根据操作符更新列的条件范围
            switch (cond.op) {
                case OP_EQ: {
                    if (!range.has_lower ||
                        (range.has_lower && memcmp(range.lower_key, cond.rhs_val.raw->data, col_meta->len) < 0)) {
                        range.lower_key = cond.rhs_val.raw->data;
                        range.lower_inclusive = true;
                        range.has_lower = true;
                    }
                    if (!range.has_upper ||
                        (range.has_upper && memcmp(range.upper_key, cond.rhs_val.raw->data, col_meta->len) > 0)) {
                        range.upper_key = cond.rhs_val.raw->data;
                        range.upper_inclusive = true;
                        range.has_upper = true;
                    }
                    break;
                }
                case OP_LT: {
                    if (!range.has_upper ||
                        (range.has_upper && memcmp(range.upper_key, cond.rhs_val.raw->data, col_meta->len) > 0)) {
                        range.upper_key = cond.rhs_val.raw->data;
                        range.upper_inclusive = false;
                        range.has_upper = true;
                    }
                    break;
                }
                case OP_LE: {
                    if (!range.has_upper ||
                        (range.has_upper && memcmp(range.upper_key, cond.rhs_val.raw->data, col_meta->len) > 0)) {
                        range.upper_key = cond.rhs_val.raw->data;
                        range.upper_inclusive = true;
                        range.has_upper = true;
                    }
                    break;
                }
                case OP_GT: {
                    if (!range.has_lower ||
                        (range.has_lower && memcmp(range.lower_key, cond.rhs_val.raw->data, col_meta->len) < 0)) {
                        range.lower_key = cond.rhs_val.raw->data;
                        range.lower_inclusive = false;
                        range.has_lower = true;
                    }
                    break;
                }
                case OP_GE: {
                    if (!range.has_lower ||
                        (range.has_lower && memcmp(range.lower_key, cond.rhs_val.raw->data, col_meta->len) < 0)) {
                        range.lower_key = cond.rhs_val.raw->data;
                        range.lower_inclusive = true;
                        range.has_lower = true;
                    }
                    break;
                }
                default:
                    break;
            }
        }

        // 构建完整的索引查询键
        bool need_scan = false;
        if (!col_ranges.empty()) {
            lower_key = new char[total_len];
            upper_key = new char[total_len];
            memset(lower_key, 0, total_len);
            memset(upper_key, 0xff, total_len); // 默认最大值

            int offset = 0;
            bool has_lower = false;
            bool has_upper = false;

            // 按照索引列顺序构建键
            for (const auto &index_col_name : index_col_names_) {
                auto col_meta = tab_.get_col(index_col_name);
                auto it = col_ranges.find(index_col_name);

                if (it != col_ranges.end() && it->second.has_lower) {
                    memcpy(lower_key + offset, it->second.lower_key, col_meta->len);
                    has_lower = true;
                    lower_inclusive = it->second.lower_inclusive;
                }

                if (it != col_ranges.find(index_col_name)) {
                    // 如果对前面的列有限制，但对这一列没有，后续列就不能用于范围查询了
                    // 但如果前面列是等值条件，此列是范围条件，依然可以用索引
                    if (has_lower && !has_upper && it != col_ranges.end() && it->second.has_upper) {
                        memcpy(upper_key + offset, it->second.upper_key, col_meta->len);
                        has_upper = true;
                        upper_inclusive = it->second.upper_inclusive;
                        // 对于后续列，使用默认的最小/最大值
                        break;
                    }
                }

                if (it != col_ranges.end() && it->second.has_upper) {
                    memcpy(upper_key + offset, it->second.upper_key, col_meta->len);
                    has_upper = true;
                    upper_inclusive = it->second.upper_inclusive;
                }

                offset += col_meta->len;

                // 最左前缀匹配原则：如果当前列没有等值条件，后续列就不能用于范围查询
                if (it == col_ranges.end() ||
                   (it != col_ranges.end() && (!it->second.has_lower || !it->second.has_upper || 
                                              memcmp(it->second.lower_key, it->second.upper_key, col_meta->len) != 0))) {
                    break;
                }
            }

            // 创建索引扫描
            if (has_lower && has_upper) {
                scan_ = ih->create_scan(lower_key, upper_key, lower_inclusive, upper_inclusive);
            } else if (has_lower) {
                scan_ = ih->create_scan(lower_key, nullptr, lower_inclusive, false);
            } else if (has_upper) {
                scan_ = ih->create_scan(nullptr, upper_key, false, upper_inclusive);
            } else {
                need_scan = true;
            }
        } else {
            need_scan = true;
        }

        // 如果没有可用条件，执行完整扫描
        if (need_scan) {
            scan_ = ih->create_scan(nullptr, nullptr, false, false);
        }
        nextTuple();
    }

    void nextTuple() override {
        // 循环，直到找到一个匹配的记录或扫描结束
        while (!scan_->is_end()) {
            // 获取当前记录ID
            rid_ = scan_->rid();
            // 读取记录数据
            auto rec = fh_->get_record(rid_, context_);
            if (!rec) {
                scan_->next();
                continue;
            }

            // 检查所有附加条件是否满足
            bool match = true;
            for (auto &cond : fed_conds_) {
                // ... 您现有的条件检查逻辑 ...
                // (这部分逻辑是正确的，无需修改)
                const auto &col = tab_.get_col(cond.lhs_col.col_name);
                char *lhs_val = rec->data + col->offset;
                if (cond.is_rhs_val) {
                    char *rhs_val = cond.rhs_val.raw->data;
                    int cmp_res = ix_compare(lhs_val, rhs_val, col->type, col->len);
                    switch (cond.op) {
                        case OP_EQ: match = (cmp_res == 0); break;
                        case OP_NE: match = (cmp_res != 0); break;
                        case OP_LT: match = (cmp_res < 0); break;
                        case OP_GT: match = (cmp_res > 0); break;
                        case OP_LE: match = (cmp_res <= 0); break;
                        case OP_GE: match = (cmp_res >= 0); break;
                    }
                }
                if (!match) break;
            }

            if (match) {
                // 找到了一个匹配的记录，方法结束。
                // rid_ 现在指向这个记录，等待Next()方法来取用。
                return;
            }

            // 当前记录不匹配，移动到索引中的下一条记录
            scan_->next();
        }
    }

    bool is_end() const override {
        return scan_ == nullptr || scan_->is_end();
    }

     std::unique_ptr<RmRecord> Next() override {
        // **关键修改 2: 重新设计Next()的逻辑**

        // 如果上一次的nextTuple()调用已经确定我们到达了末尾，则返回空
        if (is_end()) {
            return nullptr;
        }

        // 获取当前已定位好的记录
        auto record_to_return = fh_->get_record(rid_, context_);

        // 在返回当前记录之前，为下一次调用做好准备
        // 1. 将底层扫描器向前移动一个位置
        scan_->next();
        // 2. 调用nextTuple()来寻找下一个满足所有条件的记录
        nextTuple();

        // 返回我们之前保存的记录
        return record_to_return;
    }

    const std::vector<ColMeta> &cols() const override {
        return cols_;
    }


    size_t tupleLen() const override {
        return len_;
    }

    Rid &rid() override { return rid_; }
};