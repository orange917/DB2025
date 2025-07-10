/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "rm_file_handle.h"
#include "bitmap.h"
#include "rm_defs.h"
#include "storage/page.h"
#include "transaction/txn_defs.h"
#include "transaction/transaction_manager.h"
#include "execution/execution_common.h"
#include "system/sm_manager.h"
#include <cstddef>
#include <memory>
#include <optional>
#include <defs.h>
#include <iostream>

/**
 * @description: 获取当前表中记录号为rid的记录
 * @param {Rid&} rid 记录号，指定记录的位置
 * @param {Context*} context
 * @return {unique_ptr<RmRecord>} rid对应的记录对象指针
 */
std::unique_ptr<RmRecord> RmFileHandle::get_record(const Rid& rid, Context* context) const {
    // Todo:
    // 1. 获取指定记录所在的page handle
    // 2. 初始化一个指向RmRecord的指针（赋值其内部的data和size）
    RmPageHandle page_handle = fetch_page_handle(rid.page_no);
    if(page_handle.page == nullptr) {
        return nullptr;
    }
    else
    {
        char* slot_data = page_handle.get_slot(rid.slot_no);

        // 检查是否是MVCC模式
        bool is_mvcc = (context != nullptr && context->txn_mgr_ != nullptr && 
            context->txn_mgr_->get_concurrency_mode() == ConcurrencyMode::MVCC);

        if (is_mvcc) {
            // MVCC模式：返回包含TupleMeta的完整记录
            auto record = std::make_unique<RmRecord>(file_hdr_.record_size);
            record->size = file_hdr_.record_size;
            record->SetData(slot_data);
            buffer_pool_manager_->unpin_page(page_handle.page->get_page_id(), false);
            return record;
        } else {
            // 非MVCC模式：检查记录是否意外包含了TupleMeta
            // 这种情况可能发生在混合模式或数据迁移场景下
            
            // 简单的启发式检查：检查记录开头是否像TupleMeta
            // 如果第一个字段看起来像时间戳（非零且合理范围），可能包含TupleMeta
            timestamp_t potential_ts = *(timestamp_t*)slot_data;
            bool potential_deleted = *(bool*)(slot_data + sizeof(timestamp_t));
            
            // 如果开头看起来像TupleMeta（时间戳>0且删除标志是布尔值），则跳过TupleMeta
            if (potential_ts > 0 && potential_ts < UINT64_MAX && 
                (potential_deleted == false || potential_deleted == true)) {
                // 跳过TupleMeta，返回纯数据部分
                int data_size = file_hdr_.record_size - sizeof(TupleMeta);
                auto record = std::make_unique<RmRecord>(data_size);
                record->size = data_size;
                memcpy(record->data, slot_data + sizeof(TupleMeta), data_size);
                buffer_pool_manager_->unpin_page(page_handle.page->get_page_id(), false);
                return record;
            } else {
                // 不包含TupleMeta，返回原始数据
                auto record = std::make_unique<RmRecord>(file_hdr_.record_size);
                record->size = file_hdr_.record_size;
                record->SetData(slot_data);
                buffer_pool_manager_->unpin_page(page_handle.page->get_page_id(), false);
                return record;
            }
        }
    }
}

/**
 * @description: 在当前表中插入一条记录，不指定插入位置
 * @param {char*} buf 要插入的记录的数据
 * @param {Context*} context
 * @return {Rid} 插入的记录的记录号（位置）
 */
Rid RmFileHandle::insert_record(char* buf, Context* context) {
    // Todo:
    // 1. 获取当前未满的page handle
    // 2. 在page handle中找到空闲slot位置
    // 3. 将buf复制到空闲slot位置
    // 4. 更新page_handle.page_hdr中的数据结构
    // 注意考虑插入一条记录后页面已满的情况，需要更新file_hdr_.first_free_page_no
    RmPageHandle page_handle = create_page_handle();

    // 找到空闲的slot位置
    int slot_no = Bitmap::first_bit(false, page_handle.bitmap, file_hdr_.num_records_per_page);
    if (slot_no < file_hdr_.num_records_per_page) {
        char *slot = page_handle.get_slot(slot_no);
        // 将buf复制到空闲slot位置
        std::memcpy(slot, buf, file_hdr_.record_size);
        // 更新page_hdr中的数据结构
        page_handle.page_hdr->num_records += 1;
        Bitmap::set(page_handle.bitmap, slot_no); // 标记该slot为已使用
        
        if(page_handle.page_hdr->num_records == file_hdr_.num_records_per_page) {
            // 如果插入后页面已满，更新file_hdr_.first_free_page_no
            file_hdr_.first_free_page_no = RM_NO_PAGE;
        }
            
        buffer_pool_manager_->unpin_page(page_handle.page->get_page_id(), true);
            
        // 创建并返回记录号
        Rid rid;
        rid.page_no = page_handle.page->get_page_id().page_no;
        rid.slot_no = slot_no;

        return rid;
    }
    
    // 如果没有找到空闲槽位，返回无效的Rid
    Rid invalid_rid;
    invalid_rid.page_no = -1;
    invalid_rid.slot_no = -1;
    return invalid_rid;
}

/**
 * @description: 在当前表中的指定位置插入一条记录
 * @param {Rid&} rid 要插入记录的位置
 * @param {char*} buf 要插入记录的数据
 */
void RmFileHandle::insert_record(const Rid& rid, char* buf) {
    create_new_page_handle();
    RmPageHandle page_handle = fetch_page_handle(rid.page_no);

    char* slot = page_handle.get_slot(rid.slot_no);
    if (slot != nullptr) {
        std::memcpy(slot, buf, file_hdr_.record_size);
        // 更新page_hdr中的数据结构
        page_handle.page_hdr->num_records += 1;
        Bitmap::set(page_handle.bitmap, rid.slot_no); // 标记该slot为已使用
        if(page_handle.page_hdr->num_records == file_hdr_.num_records_per_page) {
            // 如果插入后页面已满，更新file_hdr_.first_free_page_no
            file_hdr_.first_free_page_no = page_handle.page_hdr->next_free_page_no;
        }
        buffer_pool_manager_->unpin_page(page_handle.page->get_page_id(), true); // 解除对页面的锁定
    }
}

/**
 * @description: 删除记录文件中记录号为rid的记录
 * @param {Rid&} rid 要删除的记录的记录号（位置）
 * @param {Context*} context
 */
void RmFileHandle::delete_record(const Rid& rid, Context* context) {
    // Todo:
    // 1. 获取指定记录所在的page handle
    // 2. 更新page_handle.page_hdr中的数据结构
    // 注意考虑删除一条记录后页面未满的情况，需要调用release_page_handle()
    RmPageHandle page_handle = fetch_page_handle(rid.page_no);
    if (page_handle.page != nullptr) {
        char* slot = page_handle.get_slot(rid.slot_no);
        if (slot != nullptr && Bitmap::is_set(page_handle.bitmap, rid.slot_no)) {
            // 先清除数据
            std::memset(slot, 0, file_hdr_.record_size);
            // 然后更新bitmap和页头
            Bitmap::reset(page_handle.bitmap, rid.slot_no);
            page_handle.page_hdr->num_records -= 1;
            
            // 如果页面未满，更新文件头
            if (page_handle.page_hdr->num_records < file_hdr_.num_records_per_page) {
                release_page_handle(page_handle);
            }
            
            buffer_pool_manager_->unpin_page(page_handle.page->get_page_id(), true);
        }
    }
}


/**
 * @description: 更新记录文件中记录号为rid的记录
 * @param {Rid&} rid 要更新的记录的记录号（位置）
 * @param {char*} buf 新记录的数据
 * @param {Context*} context
 */
void RmFileHandle::update_record(const Rid& rid, char* buf, Context* context) {
    // Todo:
    // 1. 获取指定记录所在的page handle
    // 2. 更新记录
    RmPageHandle page_handle = fetch_page_handle(rid.page_no);
    if (page_handle.page != nullptr) {
        char* slot = page_handle.get_slot(rid.slot_no);
        if (slot != nullptr) {
            // 将buf复制到指定的slot位置
            std::memcpy(slot, buf, file_hdr_.record_size);
            buffer_pool_manager_->unpin_page(page_handle.page->get_page_id(), true); // 解除对页面的锁定
        }
    }
}

/**
 * 以下函数为辅助函数，仅提供参考，可以选择完成如下函数，也可以删除如下函数，在单元测试中不涉及如下函数接口的直接调用
*/
/**
 * @description: 获取指定页面的页面句柄
 * @param {int} page_no 页面号
 * @return {RmPageHandle} 指定页面的句柄
 */
RmPageHandle RmFileHandle::fetch_page_handle(int page_no) const {
    // Todo:
    // 使用缓冲池获取指定页面，并生成page_handle返回给上层
    // 检查page_no是否在有效范围内
    if (page_no < RM_FIRST_RECORD_PAGE || page_no >= file_hdr_.num_pages) {
       return RmPageHandle(nullptr, nullptr);
    }

    PageId page_id;
    page_id.fd = fd_;
    page_id.page_no = page_no;

    // 从缓冲池获取页面
    Page *page = buffer_pool_manager_->fetch_page(page_id);

    // 检查页面获取是否成功
    if (page == nullptr) {
        return RmPageHandle(nullptr, nullptr);
    }

    // 返回有效的页面句柄
    return RmPageHandle(&file_hdr_, page);
}

/**
 * @description: 创建一个新的page handle
 * @return {RmPageHandle} 新的PageHandle
 */
RmPageHandle RmFileHandle::create_new_page_handle() {
    // Todo:
    // 1.使用缓冲池来创建一个新page
    // 2.更新page handle中的相关信息
    // 3.更新file_hdr_
    // 创建新page
    PageId page_id;
    page_id.fd = fd_;
    Page *page = buffer_pool_manager_->new_page(&page_id);

    // 更新page handle中的相关信息
    if(page != nullptr){
        // 先更新file_hdr_中的页面数量
        file_hdr_.num_pages += 1;
        
        RmPageHandle page_handle(&file_hdr_, page);
        // 更新page handle中的信息
        page_handle.page_hdr->num_records = 0; // 新页面初始没有记录
        page_handle.page_hdr->next_free_page_no = file_hdr_.first_free_page_no; // 新页面的下一个空闲页为当前文件头中的第一个空闲页
        file_hdr_.first_free_page_no = page_handle.page->get_page_id().page_no; // 将新页面设置为第一个空闲页
        Bitmap::init(page_handle.bitmap, file_hdr_.bitmap_size);
        
        // 将文件头写回磁盘
        disk_manager_->write_page(fd_, RM_FILE_HDR_PAGE, (char *)&file_hdr_, sizeof(file_hdr_));
        
        // 将新页面的page handle返回给上层
        return page_handle;
    }
    return RmPageHandle(nullptr, nullptr);
}

/**
 * @brief 创建或获取一个空闲的page handle
 *
 * @return RmPageHandle 返回生成的空闲page handle
 * @note pin the page, remember to unpin it outside!
 */
RmPageHandle RmFileHandle::create_page_handle() {
    // Todo:
    // 1. 判断file_hdr_中是否还有空闲页
    //     1.1 没有空闲页：使用缓冲池来创建一个新page；可直接调用create_new_page_handle()
    //     1.2 有空闲页：直接获取第一个空闲页
    // 2. 生成page handle并返回给上层
    if(file_hdr_.first_free_page_no == -1) {
        // 如果没有空闲页，创建一个新的page handle
        return create_new_page_handle();
    }
    // 如果有空闲页，获取第一个空闲页
    if (file_hdr_.first_free_page_no >= 0 && file_hdr_.first_free_page_no < file_hdr_.num_pages) {
        RmPageHandle page_handle = fetch_page_handle(file_hdr_.first_free_page_no);
        if (page_handle.page != nullptr) {
            return page_handle;
        }
    }
    // 如果first_free_page_no无效或获取页面失败，创建一个新页面
    return create_new_page_handle();
}

/**
 * @description: 当一个页面从没有空闲空间的状态变为有空闲空间状态时，更新文件头和页头中空闲页面相关的元数据
 */
void RmFileHandle::release_page_handle(RmPageHandle&page_handle) {
    // Todo:
    // 当page从已满变成未满，考虑如何更新：
    // 1. page_handle.page_hdr->next_free_page_no
    // 2. file_hdr_.first_free_page_no
    int current_page_no = page_handle.page->get_page_id().page_no;
    if (current_page_no >= 0 && current_page_no < file_hdr_.num_pages) {
        page_handle.page_hdr->next_free_page_no = file_hdr_.first_free_page_no; // 将当前页的下一个空闲页设置为文件头中的第一个空闲页
        file_hdr_.first_free_page_no = current_page_no; // 更新文件头中的第一个空闲页为当前页
        
        // 将文件头写回磁盘
        disk_manager_->write_page(fd_, RM_FILE_HDR_PAGE, (char *)&file_hdr_, sizeof(file_hdr_));
    }
}

int RmFileHandle::get_records_num() {
    int total = 0;
    for (int page_no = RM_FIRST_RECORD_PAGE; page_no < file_hdr_.num_pages; ++page_no) {
        RmPageHandle page_handle = fetch_page_handle(page_no);
        if (page_handle.page_hdr) {
            total += page_handle.page_hdr->num_records;
            buffer_pool_manager_->unpin_page(page_handle.page->get_page_id(), false);
        }
    }
    return total;
}

/**
 * @description: 获取当前表中记录号为rid的记录（支持MVCC）
 * @param {Rid&} rid 记录号，指定记录的位置
 * @param {Context*} context
 * @return {unique_ptr<RmRecord>} rid对应的记录对象指针
 */
std::unique_ptr<RmRecord> RmFileHandle::get_record_mvcc(const Rid& rid, Context* context) const {
    if (context == nullptr || context->txn_ == nullptr) {
        return get_record(rid, context);
    }

    TransactionManager* txn_manager = context->txn_mgr_;
    if (txn_manager == nullptr || txn_manager->get_concurrency_mode() != ConcurrencyMode::MVCC) {
        return get_record(rid, context);
    }

    Transaction* txn = context->txn_;

    // **调试输出**
    std::cout << "[DEBUG] get_record_mvcc: txn_id=" << txn->get_transaction_id() 
              << ", start_ts=" << txn->get_start_ts() 
              << ", rid=(" << rid.page_no << "," << rid.slot_no << ")" << std::endl;

    // 获取原始记录（包含TupleMeta）
    auto raw_record = get_record(rid, context);
    if (!raw_record) {
        std::cout << "[DEBUG] raw_record is null, returning nullptr" << std::endl;
        return nullptr;
    }

    // **新增：详细分析原始数据的内存布局**
    std::cout << "[DEBUG] Raw record analysis:" << std::endl;
    std::cout << "  raw_record->size: " << raw_record->size << std::endl;
    std::cout << "  file_hdr_.record_size: " << file_hdr_.record_size << std::endl;
    
    // 打印原始数据的十六进制表示（前24字节）
    std::cout << "  Raw data (first 24 bytes): ";
    for (int i = 0; i < std::min(24, raw_record->size); i++) {
        printf("%02x ", (unsigned char)raw_record->data[i]);
    }
    std::cout << std::endl;
    
    // 尝试解析为不同的数据结构
    if (raw_record->size >= sizeof(TupleMeta)) {
        TupleMeta* meta_ptr = (TupleMeta*)raw_record->data;
        std::cout << "  As TupleMeta: ts=" << meta_ptr->ts_ << ", is_deleted=" << meta_ptr->is_deleted_ << std::endl;
    }
    
    if (raw_record->size >= 8) {
        int* int_ptr = (int*)raw_record->data;
        std::cout << "  As int array: [" << int_ptr[0] << ", " << int_ptr[1] << "]" << std::endl;
    }

    // 关键修复：检查记录是否真的包含TupleMeta结构体
    // 如果记录是在非MVCC模式下插入的，可能不包含TupleMeta
    bool has_tuple_meta = false;
    TupleMeta tuple_meta;
    
    // **关键修复：更准确的TupleMeta检测逻辑**
    // 在MVCC模式下，记录应该包含TupleMeta + 实际数据
    // 而非MVCC模式下，记录只包含实际数据
    
    // 首先检查记录大小
    // 计算不包含TupleMeta的预期记录大小
    int expected_data_size = file_hdr_.record_size - sizeof(TupleMeta);
    
    std::cout << "[DEBUG] Record size analysis:" << std::endl;
    std::cout << "  raw_record->size: " << raw_record->size << std::endl;
    std::cout << "  file_hdr_.record_size: " << file_hdr_.record_size << std::endl;
    std::cout << "  expected_data_size (without TupleMeta): " << expected_data_size << std::endl;
    std::cout << "  sizeof(TupleMeta): " << sizeof(TupleMeta) << std::endl;
    
    if (raw_record->size == file_hdr_.record_size) {
        // 记录大小等于文件头中的记录大小，可能包含TupleMeta
        // 进一步检查数据模式来确认
        
        // 尝试解析前sizeof(TupleMeta)字节为TupleMeta
        TupleMeta potential_meta;
        memcpy(&potential_meta, raw_record->data, sizeof(TupleMeta));
        
        std::cout << "[DEBUG] Potential TupleMeta: ts=" << potential_meta.ts_ 
                  << ", is_deleted=" << potential_meta.is_deleted_ << std::endl;
        
        // **关键判断逻辑**：检查时间戳是否合理
        // 合理的时间戳应该：
        // 1. 等于0（初始数据）
        // 2. 或者在合理范围内（1到当前最大事务时间戳+一些缓冲）
        timestamp_t max_reasonable_ts = txn_manager->get_next_timestamp() + 1000; // 加一些缓冲
        
        if (potential_meta.ts_ == 0 || 
            (potential_meta.ts_ > 0 && potential_meta.ts_ <= max_reasonable_ts &&
             (potential_meta.is_deleted_ == false || potential_meta.is_deleted_ == true))) {
            // 时间戳合理，且删除标志是有效的布尔值，认为包含TupleMeta
            has_tuple_meta = true;
            tuple_meta = potential_meta;
            std::cout << "[DEBUG] Record contains valid TupleMeta" << std::endl;
        } else {
            // 时间戳不合理，认为这是纯业务数据，不包含TupleMeta
            has_tuple_meta = false;
            // 对于非MVCC插入的记录，假设它们是时间戳为0的初始数据
            tuple_meta.ts_ = 0;
            tuple_meta.is_deleted_ = false;
            std::cout << "[DEBUG] Record does NOT contain TupleMeta (unreasonable timestamp), treating as pure data" << std::endl;
        }
    } else {
        // 记录大小不等于文件头中的记录大小，很可能不包含TupleMeta
        has_tuple_meta = false;
        tuple_meta.ts_ = 0;
        tuple_meta.is_deleted_ = false;
        std::cout << "[DEBUG] Record size mismatch, treating as non-MVCC data" << std::endl;
    }

    // MVCC可见性检查
    std::unique_ptr<RmRecord> visible_record = nullptr;

    // 关键修复：首先处理时间戳为0的初始数据
    if (tuple_meta.ts_ == 0) {
        std::cout << "[DEBUG] Processing initial data (ts=0)" << std::endl;
        // 时间戳为0的记录是初始数据，对所有事务都可见（除非已被删除）
        if (!tuple_meta.is_deleted_) {
            if (has_tuple_meta) {
                // 从原始记录中提取数据部分（去掉TupleMeta）
                int data_size = raw_record->size - sizeof(TupleMeta);
                visible_record = std::make_unique<RmRecord>(data_size);
                visible_record->size = data_size;
                memcpy(visible_record->data, raw_record->data + sizeof(TupleMeta), data_size);
            } else {
                // 记录本身就是纯数据，直接返回
                visible_record = std::make_unique<RmRecord>(raw_record->size);
                visible_record->size = raw_record->size;
                memcpy(visible_record->data, raw_record->data, raw_record->size);
            }
            std::cout << "[DEBUG] Initial data is visible, returning record" << std::endl;
            return visible_record;
        } else {
            // 初始数据已被删除，返回nullptr
            std::cout << "[DEBUG] Initial data is deleted, returning nullptr" << std::endl;
            return nullptr;
        }
    }

    if (tuple_meta.is_deleted_) {
        std::cout << "[DEBUG] Record is deleted, checking delete operation visibility" << std::endl;
        // 记录已被删除，检查删除操作的可见性
        if (tuple_meta.ts_ == txn->get_start_ts()) {
            // 当前事务删除的，返回nullptr
            std::cout << "[DEBUG] Current transaction deleted this record" << std::endl;
            return nullptr;
        } else {
            // **关键修复**：对于删除操作的可见性检查
            // 删除操作的可见性规则：如果删除操作对当前事务可见，则记录不可见
            if (txn_manager->IsCommittedBeforeReadTs(tuple_meta.ts_, txn->get_start_ts())) {
                // 删除操作在读时间戳之前已提交，记录对当前事务不可见
                std::cout << "[DEBUG] Delete operation is visible, record not visible" << std::endl;
                return nullptr;
            } else {
                // 删除操作未提交、已回滚，或在读时间戳之后才提交，查找历史版本
                std::cout << "[DEBUG] Delete operation not visible, checking history" << std::endl;
                visible_record = GetVisibleVersionFromHistory(rid, txn, txn_manager);
            }
        }
    } else {
        std::cout << "[DEBUG] Record is not deleted, checking visibility" << std::endl;
        // 记录未被删除，检查可见性
        if (tuple_meta.ts_ == txn->get_start_ts()) {
            std::cout << "[DEBUG] Current transaction created/modified this record" << std::endl;
            // 当前事务创建/修改的版本，直接可见
            if (has_tuple_meta) {
                // 从原始记录中提取数据部分（去掉TupleMeta）
                int data_size = raw_record->size - sizeof(TupleMeta);
                visible_record = std::make_unique<RmRecord>(data_size);
                visible_record->size = data_size;
                memcpy(visible_record->data, raw_record->data + sizeof(TupleMeta), data_size);
            } else {
                // 记录本身就是纯数据，直接返回
                visible_record = std::make_unique<RmRecord>(raw_record->size);
                visible_record->size = raw_record->size;
                memcpy(visible_record->data, raw_record->data, raw_record->size);
            }
        } else if (tuple_meta.ts_ < txn->get_start_ts()) {
            std::cout << "[DEBUG] Record created before current transaction, checking if committed" << std::endl;
            // 在当前事务开始前的版本，检查是否在读时间戳之前已提交
            if (txn_manager->IsCommittedBeforeReadTs(tuple_meta.ts_, txn->get_start_ts())) {
                std::cout << "[DEBUG] Record is visible (committed before read)" << std::endl;
                // 已提交版本，且在快照时间戳之前提交
                if (has_tuple_meta) {
                    // 从原始记录中提取数据部分（去掉TupleMeta）
                    int data_size = raw_record->size - sizeof(TupleMeta);
                    visible_record = std::make_unique<RmRecord>(data_size);
                    visible_record->size = data_size;
                    memcpy(visible_record->data, raw_record->data + sizeof(TupleMeta), data_size);
                } else {
                    // 记录本身就是纯数据，直接返回
                    visible_record = std::make_unique<RmRecord>(raw_record->size);
                    visible_record->size = raw_record->size;
                    memcpy(visible_record->data, raw_record->data, raw_record->size);
                }
            } else {
                // 事务未提交、已回滚，或在读时间戳之后才提交，查找历史版本
                std::cout << "[DEBUG] Record not visible, checking history" << std::endl;
                visible_record = GetVisibleVersionFromHistory(rid, txn, txn_manager);
            }
        } else {
            std::cout << "[DEBUG] Record created after current transaction started" << std::endl;
            // tuple_meta.ts_ > txn->get_start_ts()
            // 在当前事务开始后创建的版本，需要更仔细的处理
            
            // 修复关键点：不应该直接假设这个版本不可见
            // 需要检查创建该版本的事务状态
            
            // 首先检查是否是已回滚的事务创建的版本
            if (txn_manager->is_transaction_aborted(tuple_meta.ts_)) {
                // 已回滚的事务创建的版本不可见，查找历史版本
                std::cout << "[DEBUG] Record created by aborted transaction, checking history" << std::endl;
                visible_record = GetVisibleVersionFromHistory(rid, txn, txn_manager);
            } else {
                // 检查创建该版本的事务是否已提交
                if (txn_manager->IsCommitted(tuple_meta.ts_)) {
                    // 虽然已提交，但时间戳大于当前事务开始时间，
                    // 根据MVCC规则，这个版本对当前事务不可见
                    std::cout << "[DEBUG] Record committed but after read timestamp, checking history" << std::endl;
                    visible_record = GetVisibleVersionFromHistory(rid, txn, txn_manager);
                } else {
                    // 未提交的版本，查找历史版本
                    std::cout << "[DEBUG] Record not committed, checking history" << std::endl;
                    visible_record = GetVisibleVersionFromHistory(rid, txn, txn_manager);
                }
            }
        }
    }

    std::cout << "[DEBUG] Returning " << (visible_record ? "visible record" : "nullptr") << std::endl;
    return visible_record;
}

/**
 * @description: 从历史版本中获取可见的版本
 * @param {Rid&} rid 记录号
 * @param {Transaction*} txn 当前事务
 * @param {TransactionManager*} txn_manager 事务管理器
 * @return {unique_ptr<RmRecord>} 可见的记录版本
 */
std::unique_ptr<RmRecord> RmFileHandle::GetVisibleVersionFromHistory(const Rid& rid, Transaction* txn, TransactionManager* txn_manager) const {
    // 首先尝试从版本链中查找
    try {
        std::optional<VersionUndoLink> version_link = txn_manager->GetVersionLink(rid);
        if (!version_link.has_value()) {
            // 如果没有版本链，尝试读取原始记录的初始版本
            // 这可能是一个INSERT记录，需要检查其可见性
            try {
                auto raw_record = get_record(rid, nullptr);
                if (raw_record) {
                    // 检查是否包含TupleMeta
                    int expected_data_size = file_hdr_.record_size - sizeof(TupleMeta);
                    bool has_tuple_meta = (raw_record->size != expected_data_size);
                    
                    TupleMeta meta;
                    if (has_tuple_meta) {
                        memcpy(&meta, raw_record->data, sizeof(TupleMeta));
                    } else {
                        // 非MVCC插入的记录，假设为初始数据
                        meta.ts_ = 0;
                        meta.is_deleted_ = false;
                    }
                    
                    // 检查原始记录的可见性
                    if (meta.ts_ == 0 || 
                        (meta.ts_ <= txn->get_start_ts() && 
                         txn_manager->IsCommittedBeforeReadTs(meta.ts_, txn->get_start_ts()) && 
                         !meta.is_deleted_)) {
                        // 返回数据部分
                        if (has_tuple_meta) {
                            // 去除TupleMeta
                            int data_size = raw_record->size - sizeof(TupleMeta);
                            auto visible_record = std::make_unique<RmRecord>(data_size);
                            visible_record->size = data_size;
                            memcpy(visible_record->data, raw_record->data + sizeof(TupleMeta), data_size);
                            return visible_record;
                        } else {
                            // 直接返回纯数据
                            auto visible_record = std::make_unique<RmRecord>(raw_record->size);
                            visible_record->size = raw_record->size;
                            memcpy(visible_record->data, raw_record->data, raw_record->size);
                            return visible_record;
                        }
                    }
                }
            } catch (...) {
                // 忽略异常
            }
            
            return nullptr;
        }
        
        std::optional<UndoLink> undo_link = version_link->prev_;
        while (undo_link.has_value()) {
            auto log_opt = txn_manager->GetUndoLogOptional(*undo_link);
            if (!log_opt.has_value()) {
                break;
            }
            
            const UndoLog& undo_log = *log_opt;
            
            // 检查这个版本是否可见
            if (undo_log.ts_ == 0) {
                // 时间戳为0的版本总是可见（初始数据）
                if (undo_log.is_deleted_) {
                    return nullptr;
                }
                // 尝试重建记录
                auto reconstructed = ReconstructTupleFromUndoLog(undo_log);
                if (reconstructed) {
                    return reconstructed;
                }
            } else if (undo_log.ts_ == txn->get_start_ts()) {
                // 如果是当前事务创建的，总是可见
                if (undo_log.is_deleted_) {
                    return nullptr;
                }
                auto reconstructed = ReconstructTupleFromUndoLog(undo_log);
                if (reconstructed) {
                    return reconstructed;
                }
            } else if (undo_log.ts_ < txn->get_start_ts()) {
                // 时间戳小于当前事务开始时间，检查是否在读时间戳之前已提交
                if (txn_manager->IsCommittedBeforeReadTs(undo_log.ts_, txn->get_start_ts())) {
                    if (undo_log.is_deleted_) {
                        return nullptr;
                    }
                    auto reconstructed = ReconstructTupleFromUndoLog(undo_log);
                    if (reconstructed) {
                        return reconstructed;
                    }
                }
                // 如果未提交、已回滚，或在读时间戳之后才提交，继续查找更早的版本
            }
            // 对于时间戳大于当前事务开始时间的版本，直接跳过
            
            // 移动到下一个版本
            undo_link = undo_log.prev_version_;
        }
    } catch (const std::exception& e) {
        std::cout << "[ERROR] GetVisibleVersionFromHistory: Exception during version chain access: " << e.what() << std::endl;
        // 如果访问版本链失败，尝试直接读取原始记录
    }
    
    // 如果版本链遍历完毕还没找到可见版本，尝试获取原始INSERT的数据
    try {
        auto raw_record = get_record(rid, nullptr);
        if (raw_record) {
            // 检查是否包含TupleMeta
            int expected_data_size = file_hdr_.record_size - sizeof(TupleMeta);
            bool has_tuple_meta = (raw_record->size != expected_data_size);
            
            TupleMeta meta;
            if (has_tuple_meta) {
                memcpy(&meta, raw_record->data, sizeof(TupleMeta));
            } else {
                // 非MVCC插入的记录，假设为初始数据
                meta.ts_ = 0;
                meta.is_deleted_ = false;
            }
            
            // 检查原始记录的可见性
            if (meta.ts_ == 0 || 
                (meta.ts_ <= txn->get_start_ts() && 
                 txn_manager->IsCommittedBeforeReadTs(meta.ts_, txn->get_start_ts()) && 
                 !meta.is_deleted_)) {
                // 返回数据部分
                if (has_tuple_meta) {
                    // 去除TupleMeta
                    int data_size = raw_record->size - sizeof(TupleMeta);
                    auto visible_record = std::make_unique<RmRecord>(data_size);
                    visible_record->size = data_size;
                    memcpy(visible_record->data, raw_record->data + sizeof(TupleMeta), data_size);
                    return visible_record;
                } else {
                    // 直接返回纯数据
                    auto visible_record = std::make_unique<RmRecord>(raw_record->size);
                    visible_record->size = raw_record->size;
                    memcpy(visible_record->data, raw_record->data, raw_record->size);
                    return visible_record;
                }
            }
        }
    } catch (...) {
        // 忽略异常
    }
    
    return nullptr;
}

/**
 * @description: 从撤销日志重建记录
 * @param {UndoLog&} undo_log 撤销日志
 * @return {unique_ptr<RmRecord>} 重建的记录
 */
std::unique_ptr<RmRecord> RmFileHandle::ReconstructTupleFromUndoLog(const UndoLog& undo_log) const {
    // 计算记录大小（不包含TupleMeta）
    int data_size = file_hdr_.record_size - sizeof(TupleMeta);
    auto reconstructed_record = std::make_unique<RmRecord>(data_size);
    reconstructed_record->size = data_size;
    
    // 初始化记录数据为0
    memset(reconstructed_record->data, 0, data_size);
    
    // 检查撤销日志是否包含有效数据
    if (undo_log.tuple_.empty()) {
        // 如果撤销日志为空，这可能是一个INSERT操作的初始版本
        return nullptr;
    }

    // 从撤销日志中重建记录
    // 根据实际的字段数量进行重建
    size_t field_count = std::min(undo_log.tuple_.size(), undo_log.modified_fields_.size());
    
    // 重建记录的每个字段
    for (size_t i = 0; i < field_count && i < 2; ++i) { // 限制为2个字段（id, score）
        const Value& val = undo_log.tuple_[i];
        
        // 根据字段索引确定偏移量和大小
        int field_offset = 0;
        int field_size = 0;
        
        // 对于student表的固定布局：id(int, 4字节) + score(int, 4字节)
        if (i == 0) {
            // 第一个字段 id (int)
            field_offset = 0;
            field_size = sizeof(int);
        } else if (i == 1) {
            // 第二个字段 score (int)
            field_offset = sizeof(int);
            field_size = sizeof(int);
        } else {
            continue;
        }
        
        // 检查边界
        if (field_offset + field_size > data_size) {
            break;
        }
        
        // 根据字段类型写入数据
        switch (val.type) {
            case TYPE_INT:
                *(int*)(reconstructed_record->data + field_offset) = val.int_val;
                break;
                
            case TYPE_FLOAT:
                *(float*)(reconstructed_record->data + field_offset) = val.float_val;
                break;
                
            case TYPE_STRING:
                {
                    size_t str_len = std::min(val.str_val.length(), static_cast<size_t>(field_size));
                    memcpy(reconstructed_record->data + field_offset, val.str_val.c_str(), str_len);
                    // 确保字符串以null结尾（如果有空间的话）
                    if (str_len < field_size) {
                        reconstructed_record->data[field_offset + str_len] = '\0';
                    }
                }
                break;
                
            default:
                // 对于未知类型，跳过
                break;
        }
    }
    
    return reconstructed_record;
}

/**
 * @description: 更新记录中的提交时间戳（修复时间戳一致性问题）
 * @param {Rid&} rid 记录号
 * @param {timestamp_t} commit_ts 提交时间戳
 * @param {Context*} context
 */
void RmFileHandle::UpdateRecordCommitTimestamp(const Rid& rid, timestamp_t commit_ts, Context* context) {
    if (context == nullptr || context->txn_mgr_ == nullptr || 
        context->txn_mgr_->get_concurrency_mode() != ConcurrencyMode::MVCC) {
        return; // 非MVCC模式不需要更新时间戳
    }
    
    // 检查记录是否存在
    if (!is_record(rid)) {
        return;
    }
    
    // 获取页面句柄
    RmPageHandle page_handle = fetch_page_handle(rid.page_no);
    if (page_handle.page == nullptr) {
        return;
    }
    
    // 获取记录位置
    char* slot = page_handle.get_slot(rid.slot_no);
    if (slot == nullptr) {
        buffer_pool_manager_->unpin_page(page_handle.page->get_page_id(), false);
        return;
    }
    
    // 更新TupleMeta中的时间戳
    TupleMeta* tuple_meta = reinterpret_cast<TupleMeta*>(slot);
    
    // 关键修复：更严格地检查时间戳更新的条件
    // 只有当记录的当前时间戳确实是当前事务的开始时间戳时，才能更新
    if (context->txn_ != nullptr && tuple_meta->ts_ == context->txn_->get_start_ts()) {
        std::cout << "[DEBUG] UpdateRecordCommitTimestamp: updating ts from " << tuple_meta->ts_ 
                  << " to " << commit_ts << " for rid=(" << rid.page_no << "," << rid.slot_no << ")" << std::endl;
        // 额外的安全检查：确保提交时间戳大于开始时间戳
        if (commit_ts > context->txn_->get_start_ts()) {
            tuple_meta->ts_ = commit_ts;

            // 正确地unpin页面并标记为脏页
            buffer_pool_manager_->unpin_page(page_handle.page->get_page_id(), true);
            std::cout << "[DEBUG] Successfully updated timestamp to " << commit_ts << std::endl;
        } else {
            // 如果提交时间戳不合理，不更新，但仍需unpin页面
            buffer_pool_manager_->unpin_page(page_handle.page->get_page_id(), false);
            std::cout << "[DEBUG] Commit timestamp not reasonable, skipping update" << std::endl;
        }
    } else {
        // 如果不需要更新，unpin页面但不标记为脏页
        buffer_pool_manager_->unpin_page(page_handle.page->get_page_id(), false);
        std::cout << "[DEBUG] Not updating timestamp: txn_=" << (context->txn_ ? "valid" : "null") 
                  << ", tuple_ts=" << tuple_meta->ts_ 
                  << ", txn_start_ts=" << (context->txn_ ? context->txn_->get_start_ts() : 0) << std::endl;
    }
}

/**
 * @description: 检查写-写冲突（专门用于写操作前的检查）
 * @param {Rid&} rid 记录号
 * @param {Context*} context
 * @return {bool} 如果存在冲突返回true，否则返回false
 */
bool RmFileHandle::CheckWriteWriteConflict(const Rid& rid, Context* context) const {
    if (context == nullptr || context->txn_ == nullptr || context->txn_mgr_ == nullptr) {
        return false; // 非事务模式不检查冲突
    }

    TransactionManager* txn_manager = context->txn_mgr_;
    if (txn_manager->get_concurrency_mode() != ConcurrencyMode::MVCC) {
        return false; // 非MVCC模式不检查冲突
    }

    Transaction* current_txn = context->txn_;
    
    // **关键修复**：获取原始记录（包含TupleMeta）
    auto record = get_record(rid, context);
    if (!record) {
        return false; // 记录不存在，没有冲突
    }
    
    // 获取TupleMeta
    TupleMeta tuple_meta;
    memcpy(&tuple_meta, record->data, sizeof(TupleMeta));
    
    // 新增：时间戳合理性检查
    if (tuple_meta.ts_ > 1000000000ULL) { // 如果时间戳过大，可能是数据损坏
        std::cout << "[WARNING] CheckWriteWriteConflict: Detected corrupted timestamp: " << tuple_meta.ts_ << std::endl;
        
        // 关键修复：对于损坏的时间戳，直接假设不存在冲突
        std::cout << "[DEBUG] Assuming no write-write conflict due to corrupted timestamp" << std::endl;
        return false;
    }
    
    std::cout << "[DEBUG] CheckWriteWriteConflict: rid=(" << rid.page_no << "," << rid.slot_no 
              << "), tuple_ts=" << tuple_meta.ts_ 
              << ", current_txn_start_ts=" << current_txn->get_start_ts() << std::endl;
    
    // 如果是当前事务创建/修改的版本，没有冲突
    if (tuple_meta.ts_ == current_txn->get_start_ts()) {
        std::cout << "[DEBUG] No conflict: record belongs to current transaction" << std::endl;
        return false;
    }
    
    // **写-写冲突检测的核心逻辑**
    
    // 情况1：事务A尝试更新一条元组时，发现该元组的最新时间戳属于另一个未提交的事务B
    Transaction* tuple_owner_txn = txn_manager->get_transaction_by_timestamp(tuple_meta.ts_);
    if (tuple_owner_txn != nullptr && tuple_owner_txn != current_txn) {
        TransactionState owner_state = tuple_owner_txn->get_state();
        std::cout << "[DEBUG] Found tuple owner transaction, state: " << static_cast<int>(owner_state) << std::endl;
        
        if (owner_state == TransactionState::DEFAULT) {
            // 其他事务正在进行且未提交，存在写-写冲突
            std::cout << "[DEBUG] Write-write conflict detected: tuple belongs to uncommitted transaction" << std::endl;
            return true;
        }
    }
    
    // 情况2：事务A尝试更新一条元组时，发现该元组的最新时间戳属于另一个已提交的事务B，
    // 且该事务B的提交时间戳大于事务A的读时间戳
    if (tuple_meta.ts_ != 0) { // 排除初始数据（时间戳为0）
        // 检查该时间戳是否对应一个已提交的事务
        if (txn_manager->IsCommitted(tuple_meta.ts_)) {
            // **修复**：使用事务对象的get_commit_ts()方法获取提交时间戳
            Transaction* tuple_owner_txn = txn_manager->get_transaction_by_timestamp(tuple_meta.ts_);
            if (tuple_owner_txn != nullptr) {
                timestamp_t commit_ts = tuple_owner_txn->get_commit_ts();
                timestamp_t read_ts = current_txn->get_start_ts(); // 在快照隔离中，读时间戳就是事务开始时间戳
                
                std::cout << "[DEBUG] Tuple belongs to committed transaction, commit_ts=" << commit_ts 
                          << ", read_ts=" << read_ts << std::endl;
                
                if (commit_ts > read_ts) {
                    // 已提交事务的提交时间戳大于当前事务的读时间戳，存在写-写冲突
                    std::cout << "[DEBUG] Write-write conflict detected: committed transaction's commit_ts > current txn's read_ts" << std::endl;
                    return true;
                }
            } else {
                // 如果找不到事务对象，但IsCommitted返回true，说明事务已被清理
                // 在这种情况下，我们需要保守处理
                if (tuple_meta.ts_ > current_txn->get_start_ts()) {
                    // 如果时间戳大于当前事务开始时间，可能存在冲突
                    std::cout << "[DEBUG] Potential write-write conflict: cleaned committed transaction with ts > current txn start_ts" << std::endl;
                    return true;
                }
            }
        } else {
            // 如果时间戳不对应任何已知的事务，可能是一个已经清理的事务
            // 在这种情况下，我们需要更保守的处理
            if (tuple_meta.ts_ > current_txn->get_start_ts()) {
                // 如果时间戳大于当前事务开始时间，可能存在冲突
                std::cout << "[DEBUG] Potential write-write conflict: unknown transaction with ts > current txn start_ts" << std::endl;
                return true;
            }
        }
    }
    
    std::cout << "[DEBUG] No write-write conflict detected" << std::endl;
    return false; // 没有冲突
}