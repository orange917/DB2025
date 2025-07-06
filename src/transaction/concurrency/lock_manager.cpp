/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "lock_manager.h"
#include "transaction/transaction_manager.h"

/**
 * @brief 检查当前是否为MVCC模式
 */
bool LockManager::IsMVCCMode() {
    if (get_concurrency_mode_) {
        return get_concurrency_mode_() == ConcurrencyMode::MVCC;
    }
    return false;
}

/**
 * @description: 申请行级共享锁
 * @return {bool} 加锁是否成功
 * @param {Transaction*} txn 要申请锁的事务对象指针
 * @param {Rid&} rid 加锁的目标记录ID 记录所在的表的fd
 * @param {int} tab_fd
 */
bool LockManager::lock_shared_on_record(Transaction* txn, const Rid& rid, int tab_fd) {
    // 在MVCC模式下，读操作通常不需要锁，因为MVCC提供快照隔离
    if (IsMVCCMode()) {
        return true;
    }
    
    // 非MVCC模式下的传统锁处理逻辑
    // TODO: 实现真正的锁机制
    return true;
}

/**
 * @description: 申请行级排他锁
 * @return {bool} 加锁是否成功
 * @param {Transaction*} txn 要申请锁的事务对象指针
 * @param {Rid&} rid 加锁的目标记录ID
 * @param {int} tab_fd 记录所在的表的fd
 */
bool LockManager::lock_exclusive_on_record(Transaction* txn, const Rid& rid, int tab_fd) {
    // 在MVCC模式下，写操作由写-写冲突检测来处理，不需要传统的排他锁
    if (IsMVCCMode()) {
        return true;
    }

    // 非MVCC模式下的传统锁处理逻辑
    // TODO: 实现真正的锁机制
    return true;
}

/**
 * @description: 申请表级读锁
 * @return {bool} 返回加锁是否成功
 * @param {Transaction*} txn 要申请锁的事务对象指针
 * @param {int} tab_fd 目标表的fd
 */
bool LockManager::lock_shared_on_table(Transaction* txn, int tab_fd) {
    // 在MVCC模式下，读操作通常不需要表级锁
    if (IsMVCCMode()) {
        return true;
    }
    
    // 非MVCC模式下的传统锁处理逻辑
    // TODO: 实现真正的锁机制
    return true;
}

/**
 * @description: 申请表级写锁
 * @return {bool} 返回加锁是否成功
 * @param {Transaction*} txn 要申请锁的事务对象指针
 * @param {int} tab_fd 目标表的fd
 */
bool LockManager::lock_exclusive_on_table(Transaction* txn, int tab_fd) {
    // 在MVCC模式下，表级写锁可能仍然需要，用于DDL操作
    if (IsMVCCMode()) {
        // 对于DDL操作，仍然需要表级锁来保证一致性
        // 这里暂时简化处理
        return true;
    }
    
    // 非MVCC模式下的传统锁处理逻辑
    // TODO: 实现真正的锁机制
    return true;
}

/**
 * @description: 申请表级意向读锁
 * @return {bool} 返回加锁是否成功
 * @param {Transaction*} txn 要申请锁的事务对象指针
 * @param {int} tab_fd 目标表的fd
 */
bool LockManager::lock_IS_on_table(Transaction* txn, int tab_fd) {
    // 在MVCC模式下，意向锁的作用有限
    if (IsMVCCMode()) {
        return true;
    }
    
    // 非MVCC模式下的传统锁处理逻辑
    // TODO: 实现真正的锁机制
    return true;
}

/**
 * @description: 申请表级意向写锁
 * @return {bool} 返回加锁是否成功
 * @param {Transaction*} txn 要申请锁的事务对象指针
 * @param {int} tab_fd 目标表的fd
 */
bool LockManager::lock_IX_on_table(Transaction* txn, int tab_fd) {
    // 在MVCC模式下，意向锁的作用有限
    if (IsMVCCMode()) {
        return true;
    }
    
    // 非MVCC模式下的传统锁处理逻辑
    // TODO: 实现真正的锁机制
    return true;
}

/**
 * @description: 释放锁
 * @return {bool} 返回解锁是否成功
 * @param {Transaction*} txn 要释放锁的事务对象指针
 * @param {LockDataId} lock_data_id 要释放的锁ID
 */
bool LockManager::unlock(Transaction* txn, LockDataId lock_data_id) {
    // 在MVCC模式下，由于大部分操作不实际持有锁，解锁操作通常是无操作
    if (IsMVCCMode()) {
        return true;
    }
   
    // 非MVCC模式下的传统锁处理逻辑
    // TODO: 实现真正的锁机制
    return true;
}