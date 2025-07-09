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

    // MVCC支持：分配时间戳并更新水印
    if (concurrency_mode_ == ConcurrencyMode::MVCC) {
        // 使用全局锁来确保时间戳分配的原子性，避免时间戳乱序问题
        std::unique_lock<std::mutex> ts_lock(timestamp_mutex_);
        
        // **调试输出**
        timestamp_t current_next = next_timestamp_.load();
        timestamp_t last_commit = last_commit_ts_.load();
        std::cout << "[DEBUG] Transaction " << txn->get_transaction_id() 
                  << " starting - current_next_ts: " << current_next 
                  << ", last_commit_ts: " << last_commit << std::endl;
        
        // **关键修复**：对于快照隔离，事务应该获得一个唯一的开始时间戳
        // 并且能够看到在其开始之前已经提交的所有数据
        timestamp_t read_ts;
        
        // 为事务分配一个新的时间戳作为开始时间戳
        // 这样可以确保事务的顺序性和唯一性
        read_ts = next_timestamp_.fetch_add(1);
        
        txn->set_start_ts(read_ts);
        
        // **调试输出**
        std::cout << "[DEBUG] Transaction " << txn->get_transaction_id() 
                  << " assigned start_ts: " << read_ts << std::endl;
        
        // 更新水印 - 加入活跃事务列表
        running_txns_.AddTxn(read_ts);
        
        ts_lock.unlock();
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

    // MVCC支持：分配提交时间戳并更新水印
    if (concurrency_mode_ == ConcurrencyMode::MVCC) {
        // 使用全局锁来确保时间戳分配的原子性，避免时间戳乱序问题
        std::unique_lock<std::mutex> ts_lock(timestamp_mutex_);
        
        // **调试输出**
        timestamp_t old_next = next_timestamp_.load();
        std::cout << "[DEBUG] Transaction " << txn->get_transaction_id() 
                  << " (start_ts: " << txn->get_start_ts() << ") committing - next_ts before: " << old_next << std::endl;
        
        // 分配提交时间戳
        timestamp_t commit_ts = next_timestamp_.fetch_add(1);
        
        // **调试输出**
        std::cout << "[DEBUG] Transaction " << txn->get_transaction_id() 
                  << " assigned commit_ts: " << commit_ts << std::endl;
        
        // 更新所有该事务创建的版本链中的时间戳
        UpdateCommitTimestamp(txn, commit_ts);
        
        // 更新最后提交时间戳
        timestamp_t last_ts = last_commit_ts_.load();
        while (last_ts < commit_ts && !last_commit_ts_.compare_exchange_weak(last_ts, commit_ts)) {
            // 自旋直到成功更新
        }
        
        // **调试输出**
        std::cout << "[DEBUG] Updated last_commit_ts to: " << commit_ts << std::endl;
        
        // 更新水印的提交时间戳
        running_txns_.UpdateCommitTs(commit_ts);
        
        // 从水印中移除事务
        running_txns_.RemoveTxn(txn->get_start_ts());
        
        // **新增**：记录已提交事务的时间戳，用于后续可见性检查
        {
            std::unique_lock<std::mutex> lock(committed_txns_mutex_);
            committed_txns_.insert(commit_ts);  // **修复**：应该插入commit_ts而不是start_ts
            std::cout << "[DEBUG] Added commit_ts=" << commit_ts << " to committed_txns_ (size now: " << committed_txns_.size() << ")" << std::endl;
        }
        
        ts_lock.unlock();
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

    // **关键修复**：在MVCC模式下，不要立即删除已提交的事务对象
    // 因为其他事务可能需要查询其状态来进行可见性检查
    // 事务对象的清理应该通过垃圾回收机制延迟进行
    if (concurrency_mode_ != ConcurrencyMode::MVCC) {
        // 只有在非MVCC模式下才立即从全局事务表中移除此事务
        std::unique_lock<std::mutex> lock(latch_);
        txn_map.erase(txn->get_transaction_id());
    }
    // 在MVCC模式下，已提交的事务对象将通过垃圾回收机制清理
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

    //从水印中移除事务
    if (concurrency_mode_ == ConcurrencyMode::MVCC) {
        running_txns_.RemoveTxn(txn->get_start_ts());
        
        // 记录已回滚事务的时间戳，用于后续可见性检查
        {
            std::unique_lock<std::mutex> lock(aborted_txns_mutex_);
            aborted_txns_.insert(txn->get_start_ts());
        }
        
        // 在MVCC模式下，不需要进行物理回滚
        // 事务回滚通过可见性检查来实现：
        // 1. 已回滚事务创建的版本对其他事务不可见
        // 2. 通过版本链访问之前的版本
        // 这样避免了物理修改数据，保持了MVCC的无锁特性
        
        // 重要：在MVCC模式下，不能立即从txn_map中删除事务对象
        // 因为其他事务可能需要通过版本链访问这个事务的撤销日志
        // 事务对象的清理应该通过垃圾回收机制延迟进行
        
    } else {
        // 非MVCC模式下的传统回滚逻辑
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
            } else if(record->GetWriteType() == WType::DELETE_TUPLE) {
                // 如果是删除操作，恢复记录
                rm_file->insert_record(record->GetRid(), record->GetRecord().data);
            } else if(record->GetWriteType() == WType::UPDATE_TUPLE) {
                // 如果是更新操作，恢复为原记录
                rm_file->update_record(record->GetRid(), record->GetRecord().data, &context);
            }
            delete record;
        }
        
        // 非MVCC模式下可以立即从全局事务表中移除事务
        {
            std::unique_lock<std::mutex> lock(latch_);
            txn_map.erase(txn->get_transaction_id());
        }
    }

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
}

/**
 * @brief 更新一个撤销链接，该链接将表堆元组与第一个撤销日志连接起来。
 * 在更新之前，将调用 `check` 函数以确保有效性。
 */
bool TransactionManager::UpdateUndoLink(Rid rid, std::optional<UndoLink> prev_link,
                    std::function<bool(std::optional<UndoLink>)> &&check) {
    // 获取页面版本信息
    std::shared_ptr<PageVersionInfo> page_version_info;
    {
        std::shared_lock<std::shared_mutex> lock(version_info_mutex_);
        auto it = version_info_.find(rid.page_no);
        if (it == version_info_.end()) {
            return false;
        }
        page_version_info = it->second;
    }
    
    // 获取槽位版本信息
    std::unique_lock<std::shared_mutex> lock(page_version_info->mutex_);
    auto it = page_version_info->prev_version_.find(rid.slot_no);
    if (it == page_version_info->prev_version_.end()) {
        return false;
    }
    
    // 检查版本链接
    if (check && !check(it->second.prev_.IsValid() ? std::optional<UndoLink>(it->second.prev_) : std::nullopt)) {
        return false;
    }
    
    // 更新版本链接
    if (prev_link.has_value()) {
        it->second.prev_ = *prev_link;
    } else {
        page_version_info->prev_version_.erase(it);
    }
    
    return true;
}

/**
 * @brief 更新一个撤销链接，该链接将表堆元组与第一个撤销日志连接起来。
 * 在更新之前，将调用 `check` 函数以确保有效性。
 */
bool TransactionManager::UpdateVersionLink(Rid rid, std::optional<VersionUndoLink> prev_version,
                       std::function<bool(std::optional<VersionUndoLink>)> &&check) {
    // 获取页面版本信息，如果不存在则创建
    std::shared_ptr<PageVersionInfo> page_version_info;
    {
        std::unique_lock<std::shared_mutex> lock(version_info_mutex_);
        auto it = version_info_.find(rid.page_no);
        if (it == version_info_.end()) {
            if (!prev_version.has_value()) {
                return true;
            }
            page_version_info = std::make_shared<PageVersionInfo>();
            version_info_[rid.page_no] = page_version_info;
        } else {
            page_version_info = it->second;
        }
    }
    
    // 获取槽位版本信息
    std::unique_lock<std::shared_mutex> lock(page_version_info->mutex_);
    auto it = page_version_info->prev_version_.find(rid.slot_no);
    
    // 如果槽位版本信息不存在且不需要创建，直接返回
    if (it == page_version_info->prev_version_.end() && !prev_version.has_value()) {
        return true;
    }
    
    // 检查版本链接
    if (check) {
        std::optional<VersionUndoLink> current_version;
        if (it != page_version_info->prev_version_.end()) {
            current_version = it->second;
        }
        if (!check(current_version)) {
            return false;
        }
    }
    
    // 更新版本链接
    if (prev_version.has_value()) {
        page_version_info->prev_version_[rid.slot_no] = *prev_version;
    } else if (it != page_version_info->prev_version_.end()) {
        page_version_info->prev_version_.erase(it);
    }
    
    return true;
}

/** @brief 获取表堆元组的第一个撤销日志。 */
std::optional<UndoLink> TransactionManager::GetUndoLink(Rid rid) {
    // 获取页面版本信息
    std::shared_ptr<PageVersionInfo> page_version_info;
    {
        std::shared_lock<std::shared_mutex> lock(version_info_mutex_);
        auto it = version_info_.find(rid.page_no);
        if (it == version_info_.end()) {
            return std::nullopt;
        }
        page_version_info = it->second;
    }
    
    // 获取槽位版本信息
    std::shared_lock<std::shared_mutex> lock(page_version_info->mutex_);
    auto it = page_version_info->prev_version_.find(rid.slot_no);
    if (it == page_version_info->prev_version_.end() || !it->second.prev_.IsValid()) {
        return std::nullopt;
    }
    
    return it->second.prev_;
}

/** @brief 获取表堆元组的第一个撤销日志。*/
std::optional<VersionUndoLink> TransactionManager::GetVersionLink(Rid rid) {
    // 获取页面版本信息
    std::shared_ptr<PageVersionInfo> page_version_info;
    {
        std::shared_lock<std::shared_mutex> lock(version_info_mutex_);
        auto it = version_info_.find(rid.page_no);
        if (it == version_info_.end()) {
            return std::nullopt;
        }
        page_version_info = it->second;
    }
    
    // 获取槽位版本信息
    std::shared_lock<std::shared_mutex> lock(page_version_info->mutex_);
    auto it = page_version_info->prev_version_.find(rid.slot_no);
    if (it == page_version_info->prev_version_.end()) {
        return std::nullopt;
    }
    
    return it->second;
}

/** @brief 访问事务撤销日志缓冲区并获取撤销日志。如果事务不存在，返回 nullopt。
 * 如果索引超出范围仍然会抛出异常。 */
std::optional<UndoLog> TransactionManager::GetUndoLogOptional(UndoLink link) {
    // 获取事务
    auto txn = get_transaction(link.prev_txn_);
    if (txn == nullptr) {
        return std::nullopt;
    }
    
    // 获取撤销日志
    if (link.prev_log_idx_ >= static_cast<int>(txn->GetUndoLogNum())) {
        return std::nullopt;
    }
    
    return txn->GetUndoLog(link.prev_log_idx_);
}

/** @brief 访问事务撤销日志缓冲区并获取撤销日志。除非访问当前事务缓冲区，
 * 否则应该始终调用此函数以获取撤销日志，而不是手动检索事务 shared_ptr 并访问缓冲区。 */
UndoLog TransactionManager::GetUndoLog(UndoLink link) {
    auto log_opt = GetUndoLogOptional(link);
    if (!log_opt.has_value()) {
        throw std::runtime_error("Cannot find undo log");
    }
    return *log_opt;
}

/** @brief 获取系统中的最低读时间戳。 */
timestamp_t TransactionManager::GetWatermark() {
    return running_txns_.GetWatermark();
}

/** @brief 垃圾回收。仅在所有事务都未访问时调用。 */
void TransactionManager::GarbageCollection() {
    // 获取水印
    timestamp_t watermark = GetWatermark();
    
    // **修复**：清理已提交和已回滚的事务对象（MVCC模式下的关键修复）
    {
        std::unique_lock<std::mutex> lock(latch_);
        auto it = txn_map.begin();
        while (it != txn_map.end()) {
            Transaction* txn = it->second;
            // 清理已完成的事务（已提交或已回滚），且其时间戳小于等于水印（确保没有其他事务在访问）
            if ((txn->get_state() == TransactionState::COMMITTED || 
                 txn->get_state() == TransactionState::ABORTED) && 
                txn->get_start_ts() <= watermark) {
                
                // 删除事务对象
                delete txn;
                it = txn_map.erase(it);
            } else {
                ++it;
            }
        }
    }
    
    // 清理已回滚事务记录
    {
        std::unique_lock<std::mutex> lock(aborted_txns_mutex_);
        auto it = aborted_txns_.begin();
        while (it != aborted_txns_.end()) {
            if (*it <= watermark) {
                it = aborted_txns_.erase(it);
            } else {
                ++it;
            }
        }
    }
    
    // **新增**：清理已提交事务记录
    {
        std::unique_lock<std::mutex> lock(committed_txns_mutex_);
        auto it = committed_txns_.begin();
        while (it != committed_txns_.end()) {
            if (*it <= watermark) {
                it = committed_txns_.erase(it);
            } else {
                ++it;
            }
        }
    }
    
    // 清理版本信息
    std::unique_lock<std::shared_mutex> lock(version_info_mutex_);
    for (auto it = version_info_.begin(); it != version_info_.end();) {
        auto page_version_info = it->second;
        std::unique_lock<std::shared_mutex> page_lock(page_version_info->mutex_);
        
        // 清理槽位版本信息
        for (auto slot_it = page_version_info->prev_version_.begin(); slot_it != page_version_info->prev_version_.end();) {
            // 获取版本链接
            auto version_link = slot_it->second;
            if (!version_link.prev_.IsValid()) {
                slot_it = page_version_info->prev_version_.erase(slot_it);
                continue;
            }
            
            // 获取撤销日志
            auto log_opt = GetUndoLogOptional(version_link.prev_);
            if (!log_opt.has_value() || log_opt->ts_ <= watermark) {
                slot_it = page_version_info->prev_version_.erase(slot_it);
            } else {
                ++slot_it;
            }
        }
        
        // 如果页面版本信息为空，删除页面版本信息
        if (page_version_info->prev_version_.empty()) {
            it = version_info_.erase(it);
        } else {
            ++it;
        }
    }
}

/** @brief 更新事务提交时的时间戳，将该事务创建的所有版本的时间戳从start_ts更新为commit_ts */
void TransactionManager::UpdateCommitTimestamp(Transaction* txn, timestamp_t commit_ts) {
    if (txn == nullptr) return;
    
    timestamp_t start_ts = txn->get_start_ts();
    
    // 设置事务的提交时间戳
    txn->set_commit_ts(commit_ts);
    
    // 更新该事务的所有撤销日志中的时间戳
    size_t undo_log_num = txn->GetUndoLogNum();
    for (size_t i = 0; i < undo_log_num; i++) {
        auto undo_log = txn->GetUndoLog(i);
        if (undo_log.ts_ == start_ts) {
            undo_log.ts_ = commit_ts;
            txn->ModifyUndoLog(i, undo_log);
        }
    }
    
    // 关键修复：直接使用事务的write_set来更新所有被修改记录的时间戳
    // 这比遍历版本链更直接、更可靠
    auto write_set = txn->get_write_set();
    if (write_set) {
        for (const auto& write_record : *write_set) {
            if (write_record) {
                Rid rid = write_record->GetRid();
                std::string table_name = write_record->GetTableName();
                
                // 获取对应的表文件句柄
                auto fh_it = sm_manager_->fhs_.find(table_name);
                if (fh_it != sm_manager_->fhs_.end()) {
                    auto& fh = fh_it->second;
                    
                    // 创建临时Context来调用UpdateRecordCommitTimestamp
                    Context temp_context(lock_manager_, nullptr, txn);
                    temp_context.txn_mgr_ = this;
                    
                    // 直接更新这个记录的时间戳
                    fh->UpdateRecordCommitTimestamp(rid, commit_ts, &temp_context);
                }
            }
        }
    }
}

/**
 * @description: 创建只读快照事务，用于事务外的SELECT语句
 * @return {Transaction*} 只读快照事务的指针
 * @param {LogManager*} log_manager 日志管理器指针
 */
Transaction* TransactionManager::begin_read_only_snapshot(LogManager* log_manager) {
    // 生成新的事务ID
    txn_id_t new_id = next_txn_id_.fetch_add(1);
    Transaction* txn = new Transaction(new_id);
    
    // 设置为非显式事务模式（自动提交）
    txn->set_txn_mode(false);
    txn->set_state(TransactionState::DEFAULT);

    if (concurrency_mode_ == ConcurrencyMode::MVCC) {
        // **关键修复**：只读快照事务应该使用与普通事务相同的时间戳分配策略
        // 确保看到一致的已提交数据快照
        std::unique_lock<std::mutex> ts_lock(timestamp_mutex_);
        
        // **调试输出**
        timestamp_t current_next = next_timestamp_.load();
        timestamp_t last_commit = last_commit_ts_.load();
        std::cout << "[DEBUG] Read-only Transaction " << txn->get_transaction_id() 
                  << " starting - current_next_ts: " << current_next 
                  << ", last_commit_ts: " << last_commit << std::endl;
        
        timestamp_t read_ts;
        
        // **修复**：只读事务也应该分配唯一的时间戳
        // 但是可以使用最后提交的时间戳来确保看到最新的已提交数据
        if (last_commit > 0) {
            // 使用最后提交的时间戳作为读时间戳，确保看到所有已提交的数据
            read_ts = last_commit;
        } else {
            // 如果没有任何已提交的事务，分配一个新的时间戳
            read_ts = next_timestamp_.fetch_add(1);
        }
        
        txn->set_start_ts(read_ts);
        
        // **调试输出**
        std::cout << "[DEBUG] Read-only Transaction " << txn->get_transaction_id() 
                  << " assigned start_ts: " << read_ts << std::endl;
        
        // **重要**：只读快照事务不需要添加到running_txns_水印中，
        // 因为它们不会创建任何新版本，也不会影响垃圾回收
        // 但如果系统设计需要追踪所有事务，可以选择添加
        
        ts_lock.unlock();
    }

    // 把只读快照事务加入到全局事务表中
    {
        std::unique_lock<std::mutex> lock(latch_);
        txn_map[txn->get_transaction_id()] = txn;
    }

    return txn;
}