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

#include <cassert>
#include <cstring>
#include <memory>
#include <string>
#include <vector>
#include "defs.h"
#include "record/rm_defs.h"
#include "parser/ast.h"


struct TabCol {
    std::string tab_name;        // 真实表名（用于查询执行）
    std::string col_name;
    std::string alias;           // 列别名
    std::string original_tab_name; // 原始表名（用于显示，可能是别名）

    TabCol() = default;
    TabCol(const std::string& t, const std::string& c, const std::string& a = "") : tab_name(t), col_name(c), alias(a), original_tab_name(t) {}
    TabCol(const std::string& t, const std::string& c, const std::string& a, const std::string& orig_t) : tab_name(t), col_name(c), alias(a), original_tab_name(orig_t) {}

    friend bool operator<(const TabCol &x, const TabCol &y) {
        return std::make_pair(x.tab_name, x.col_name) < std::make_pair(y.tab_name, y.col_name);
    }
};

struct Value {
    ColType type;  // type of value
    union {
        int int_val;      // int value
        float float_val;  // float value
    };
    std::string str_val;  // string value

    std::shared_ptr<RmRecord> raw;  // raw record buffer

    void set_int(int int_val_) {
        type = TYPE_INT;
        int_val = int_val_;
    }

    void set_float(float float_val_) {
        type = TYPE_FLOAT;
        float_val = float_val_;
    }

    void set_str(std::string str_val_) {
        type = TYPE_STRING;
        str_val = std::move(str_val_);
    }

    void init_raw(int len) {
        if (raw != nullptr) {
            raw.reset();
        }
        assert(raw == nullptr);
        raw = std::make_shared<RmRecord>(len);
        if (type == TYPE_INT) {
            assert(len == sizeof(int));
            *(int *)(raw->data) = int_val;
        } else if (type == TYPE_FLOAT) {
            assert(len == sizeof(float));
            *(float *)(raw->data) = float_val;
        } else if (type == TYPE_STRING) {
            if (len < (int)str_val.size()) {
                throw StringOverflowError();
            }
            memset(raw->data, 0, len);
            memcpy(raw->data, str_val.c_str(), str_val.size());
        }
    }
};

enum CompOp { OP_EQ, OP_NE, OP_LT, OP_GT, OP_LE, OP_GE };

// 添加聚合函数类型枚举
enum AggFuncType { AGG_COUNT, AGG_MAX, AGG_MIN, AGG_SUM, AGG_AVG };

// 添加聚合函数结构
struct AggFunc {
    AggFuncType func_type;
    TabCol col;       // 列名，对于COUNT(*)时tab_name和col_name都为空
    std::string alias; // 别名
    
    // 默认构造函数
    AggFunc() : func_type(AGG_COUNT), col(), alias("") {}
    
    AggFunc(AggFuncType func_type_, TabCol col_, std::string alias_ = "") :
        func_type(func_type_), col(std::move(col_)), alias(std::move(alias_)) {}
};

struct Condition {
    TabCol lhs_col;   // left-hand side column
    CompOp op;        // comparison operator
    bool is_rhs_val;  // true if right-hand side is a value (not a column)
    TabCol rhs_col;   // right-hand side column
    Value rhs_val;    // right-hand side value
    
    // 聚合相关字段（用于HAVING条件）
    bool is_lhs_agg;  // true if left-hand side is an aggregate function
    bool is_rhs_agg;  // true if right-hand side is an aggregate function
    AggFunc lhs_agg;  // left-hand side aggregate function
    AggFunc rhs_agg;  // right-hand side aggregate function
    Value lhs_value;  // left-hand side value (for constant values)
    Value rhs_value;  // right-hand side value (for constant values)
};

struct SetClause {
    TabCol lhs;
    Value rhs;
};