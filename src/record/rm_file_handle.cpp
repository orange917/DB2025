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
#include <cstddef>
#include <memory>
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
        auto record = std::make_unique<RmRecord>(file_hdr_.record_size);
        record->size = file_hdr_.record_size;
        record->SetData(page_handle.get_slot(rid.slot_no));
        buffer_pool_manager_->unpin_page(page_handle.page->get_page_id(), false);
        return record;
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