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

#include "ix_defs.h"
#include "transaction/transaction.h"
#include "system/sm_meta.h"
#include "errors.h"

enum class Operation { FIND = 0, INSERT, DELETE };  // 三种操作：查找、插入、删除

static const bool binary_search = false;


class IxScan;

inline int ix_compare(const char *a, const char *b, ColType type, int col_len) {
    switch (type) {
        case TYPE_INT: {
            int ia = *(int *)a;
            int ib = *(int *)b;
            return (ia < ib) ? -1 : ((ia > ib) ? 1 : 0);
        }
        case TYPE_FLOAT: {
            float fa = *(float *)a;
            float fb = *(float *)b;
            return (fa < fb) ? -1 : ((fa > fb) ? 1 : 0);
        }
        case TYPE_STRING:
            return strncmp(a, b, col_len);
        default:
            throw InternalError("Unexpected data type");
    }
}

inline int ix_compare(const char* a, const char* b, const std::vector<ColType>& col_types, const std::vector<int>& col_lens) {
    int offset = 0;
    for(size_t i = 0; i < col_types.size(); ++i) {
        int res = ix_compare(a + offset, b + offset, col_types[i], col_lens[i]);
        if(res != 0) return res;
        offset += col_lens[i];
    }
    return 0;
}

/* 管理B+树中的每个节点 */
class IxNodeHandle {
    friend class IxIndexHandle;
    friend class IxScan;

   private:
    const IxFileHdr *file_hdr;      // 节点所在文件的头部信息
    Page *page;                     // 存储节点的页面
    IxPageHdr *page_hdr;            // page->data的第一部分，指针指向首地址，长度为sizeof(IxPageHdr)
    char *keys;                     // page->data的第二部分，指针指向首地址，长度为file_hdr->keys_size，每个key的长度为file_hdr->col_len
    Rid *rids;                      // page->data的第三部分，指针指向首地址

   public:
    IxNodeHandle() = default;

    IxNodeHandle(const IxFileHdr *file_hdr_, Page *page_) : file_hdr(file_hdr_), page(page_) {
        page_hdr = reinterpret_cast<IxPageHdr *>(page->get_data());
        keys = page->get_data() + sizeof(IxPageHdr);
        rids = reinterpret_cast<Rid *>(keys + file_hdr->keys_size_);
        // if (page_hdr->num_key > 0) { // 只有在节点不为空时才打印
        //     std::cout << "[DEBUG] page=" << page
        //               << ", keys_size_=" << file_hdr->keys_size_
        //               << ", col_tot_len_=" << file_hdr->col_tot_len_
        //               << ", max_key_num=" << file_hdr->btree_order_ + 1;
        //     // 根据第一列的类型来决定如何打印第一个 key
        //     if (file_hdr->col_types_[0] == TYPE_INT) {
        //         std::cout << ", first_key(int)=" << *(int*)(keys);
        //     } else if (file_hdr->col_types_[0] == TYPE_FLOAT) {
        //         std::cout << ", first_key(float)=" << *(float*)(keys);
        //     } else {
        //         std::cout << ", first_key(str)=...";
        //     }
        //     std::cout << std::endl;
        // }
    }

    int get_size() const { return page_hdr->num_key; }

    void set_size(int size) { page_hdr->num_key = size; }

    int get_max_size() const { return file_hdr->btree_order_ + 1; }

    int get_min_size() const { return get_max_size() / 2; }

    int key_at(int i) const { return *(int *)get_key(i); }

    /* 得到第i个孩子结点的page_no */
    page_id_t value_at(int i) const { return get_rid(i)->page_no; }

    page_id_t get_page_no() const { return page->get_page_id().page_no; }

    PageId get_page_id() const { return page->get_page_id(); }

    page_id_t get_next_leaf() const { return page_hdr->next_leaf; }

    page_id_t get_prev_leaf() const { return page_hdr->prev_leaf; }

    page_id_t get_parent_page_no() const { return page_hdr->parent; }

    bool is_leaf_page() const { return page_hdr->is_leaf; }

    bool is_root_page() const { return get_parent_page_no() == INVALID_PAGE_ID; }

    void set_next_leaf(page_id_t page_no) { page_hdr->next_leaf = page_no; }

    void set_prev_leaf(page_id_t page_no) { page_hdr->prev_leaf = page_no; }

    void set_parent_page_no(page_id_t parent) { page_hdr->parent = parent; }

    char *get_key(int key_idx) const { assert(key_idx >= 0 && key_idx < get_max_size()); return keys + key_idx * file_hdr->col_tot_len_; }

    Rid *get_rid(int rid_idx) const { assert(rid_idx >= 0 && rid_idx < get_max_size()); return &rids[rid_idx]; }

    void set_key(int key_idx, const char *key) { memcpy(keys + key_idx * file_hdr->col_tot_len_, key, file_hdr->col_tot_len_); }

    void set_rid(int rid_idx, const Rid &rid) { rids[rid_idx] = rid; }

    int lower_bound(const char *target) const;

    int upper_bound(const char *target) const;

    void insert_pairs(int pos, const char *key, const Rid *rid, int n);

    page_id_t internal_lookup(const char *key);

    bool leaf_lookup(const char *key, Rid **value);

    int insert(const char *key, const Rid &value);

    void insert_pair(int key_idx, const char* key, int child_page_no);
    // 用于在结点中的指定位置插入单个键值对
    void insert_pair(int pos, const char *key, const Rid &rid) { insert_pairs(pos, key, &rid, 1); }

    void erase_pair(int pos);

    int remove(const char *key);

    /**
     * @brief used in internal node to remove the last key in root node, and return the last child
     *
     * @return the last child
     */
    page_id_t remove_and_return_only_child() {
        assert(get_size() == 1);
        page_id_t child_page_no = value_at(0);
        erase_pair(0);
        assert(get_size() == 0);
        return child_page_no;
    }

    /**
     * @brief 由parent调用，寻找child，返回child在parent中的rid_idx∈[0,page_hdr->num_key)
     * @param child
     * @return int
     */
     int find_child(IxNodeHandle *child) {
        // 修复：一个内部节点有 N 个键和 N+1 个子节点。
        // 循环必须检查所有 N+1 个子节点，所以循环条件是 rid_idx <= page_hdr->num_key。
        for (int rid_idx = 0; rid_idx <= page_hdr->num_key; rid_idx++) {
            if (get_rid(rid_idx)->page_no == child->get_page_no()) {
                return rid_idx;
            }
        }
        throw std::runtime_error("FATAL: Child page not found in parent node. Index is likely corrupt.");
    }

    const IxFileHdr* get_file_hdr() const { return file_hdr; }
};

/* B+树 */
class IxIndexHandle {
    friend class IxScan;
    friend class IxManager;

   private:
    DiskManager *disk_manager_;
    BufferPoolManager *buffer_pool_manager_;
    int fd_;                                    // 存储B+树的文件
    IxFileHdr* file_hdr_;                       // 存了root_page，但其初始化为2（第0页存FILE_HDR_PAGE，第1页存LEAF_HEADER_PAGE）
    std::mutex root_latch_;
    IndexMeta index_meta_;                      // 索引元数据，包含唯一性等信息

   public:
    IxIndexHandle(DiskManager *disk_manager, BufferPoolManager *buffer_pool_manager, int fd);
    IxIndexHandle(DiskManager *disk_manager, BufferPoolManager *buffer_pool_manager, int fd, const IndexMeta& index_meta);

    int get_fd() const { return fd_; }

    // 用于获取节点
    IxNodeHandle *get_node(int page_no) const {
        return fetch_node(page_no);
    }



    // for search
    bool get_value(const char *key, std::vector<Rid> *result, Transaction *transaction);

    std::pair<IxNodeHandle *, bool> find_leaf_page(const char *key, Operation operation, Transaction *transaction,
                                                 bool find_first = false);

    // for insert
    page_id_t insert_entry(const char *key, const Rid &value, Transaction *transaction);

    IxNodeHandle *split(IxNodeHandle *node);

    void insert_into_parent(IxNodeHandle *old_node, const char *key, IxNodeHandle *new_node, Transaction *transaction);

    // for delete
    bool delete_entry(const char *key, Transaction *transaction);

    bool coalesce_or_redistribute(IxNodeHandle *node, Transaction *transaction = nullptr,
                                bool *root_is_latched = nullptr);
    bool adjust_root(IxNodeHandle *old_root_node);

    void redistribute(IxNodeHandle *neighbor_node, IxNodeHandle *node, IxNodeHandle *parent, int index);

    bool coalesce(IxNodeHandle **neighbor_node, IxNodeHandle **node, IxNodeHandle **parent, int index,
                  Transaction *transaction, bool *root_is_latched);

    std::unique_ptr<IxScan> create_scan(const char* lower_key, const char* upper_key, 
        bool lower_inclusive, bool upper_inclusive);

    Iid lower_bound(const char *key);

    Iid upper_bound(const char *key);

    Iid leaf_end() const;

    Iid leaf_begin() const;
    
    void print_tree(const std::string& outfile_path);

   private:
    // 辅助函数
    void update_root_page_no(page_id_t root) { file_hdr_->root_page_ = root; }

    bool is_empty() const { return file_hdr_->root_page_ == IX_NO_PAGE; }

    // for get/create node
    IxNodeHandle *fetch_node(int page_no) const;

    IxNodeHandle *create_node();

    // for maintain data structure
    void maintain_parent(IxNodeHandle *node);

    void erase_leaf(IxNodeHandle *leaf);

    void release_node_handle(IxNodeHandle &node);

    void maintain_child(IxNodeHandle *node, int child_idx);

    // for index test
    Rid get_rid(const Iid &iid) const;
};