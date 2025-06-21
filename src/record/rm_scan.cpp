/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "rm_scan.h"
#include "rm_defs.h"
#include "rm_file_handle.h"
#include <iostream>

/**
 * @brief 初始化file_handle和rid
 * @param file_handle
 */
RmScan::RmScan(const RmFileHandle *file_handle) : file_handle_(file_handle) {
    // Todo:
    rid_.page_no = RM_FIRST_RECORD_PAGE;
    rid_.slot_no = -1;
    next();
}

/**
 * @brief 找到文件中下一个存放了记录的位置
 */
void RmScan::next() {
    // Todo:
    // 找到文件中下一个存放了记录的非空闲位置，用rid_来指向这个位置
    while (rid_.page_no != RM_NO_PAGE) {
        // 检查是否超出文件范围
        if (rid_.page_no >= file_handle_->file_hdr_.num_pages) {
            rid_.page_no = RM_NO_PAGE;
            rid_.slot_no = -1;
            return;
        }

        RmPageHandle page_handle = file_handle_->fetch_page_handle(rid_.page_no);

        if (page_handle.page == nullptr) {
            rid_.page_no++;
            rid_.slot_no = -1;
            continue;
        }

        // 从当前slot_no之后找到下一个已占用的位置
        int next_slot = Bitmap::next_bit(true, page_handle.bitmap, file_handle_->file_hdr_.num_records_per_page, rid_.slot_no);

        // 如果在当前页中找到了有效记录
        if (next_slot < file_handle_->file_hdr_.num_records_per_page) {
            rid_.slot_no = next_slot;
            file_handle_->buffer_pool_manager_->unpin_page(page_handle.page->get_page_id(), false);
            return;
        }

        // 如果当前页中没有找到记录，移动到下一页
        file_handle_->buffer_pool_manager_->unpin_page(page_handle.page->get_page_id(), false);
        rid_.page_no++;
        rid_.slot_no = -1;

        // 检查是否已经到达文件末尾
        if (rid_.page_no >= file_handle_->file_hdr_.num_pages) {
            rid_.page_no = RM_NO_PAGE;  // 使用RM_NO_PAGE常量
            rid_.slot_no = -1;
            return;
        }
    }
}

/**
 * @brief ​ 判断是否到达文件末尾
 */
 bool RmScan::is_end() const {
    // Todo: 修改返回值
    return rid_.page_no == RM_NO_PAGE || rid_.page_no >= file_handle_->file_hdr_.num_pages || rid_.slot_no < 0;
}

/**
 * @brief RmScan内部存放的rid
 */
Rid RmScan::rid() const {
    return rid_;
}