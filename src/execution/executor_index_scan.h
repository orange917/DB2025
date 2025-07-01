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

#include <algorithm>
#include <cfloat>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <memory>

#include "common/common.h"
#include "common/context.h"
#include "defs.h"
#include "execution_defs.h"
#include "execution_manager.h"
#include "executor_abstract.h"
#include "index/ix.h"
#include "index/ix_defs.h"
#include "index/ix_index_handle.h"
#include "index/ix_scan.h"
#include "record/rm_defs.h"
#include "system/sm.h"
#include "system/sm_meta.h"

class IndexScanExecutor : public AbstractExecutor {
 private:
  std::string tab_name_;              // 表名称
  TabMeta tab_;                       // 表的元数据
  std::vector<Condition> conds_;      // 扫描条件
  RmFileHandle *fh_;                  // 表的数据文件句柄
  std::vector<ColMeta> cols_;         // 需要读取的字段
  size_t len_;                        // 选取出来的一条记录的长度
  std::vector<Condition> fed_conds_;  // 扫描条件，和conds_字段相同

  std::vector<std::string> index_col_names_;  // index scan涉及到的索引包含的字段
  IndexMeta index_meta_;                      // index scan涉及到的索引元数据
  std::vector<ColMeta>
      index_tuple_cols_;  // 记录了index中的tuple的信息。相比index_meta.cols只修改了offset部分。

  Rid rid_;
  std::unique_ptr<RecScan> scan_;

  SmManager *sm_manager_;

 public:
  // 索引扫描算子需要读操作，所以需要context来获取事务信息。
  IndexScanExecutor(SmManager *sm_manager, std::string tab_name, std::vector<Condition> conds,
                    std::vector<std::string> index_col_names, Context *context) {
    sm_manager_ = sm_manager;
    tab_name_ = std::move(tab_name);
    tab_ = sm_manager_->db_.get_table(tab_name_);
    // index_no_ = index_no;
    index_col_names_ = index_col_names;

    AbstractExecutor::context_ = context;

    // 查找匹配的索引
    bool found_index = false;
    for (auto &index_meta : tab_.indexes) {
      size_t i;

      // 索引：index_col_names全部是索引cols的前缀。
      for (i = 0; i < index_col_names_.size(); ++i) {
        if (i >= index_meta.cols.size() || index_col_names_[i] != index_meta.cols[i].name) {
          break;
        }
      }

      if (i == index_col_names.size()) {
        index_meta_ = index_meta;
        found_index = true;
        break;
      }
    }

    // 如果没有找到匹配的索引，抛出异常
    if (!found_index) {
      throw InternalError("No matching index found for index scan");
    }

    fh_ = sm_manager_->fhs_.at(tab_name_).get();
    cols_ = tab_.cols;
    len_ = cols_.back().offset + cols_.back().len;

    std::map<CompOp, CompOp> swap_op = {
        {OP_EQ, OP_EQ}, {OP_NE, OP_NE}, {OP_LT, OP_GT},
        {OP_GT, OP_LT}, {OP_LE, OP_GE}, {OP_GE, OP_LE},
    };

    // 下面是把左边变成当前table。
    for (auto &cond : conds) {
      if (cond.lhs_col.tab_name != tab_name_) {
        // lhs is on other table, now rhs must be on this table
        assert(!cond.is_rhs_val && cond.rhs_col.tab_name == tab_name_);
        // swap lhs and rhs
        std::swap(cond.lhs_col, cond.rhs_col);
        cond.op = swap_op.at(cond.op);
      }

      // 现在条件的左表必然是tab_name_.
      // 现在左右表名不等，条件为等值，可以认为是归并连接。
      // 归并连接条件不可用作扫描算子的筛选。
      if (cond.op == OP_EQ && !cond.is_rhs_val &&
          cond.lhs_col.tab_name != cond.rhs_col.tab_name) {
        ;  // 什么都不做。
      } else {
        fed_conds_.push_back(cond);  //
      }
    }

    conds_ = fed_conds_;

    int offset = 0;
    for (auto &index_meta : index_meta_.cols) {
      index_tuple_cols_.emplace_back(index_meta);
      index_tuple_cols_.back().offset = offset;
      offset += index_meta.len;
    }

#ifdef DEBUG
    std::cout << "index scan, start !!!" << std::endl;
    for (auto &col : cols_) {
      std::cout << col.name << ' ';
    }
    std::cout << std::endl;
#endif
  }

  void beginTuple() override {
    // 进行必要的类型转换。
    // for (auto &cond : conds_) {
    //   if (cond.lhs_col.tab_name == tab_name_ && cond.is_rhs_val) {
    //     auto col_meta_iter =
    //         std::find_if(index_meta_.cols.begin(), index_meta_.cols.end(),
    //                      [&cond](ColMeta &col) { return cond.lhs_col.col_name == col.name; });
    //     if (col_meta_iter == index_meta_.cols.end()) {
    //       continue;
    //     }

    //     // 使用现有的类型转换方法
    //     if (col_meta_iter->type != cond.rhs_val.type) {
    //       if (col_meta_iter->type == TYPE_FLOAT && cond.rhs_val.type == TYPE_INT) {
    //         cond.rhs_val.float_val = static_cast<float>(cond.rhs_val.int_val);
    //         cond.rhs_val.type = TYPE_FLOAT;
    //       } else if (col_meta_iter->type == TYPE_INT && cond.rhs_val.type == TYPE_FLOAT) {
    //         cond.rhs_val.int_val = static_cast<int>(cond.rhs_val.float_val);
    //         cond.rhs_val.type = TYPE_INT;
    //       }
    //     }
    //     cond.rhs_val.init_raw(col_meta_iter->len);
    //   }
    // }

    RmRecord key_start(index_meta_.col_tot_len);
    RmRecord key_end(index_meta_.col_tot_len);
    std::vector<const ColMeta *> lower_col;
    std::vector<const ColMeta *> upper_col;

    auto ix_manager = sm_manager_->get_ix_manager();
    std::string index_name = ix_manager->get_index_name(tab_name_, index_meta_.cols);

    // 检查索引句柄是否存在，如果不存在则尝试打开索引文件
    if (sm_manager_->ihs_.find(index_name) == sm_manager_->ihs_.end()) {
        // 尝试打开索引文件
        if (ix_manager->exists(tab_name_, index_meta_.cols)) {
            auto ih = ix_manager->open_index(tab_name_, index_meta_.cols, &index_meta_);
            sm_manager_->ihs_[index_name] = std::move(ih);
        } else {
            throw InternalError("Index file not found: " + index_name);
        }
    }

    auto ix_index_handle = sm_manager_->ihs_.at(index_name).get();

    // 按照最左匹配原则构造上下界
    // 遍历索引的每一列，按顺序处理
    for (size_t col_idx = 0; col_idx < index_meta_.cols.size(); ++col_idx) {
        const std::string& index_col_name = index_meta_.cols[col_idx].name;
        bool found_condition = false;
        bool is_range_query = false;
        
        // 查找当前索引列对应的条件
        for (auto &condition : conds_) {
            if (condition.is_rhs_val && condition.lhs_col.tab_name == tab_name_ &&
                condition.lhs_col.col_name == index_col_name) {
                
                found_condition = true;
                const char *rhs_key = condition.rhs_val.raw->data;
                const ColMeta &index_col = *get_col(index_tuple_cols_, condition.lhs_col);

                switch (condition.op) {
                    case OP_EQ: {
                        // 等值查询：同时设置上下界
                        memcpy(key_start.data + index_col.offset, rhs_key, index_col.len);
                        memcpy(key_end.data + index_col.offset, rhs_key, index_col.len);
                        lower_col.push_back(&index_col);
                        upper_col.push_back(&index_col);
                        break;
                    }
                    case OP_GT:
                    case OP_GE: {
                        // 范围查询：设置下界
                        memcpy(key_start.data + index_col.offset, rhs_key, index_col.len);
                        lower_col.push_back(&index_col);
                        is_range_query = true;
                        break;
                    }
                    case OP_LT:
                    case OP_LE: {
                        // 范围查询：设置上界
                        memcpy(key_end.data + index_col.offset, rhs_key, index_col.len);
                        upper_col.push_back(&index_col);
                        is_range_query = true;
                        break;
                    }
                    default:
                        break;
                }
                break; // 找到条件后退出内层循环
            }
        }
        
        // 如果没有找到当前列的条件，或者遇到了范围查询，停止处理后续列
        if (!found_condition || is_range_query) {
            break;
        }
    }

    // 为未设置的列填充默认值
    for (size_t col_idx = lower_col.size(); col_idx < index_tuple_cols_.size(); ++col_idx) {
        switch (index_tuple_cols_[col_idx].type) {
            case TYPE_INT: {
                int min_int = INT32_MIN;
                int max_int = INT32_MAX;
                memcpy(key_start.data + index_tuple_cols_[col_idx].offset, &min_int,
                       index_tuple_cols_[col_idx].len);
                memcpy(key_end.data + index_tuple_cols_[col_idx].offset, &max_int,
                       index_tuple_cols_[col_idx].len);
                break;
            }
            case TYPE_FLOAT: {
                float min_float = -std::numeric_limits<float>::max();
                float max_float = std::numeric_limits<float>::max();
                memcpy(key_start.data + index_tuple_cols_[col_idx].offset, &min_float,
                       index_tuple_cols_[col_idx].len);
                memcpy(key_end.data + index_tuple_cols_[col_idx].offset, &max_float,
                       index_tuple_cols_[col_idx].len);
                break;
            }
            case TYPE_STRING: {
                memset(key_start.data + index_tuple_cols_[col_idx].offset, 0, index_tuple_cols_[col_idx].len);
                memset(key_end.data + index_tuple_cols_[col_idx].offset, CHAR_MAX, index_tuple_cols_[col_idx].len);
                break;
            }
            default:
                break;
        }
    }

    // 初始化 ix_scan 所需要的参数
    Iid lower_Iid;
    Iid upper_Iid;  

    // 确定扫描范围
    if (lower_col.empty()) {
        // 没有下界条件，从第一个叶子节点开始
        lower_Iid = ix_index_handle->leaf_begin();
    } else {
        lower_Iid = ix_index_handle->lower_bound(key_start.data);
    }

    if (upper_col.empty()) {
        // 没有上界条件，扫描到最后一个叶子节点结束
        upper_Iid = ix_index_handle->leaf_end();
    } else {
        upper_Iid = ix_index_handle->upper_bound(key_end.data);
    }

    // 初始化 索引表扫描操作接口
    scan_ = std::make_unique<IxScan>(ix_index_handle, lower_Iid, upper_Iid, sm_manager_->get_bpm());

    while (!scan_->is_end()) {
      auto rcd = fh_->get_record(scan_->rid(), context_);
      if (rcd == nullptr) { // 防止空指针
        scan_->next();
        continue;
      }
      // 使用现有的条件检查方法
      if (!eval_conds(rcd.get(), cols_)) {
        scan_->next();
      } else {
        rid_ = scan_->rid();
        break;
      }
    }
  }

  void nextTuple() override {
    for (scan_->next(); !scan_->is_end(); scan_->next()) {
      auto rcd = fh_->get_record(scan_->rid(), context_);
      if (rcd == nullptr) { // 防止空指针
        continue;
      }
      if (eval_conds(rcd.get(), cols_)) {
        rid_ = scan_->rid();
        break;
      }
    }
  }

  std::unique_ptr<RmRecord> Next() override { return fh_->get_record(rid_, context_); }

  Rid &rid() override { return rid_; }

  bool is_end() const override { return scan_->is_end(); }

  const std::vector<ColMeta> &cols() const override { return cols_; }

  size_t tupleLen() const override { return len_; };

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
    // 获取左操作数值
    auto lhs_col = get_col(cols, cond.lhs_col);
    const char *lhs_value = rec->data + lhs_col->offset;

    // 根据右操作数是值还是列，进行处理
    if (cond.is_rhs_val) {
      // 右操作数是字面值
      char *rhs_val = cond.rhs_val.raw->data;
      int cmp_res = ix_compare(lhs_value, rhs_val, lhs_col->type, lhs_col->len);
      
      switch (cond.op) {
        case OP_EQ: return (cmp_res == 0);
        case OP_NE: return (cmp_res != 0);
        case OP_LT: return (cmp_res < 0);
        case OP_GT: return (cmp_res > 0);
        case OP_LE: return (cmp_res <= 0);
        case OP_GE: return (cmp_res >= 0);
        default: return false;
      }
    } else {
      // 右操作数也是列
      auto rhs_col = get_col(cols, cond.rhs_col);
      const char *rhs_value = rec->data + rhs_col->offset;
      int cmp_res = ix_compare(lhs_value, rhs_value, lhs_col->type, lhs_col->len);
      
      switch (cond.op) {
        case OP_EQ: return (cmp_res == 0);
        case OP_NE: return (cmp_res != 0);
        case OP_LT: return (cmp_res < 0);
        case OP_GT: return (cmp_res > 0);
        case OP_LE: return (cmp_res <= 0);
        case OP_GE: return (cmp_res >= 0);
        default: return false;
      }
    }
  }
};