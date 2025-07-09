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

#include <atomic>
#include <unordered_map>
#include <optional>
#include <functional>
#include <shared_mutex>
#include <vector>
#include <mutex>
#include <unordered_set>
#include <iostream>

#include "transaction.h"
#include "watermark.h"
#include "recovery/log_manager.h"
#include "concurrency/lock_manager.h"
#include "system/sm_manager.h"
#include "common/exception.h"

/* 系统采用的并发控制算法，当前题目中要求两阶段封锁并发控制算法 */
enum class ConcurrencyMode { TWO_PHASE_LOCKING = 0, BASIC_TO, MVCC };

/// 版本链中的第一个撤销链接，将表堆元组链接到撤销日志。
struct VersionUndoLink {
    /** 版本链中的下一个版本。 */
    UndoLink prev_;
    bool in_progress_{false};

    friend auto operator==(const VersionUndoLink &a, const VersionUndoLink &b) {
        return a.prev_ == b.prev_ && a.in_progress_ == b.in_progress_;
    }

    friend auto operator!=(const VersionUndoLink &a, const VersionUndoLink &b) { return !(a == b); }

    inline static std::optional<VersionUndoLink> FromOptionalUndoLink(std::optional<UndoLink> undo_link) {
        if (undo_link.has_value()) {
            return VersionUndoLink{*undo_link};
        }
        return std::nullopt;
    }
};

class TransactionManager{
public:
    explicit TransactionManager(LockManager *lock_manager, SmManager *sm_manager,
                             ConcurrencyMode concurrency_mode = ConcurrencyMode::TWO_PHASE_LOCKING) {
        sm_manager_ = sm_manager;
        lock_manager_ = lock_manager;
        concurrency_mode_ = concurrency_mode;
        // 从SmManager获取持久化的时间戳作为初始值
        next_timestamp_.store(sm_manager_->get_start_timestamp());
        
        // 设置LockManager的并发控制模式获取函数
        if (lock_manager_) {
            lock_manager_->SetConcurrencyModeGetter([this]() {
                return this->concurrency_mode_;
            });
        }
    }
    
    ~TransactionManager() {
        // 在析构时，将最新的时间戳写回SmManager，以便持久化
        sm_manager_->set_next_timestamp(next_timestamp_.load());
    }

    Transaction* begin(Transaction* txn, LogManager* log_manager);

    /** @brief 创建只读快照事务，用于事务外的SELECT语句 */
    Transaction* begin_read_only_snapshot(LogManager* log_manager);

    void commit(Transaction* txn, LogManager* log_manager);

    void abort(Transaction* txn, LogManager* log_manager);

    ConcurrencyMode get_concurrency_mode() { return concurrency_mode_; }

    void set_concurrency_mode(ConcurrencyMode concurrency_mode) { concurrency_mode_ = concurrency_mode; }

    LockManager* get_lock_manager() { return lock_manager_; }

    /**
     * @description: 获取事务ID为txn_id的事务对象
     * @return {Transaction*} 事务对象的指针
     * @param {txn_id_t} txn_id 事务ID
     */    
     Transaction* get_transaction(txn_id_t txn_id) {
        if(txn_id == INVALID_TXN_ID) return nullptr;
        
        std::unique_lock<std::mutex> lock(latch_);
        auto it = TransactionManager::txn_map.find(txn_id);
        if (it == TransactionManager::txn_map.end()) {
            return nullptr;
        }
        auto *res = it->second;
        lock.unlock();
        assert(res != nullptr);

        // 如果不在MVCC模式下，才需要检查线程ID
        if (concurrency_mode_ != ConcurrencyMode::MVCC) {
            assert(res->get_thread_id() == std::this_thread::get_id());
        }

        return res;
    }

    static std::unordered_map<txn_id_t, Transaction *> txn_map;     // 全局事务表，存放事务ID与事务对象的映射关系
    std::shared_mutex txn_map_mutex_;
    /** ------------------------以下函数仅可能在MVCC当中使用------------------------------------------*/

    /**
    * @brief 更新一个撤销链接，该链接将表堆元组与第一个撤销日志连接起来。
    * 在更新之前，将调用 `check` 函数以确保有效性。
    */
    bool UpdateUndoLink(Rid rid, std::optional<UndoLink> prev_link,
                        std::function<bool(std::optional<UndoLink>)> &&check = nullptr);

    /**
     * @brief 更新一个撤销链接，该链接将表堆元组与第一个撤销日志连接起来。
     * 在更新之前，将调用 `check` 函数以确保有效性。
     */
    bool UpdateVersionLink(Rid rid, std::optional<VersionUndoLink> prev_version,
                           std::function<bool(std::optional<VersionUndoLink>)> &&check = nullptr);

    /** @brief 获取表堆元组的第一个撤销日志。 */
    std::optional<UndoLink> GetUndoLink(Rid rid);

    /** @brief 获取表堆元组的第一个撤销日志。*/
    std::optional<VersionUndoLink> GetVersionLink(Rid rid);

    /** @brief 访问事务撤销日志缓冲区并获取撤销日志。如果事务不存在，返回 nullopt。
     * 如果索引超出范围仍然会抛出异常。 */
    std::optional<UndoLog> GetUndoLogOptional(UndoLink link);

    /** @brief 访问事务撤销日志缓冲区并获取撤销日志。除非访问当前事务缓冲区，
     * 否则应该始终调用此函数以获取撤销日志，而不是手动检索事务 shared_ptr 并访问缓冲区。 */
    UndoLog GetUndoLog(UndoLink link);

    /** @brief 获取系统中的最低读时间戳。 */
    timestamp_t GetWatermark();

    /** @brief 垃圾回收。仅在所有事务都未访问时调用。 */
    void GarbageCollection();

    /** @brief 更新事务提交时的时间戳，将该事务创建的所有版本的时间戳从start_ts更新为commit_ts */
    void UpdateCommitTimestamp(Transaction* txn, timestamp_t commit_ts);

    struct PageVersionInfo {
        std::shared_mutex mutex_;
        /** 存储所有槽的先前版本信息。注意：不要使用 `[x]` 来访问它，因为
         * 即使不存在也会创建新元素。请使用 `find` 来代替。
         */
        std::unordered_map<slot_offset_t, VersionUndoLink> prev_version_;
    };

    /** 保护版本信息 */
    std::shared_mutex version_info_mutex_;
    /** 存储表堆中每个元组的先前版本。 */
    std::unordered_map<page_id_t, std::shared_ptr<PageVersionInfo>> version_info_;

    timestamp_t get_next_timestamp() { return next_timestamp_.load(); }

    /** @brief 获取所有活跃的事务列表（用于MVCC冲突检测） */
    std::vector<Transaction*> GetActiveTransactions() {
        std::vector<Transaction*> active_txns;
        std::unique_lock<std::mutex> lock(latch_);
        for (const auto& pair : txn_map) {
            if (pair.second->get_state() == TransactionState::DEFAULT ||
                pair.second->get_state() == TransactionState::GROWING ||
                pair.second->get_state() == TransactionState::SHRINKING) {
                active_txns.push_back(pair.second);
            }
        }
        return active_txns;
    }

    /** @brief 根据时间戳查找事务 */
    Transaction* get_transaction_by_timestamp(timestamp_t ts) {
        std::unique_lock<std::mutex> lock(latch_);
        for (const auto& pair : txn_map) {
            if (pair.second->get_start_ts() == ts) {
                return pair.second;
            }
        }
        return nullptr;
    }

    /** @brief 检查指定时间戳的事务是否已回滚 */
    bool is_transaction_aborted(timestamp_t ts) {
        std::unique_lock<std::mutex> lock(aborted_txns_mutex_);
        return aborted_txns_.find(ts) != aborted_txns_.end();
    }

    /** @brief 检查指定时间戳的事务是否已提交 */
    bool IsCommitted(timestamp_t ts) {
        // 特殊情况：时间戳为0的版本总是被认为已提交（初始数据）
        if (ts == 0) {
            return true;
        }
        
        // 首先检查是否在已回滚事务列表中
        {
            std::unique_lock<std::mutex> lock(aborted_txns_mutex_);
            if (aborted_txns_.find(ts) != aborted_txns_.end()) {
                return false; // 已回滚的事务
            }
        }
        
        // 查找对应的事务对象
        Transaction* txn = get_transaction_by_timestamp(ts);
        if (txn != nullptr) {
            // 如果事务对象存在，直接检查其状态
            return txn->get_state() == TransactionState::COMMITTED;
        }
        
        // 如果事务对象不存在，可能有以下几种情况：
        // 1. 事务已提交并被清理
        // 2. 事务从未存在
        // 3. 事务已回滚并被清理
        
        // 对于已被清理的事务，我们需要更保守的策略
        // 如果时间戳小于当前最低水印，并且不在已回滚列表中，
        // 则认为已提交（这是安全的假设，因为水印以下的事务都应该已经结束）
        timestamp_t watermark = GetWatermark();
        if (ts < watermark) {
            // 再次检查是否在已回滚事务列表中（双重检查）
            std::unique_lock<std::mutex> lock(aborted_txns_mutex_);
            return aborted_txns_.find(ts) == aborted_txns_.end();
        }
        
        // 对于其他情况，保守地认为未提交
        return false;
    }

    /** @brief 检查指定时间戳的事务是否在给定的读时间戳之前已提交（快照隔离专用） */
    bool IsCommittedBeforeReadTs(timestamp_t ts, timestamp_t read_ts) {
        // **调试输出**
        std::cout << "[DEBUG] IsCommittedBeforeReadTs: checking ts=" << ts << " against read_ts=" << read_ts << std::endl;
        
        // 特殊情况：时间戳为0的版本总是被认为已提交（初始数据）
        if (ts == 0) {
            std::cout << "[DEBUG] ts=0, returning true (initial data)" << std::endl;
            return true;
        }
        
        // **关键修复**：如果创建版本的时间戳大于等于读时间戳，则该版本肯定不可见
        // 这是快照隔离的核心原则：只能看到在事务开始之前的数据
        if (ts >= read_ts) {
            std::cout << "[DEBUG] ts >= read_ts, returning false (future or concurrent data)" << std::endl;
            return false;
        }
        
        // **修复**：首先检查是否在已回滚事务列表中
        {
            std::unique_lock<std::mutex> lock(aborted_txns_mutex_);
            if (aborted_txns_.find(ts) != aborted_txns_.end()) {
                std::cout << "[DEBUG] ts found in aborted list, returning false" << std::endl;
                return false; // 已回滚的事务
            }
        }
        
        // **新增**：检查是否在已提交事务列表中
        {
            std::unique_lock<std::mutex> lock(committed_txns_mutex_);
            std::cout << "[DEBUG] committed_txns_ size: " << committed_txns_.size() << ", checking for ts=" << ts << std::endl;
            if (committed_txns_.find(ts) != committed_txns_.end()) {
                std::cout << "[DEBUG] ts found in committed list, returning true" << std::endl;
                return true; // 已提交的事务
            }
            // **调试**：显示所有已提交事务的时间戳
            std::cout << "[DEBUG] committed_txns_ contents: ";
            for (const auto& committed_ts : committed_txns_) {
                std::cout << committed_ts << " ";
            }
            std::cout << std::endl;
        }
        
        // 查找对应的事务对象
        Transaction* txn = get_transaction_by_timestamp(ts);
        if (txn != nullptr) {
            // 如果事务对象存在，检查其状态
            if (txn->get_state() == TransactionState::COMMITTED) {
                std::cout << "[DEBUG] txn found, state=COMMITTED, returning true" << std::endl;
                return true;
            }
            std::cout << "[DEBUG] txn found but not committed, state=" << static_cast<int>(txn->get_state()) << std::endl;
            return false; // 未提交的事务
        }
        
        // 如果事务对象不存在，可能已被清理
        // 对于已被清理的事务，需要保守处理
        timestamp_t watermark = GetWatermark();
        if (ts <= watermark) {
            // 如果事务时间戳小于等于水印，并且不在已回滚列表中
            // 则认为是已提交的事务（因为只有已提交的事务才会被清理到水印以下）
            std::unique_lock<std::mutex> lock(aborted_txns_mutex_);
            bool result = aborted_txns_.find(ts) == aborted_txns_.end();
            std::cout << "[DEBUG] txn not found but ts <= watermark(" << watermark << "), result=" << result << std::endl;
            return result;
        }
        
        // 其他情况保守地认为不可见
        std::cout << "[DEBUG] default case, returning false" << std::endl;
        return false;
    }

private:
    ConcurrencyMode concurrency_mode_;      // 事务使用的并发控制算法，目前只需要考虑2PL
    std::atomic<txn_id_t> next_txn_id_{0};  // 用于分发事务ID
    std::atomic<timestamp_t> next_timestamp_;    // 用于分发事务时间戳
    std::mutex latch_;  // 用于txn_map的并发
    std::mutex timestamp_mutex_;  // 用于保证时间戳分配的原子性，避免时间戳乱序
    SmManager *sm_manager_;
    LockManager *lock_manager_;

    std::atomic<timestamp_t> last_commit_ts_{0};    // 最后提交的时间戳,仅用于MVCC
    Watermark running_txns_{0};             // 存储所有正在运行事务的读取时间戳，以便于垃圾回收，仅用于MVCC
    
    // 新增：用于跟踪已回滚事务的时间戳，避免在可见性检查时误判为已提交
    std::mutex aborted_txns_mutex_;
    std::unordered_set<timestamp_t> aborted_txns_;  // 存储已回滚事务的时间戳
    
    // **新增**：用于跟踪已提交事务的时间戳，提高可见性检查的准确性
    std::mutex committed_txns_mutex_;
    std::unordered_set<timestamp_t> committed_txns_;  // 存储已提交事务的时间戳
};