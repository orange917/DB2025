/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "transaction_manager.h"
#include "record/rm_file_handle.h"
#include "system/sm_manager.h"
#include "transaction/txn_defs.h"

std::unordered_map<txn_id_t, Transaction *> TransactionManager::txn_map = {};

/**
 * @description: 事务的开始方法
 * @return {Transaction*} 开始事务的指针
 * @param {Transaction*} txn 事务指针，空指针代表需要创建新事务，否则开始已有事务
 * @param {LogManager*} log_manager 日志管理器指针
 */
Transaction * TransactionManager::begin(Transaction* txn, LogManager* log_manager) {
    // Todo:
    // 1. 判断传入事务参数是否为空指针
    // 2. 如果为空指针，创建新事务
    // 3. 把开始事务加入到全局事务表中
    // 4. 返回当前事务指针
    // 如果需要支持MVCC请在上述过程中添加代码

    if (txn == nullptr) {
        // 生成新的事务ID
        txn_id_t new_id = next_txn_id_.fetch_add(1);
        txn = new Transaction(new_id);
        // 设置为显式事务模式
        txn->set_txn_mode(true);
    }

    // 确保事务状态为DEFAULT
    if(txn->get_state() != TransactionState::DEFAULT) {
        txn->set_state(TransactionState::DEFAULT);
    }

    // 把开始事务加入到全局事务表中
    {
        std::unique_lock<std::mutex> lock(latch_); // Protect txn_map
        txn_map[txn->get_transaction_id()] = txn;
    }

    return txn;
}

/**
 * @description: 事务的提交方法
 * @param {Transaction*} txn 需要提交的事务
 * @param {LogManager*} log_manager 日志管理器指针
 */
void TransactionManager::commit(Transaction* txn, LogManager* log_manager) {
    // Todo:
    // 1. 如果存在未提交的写操作，提交所有的写操作
    // 2. 释放所有锁
    // 3. 释放事务相关资源，eg.锁集
    // 4. 把事务日志刷入磁盘中
    // 5. 更新事务状态
    // 如果需要支持MVCC请在上述过程中添加代码

    // 检查事务是否存在
    if(!txn)
    {
        throw std::runtime_error("Transaction does not exist.");
    }

    // 检查事务的状态，更新为已提交状态
    txn->set_state(TransactionState::COMMITTED);

    log_manager->flush_log_to_disk(); // 刷新日志到磁盘

    // 清空索引相关资源
    auto index_latch_page_set = txn->get_index_latch_page_set();
    index_latch_page_set->clear();

    // 释放所有锁资源
    auto lock_set = txn->get_lock_set();
    lock_set->clear();

    // 从全局事务表中移除此事务
    {
        std::unique_lock<std::mutex> lock(latch_);
        txn_map.erase(txn->get_transaction_id());
    }

}

/**
 * @description: 事务的终止（回滚）方法
 * @param {Transaction *} txn 需要回滚的事务
 * @param {LogManager} *log_manager 日志管理器指针
 */
void TransactionManager::abort(Transaction * txn, LogManager *log_manager) {
    // Todo:
    // 1. 回滚所有写操作
    // 2. 释放所有锁
    // 3. 清空事务相关资源，eg.锁集
    // 4. 把事务日志刷入磁盘中
    // 5. 更新事务状态
    // 如果需要支持MVCC请在上述过程中添加代码

    Context context(lock_manager_, log_manager, txn);

    // 更新事务状态为已终止
    txn->set_state(TransactionState::ABORTED);
    
     // 回滚DDL操作
     auto ddl_set = txn->get_ddl_set();
     while (!ddl_set->empty()) {
        auto ddl_record = ddl_set->back();
        ddl_set->pop_back(); // 先弹出记录，再尝试补偿
        try {
            if (ddl_record->ddl_type_ == DdlType::CREATE_INDEX) {
                sm_manager_->drop_index(ddl_record->tab_name_, ddl_record->col_names_, &context);
            } else if (ddl_record->ddl_type_ == DdlType::DROP_INDEX) {
               // 传递记录的 is_unique_ 属性
               sm_manager_->create_index(ddl_record->tab_name_, ddl_record->col_names_, &context, ddl_record->is_unique_);
            }
        } catch (const std::exception& e) {
            // 记录补偿操作失败的日志，但在abort期间不应重新抛出异常，避免中断回滚
            std::cerr << "Abort: Failed to compensate DDL operation. Error: " << e.what() << std::endl;
        }
        delete ddl_record;
    }
    
    // 回滚所有写操作
    auto write_set = txn->get_write_set();
    while (!write_set->empty()) {
        WriteRecord* record = write_set->back();
        write_set->pop_back();

        auto& rm_file = sm_manager_->fhs_.at(record->GetTableName());
        if(record->GetWriteType() == WType::INSERT_TUPLE) {
            // 如果是插入操作，删除记录
            rm_file->delete_record(record->GetRid(), &context);
        } else if(record->GetWriteType() == WType::UPDATE_TUPLE) {
            // 如果是更新操作，恢复旧记录
            rm_file->update_record(record->GetRid(), record->GetRecord().data, &context);
        } else if(record->GetWriteType() == WType::DELETE_TUPLE) {
            // 如果是删除操作，插入旧记录
            rm_file->insert_record(record->GetRecord().data, &context);
        }
        // 释放写记录资源
        delete record;
    }

    // 如果有日志管理器，将日志刷入磁盘
    if (log_manager != nullptr) {
        log_manager->flush_log_to_disk();
    }

    // 清空索引相关资源
    auto index_latch_page_set = txn->get_index_latch_page_set();
    while (!index_latch_page_set->empty()) {
        // Page* page = index_latch_page_set->front();
        index_latch_page_set->pop_front();
        // page->WUnlatch(); // 释放写锁
    }

    // 释放所有锁资源
    auto lock_set = txn->get_lock_set();
    lock_set->clear();

    // 从全局事务表中移除此事务
    {
        std::unique_lock<std::mutex> lock(latch_);
        txn_map.erase(txn->get_transaction_id());
    }

}