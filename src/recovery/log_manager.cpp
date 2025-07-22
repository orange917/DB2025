/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include <cstring>
#include "log_manager.h"

/**
 * @description: 添加日志记录到日志缓冲区中，并返回日志记录号
 * @param {LogRecord*} log_record 要写入缓冲区的日志记录
 * @return {lsn_t} 返回该日志的日志记录号
 */
lsn_t LogManager::add_log_to_buffer(LogRecord* log_record) {
    std::scoped_lock lock{latch_};
    if (log_buffer_.is_full(log_record->log_tot_len_)) {
        flush_log_to_disk();
    }
    log_record->lsn_ = global_lsn_++;
    log_record->serialize(log_buffer_.buffer_ + log_buffer_.offset_);
    log_buffer_.offset_ += log_record->log_tot_len_;
    return log_record->lsn_;
}

/**
 * @description: 把日志缓冲区的内容刷到磁盘中，由于目前只设置了一个缓冲区，因此需要阻塞其他日志操作
 */
void LogManager::flush_log_to_disk() {
    std::lock_guard<std::mutex> lock(latch_);

    if (log_buffer_.offset_ > 0) {
        disk_manager_->write_log(log_buffer_.buffer_, log_buffer_.offset_);
        persist_lsn_ = global_lsn_ - 1;
        log_buffer_.offset_ = 0;
    }
}

/**
 * @description: 强制刷新日志到磁盘（WAL机制的核心）
 * @param {lsn_t} lsn 需要强制刷新的LSN
 */
void LogManager::force_flush_log_to_disk(lsn_t lsn) {
    std::lock_guard<std::mutex> lock(latch_);
    
    // 如果当前持久化的LSN小于目标LSN，需要强制刷新
    if (persist_lsn_ < lsn) {
        if (log_buffer_.offset_ > 0) {
            disk_manager_->write_log(log_buffer_.buffer_, log_buffer_.offset_);
            persist_lsn_ = global_lsn_ - 1;
            log_buffer_.offset_ = 0;
        }
    }
}

/**
 * @description: 获取当前持久化的LSN
 * @return {lsn_t} 当前持久化的LSN
 */
lsn_t LogManager::get_persist_lsn() const {
    return persist_lsn_;
}

/**
 * @description: 获取当前全局LSN
 * @return {lsn_t} 当前全局LSN
 */
lsn_t LogManager::get_global_lsn() const {
    return global_lsn_;
}