/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "execution_common.h"
#include "record/rm_defs.h"
#include "record/rm.h"

/**
 * @brief 根据撤销日志重建元组
 * @param schema 表的元数据
 * @param base_tuple 基础元组
 * @param base_meta 基础元组的元数据
 * @param undo_logs 撤销日志列表
 * @return 重建后的元组，如果不可见则返回nullopt
 */
auto ReconstructTuple(const TabMeta *schema, const RmRecord &base_tuple, const TupleMeta &base_meta,
                      const std::vector<UndoLog> &undo_logs) -> std::optional<RmRecord> {
    // 如果元组被标记为删除，则不可见
    if (base_meta.is_deleted_) {
        return std::nullopt;
    }
    
    // 如果没有撤销日志，直接返回原始元组
    if (undo_logs.empty()) {
        return base_tuple;
    }
    
    // 创建新的记录
    RmRecord new_tuple(base_tuple.size);
    memcpy(new_tuple.data, base_tuple.data, base_tuple.size);
    
    // 应用撤销日志中的修改
    for (const auto &log : undo_logs) {
        if (log.is_deleted_) {
            // 如果日志表示删除操作，则元组不可见
            return std::nullopt;
        }
        
        // 应用修改的字段
        for (size_t i = 0; i < log.modified_fields_.size(); i++) {
            if (log.modified_fields_[i]) {
                // 获取字段的偏移量和类型
                int offset = schema->cols[i].offset;
                int len = schema->cols[i].len;
                ColType type = schema->cols[i].type;
                
                // 根据字段类型将日志中的值复制到元组中
                switch (type) {
                    case TYPE_INT:
                        memcpy(new_tuple.data + offset, &log.tuple_[i].int_val, sizeof(int));
                        break;
                    case TYPE_FLOAT:
                        memcpy(new_tuple.data + offset, &log.tuple_[i].float_val, sizeof(float));
                        break;
                    case TYPE_STRING:
                        memcpy(new_tuple.data + offset, log.tuple_[i].str_val.c_str(), len);
                        break;
                    default:
                        // 不支持的类型
                        break;
                }
            }
        }
    }
    
    return new_tuple;
}

/**
 * @brief 检查是否存在写-写冲突
 * @param tuple_ts 元组的时间戳
 * @param txn 当前事务
 * @return 如果存在冲突返回true，否则返回false
 */
auto IsWriteWriteConflict(timestamp_t tuple_ts, Transaction *txn) -> bool {
    // 如果元组的时间戳大于事务的开始时间戳，说明在事务开始后元组被修改过，存在冲突
    return tuple_ts > txn->get_start_ts();
} 