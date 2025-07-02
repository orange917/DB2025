/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "errors.h"
#include "ix_index_handle.h"
#include <queue>
#include <fstream>
#include <iomanip>
#include <sstream>

#include "ix_scan.h"

// 全局调试输出流
// std::ofstream ix_debug_log("ix_debug.log", std::ios::app);
// std::ofstream ix_debug_detail_log("ix_debug_detail.log", std::ios::app);

// void debug_print_key(const char* key, const IxFileHdr* file_hdr) {
//     for (int j = 0; j < file_hdr->col_num_; j++) {
//         ColType type = file_hdr->col_types_[j];
//         int len = file_hdr->col_lens_[j];
//         if (j > 0) ix_debug_detail_log << ",";
//         int offset = 0;
//         for (int k = 0; k < j; k++) offset += file_hdr->col_lens_[k];
//         switch (type) {
//             case TYPE_INT: ix_debug_detail_log << *(int*)(key + offset); break;
//             case TYPE_FLOAT: ix_debug_detail_log << *(float*)(key + offset); break;
//             case TYPE_STRING: ix_debug_detail_log << "'" << std::string(key + offset, strnlen(key + offset, len)) << "'"; break;
//             default: ix_debug_detail_log << "?";
//         }
//     }
// }

// void debug_print_rid(const Rid* rid) {
//     ix_debug_detail_log << "(page_no=" << rid->page_no << ",slot_no=" << rid->slot_no << ")";
// }

// void debug_print_key_rid_array(const IxNodeHandle* node, int max_print = 20) {
//     int sz = node->get_size();
//     int print_head = std::min(sz, max_print);
//     int print_tail = std::min(sz, max_print);
//     ix_debug_detail_log << "[key_rid_array] page_no=" << node->get_page_no() << ", size=" << sz << std::endl;
//     for (int i = 0; i < print_head; i++) {
//         ix_debug_detail_log << "  idx=" << i << ", key=";
//         debug_print_key(node->get_key(i), node->get_file_hdr());
//         ix_debug_detail_log << ", rid=";
//         debug_print_rid(node->get_rid(i));
//         ix_debug_detail_log << ", key_ptr=" << static_cast<const void*>(node->get_key(i));
//         ix_debug_detail_log << ", rid_ptr=" << static_cast<const void*>(node->get_rid(i));
//         if (node->get_file_hdr()->col_num_ > 0 && node->get_file_hdr()->col_types_[0] == TYPE_INT && *(int*)node->get_key(i) == 9) {
//             ix_debug_detail_log << "  <== key=9";
//         }
//         ix_debug_detail_log << std::endl;
//     }
//     if (sz > 2 * max_print) {
//         ix_debug_detail_log << "  ... ..." << std::endl;
//         for (int i = sz - print_tail; i < sz; i++) {
//             ix_debug_detail_log << "  idx=" << i << ", key=";
//             debug_print_key(node->get_key(i), node->get_file_hdr());
//             ix_debug_detail_log << ", rid=";
//             debug_print_rid(node->get_rid(i));
//             ix_debug_detail_log << ", key_ptr=" << static_cast<const void*>(node->get_key(i));
//             ix_debug_detail_log << ", rid_ptr=" << static_cast<const void*>(node->get_rid(i));
//             if (node->get_file_hdr()->col_num_ > 0 && node->get_file_hdr()->col_types_[0] == TYPE_INT && *(int*)node->get_key(i) == 9) {
//                 ix_debug_detail_log << "  <== key=9";
//             }
//             ix_debug_detail_log << std::endl;
//         }
//     }
// }

/**
 * @brief 在当前node中查找第一个>=target的key_idx
 *
 * @return key_idx，范围为[0,num_key)，如果返回的key_idx=num_key，则表示target大于最后一个key
 * @note 返回key index（同时也是rid index），作为slot no
 */
int IxNodeHandle::lower_bound(const char *target) const {
    // Todo:
    // 查找当前节点中第一个大于等于target的key，并返回key的位置给上层
    // 提示: 可以采用多种查找方式，如顺序遍历、二分查找等；使用ix_compare()函数进行比较

    // 采用二分查找方法查找第一个大于等于target的key
    int left = 0;
    int right = page_hdr->num_key;
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (ix_compare(get_key(mid), target, file_hdr->col_types_, file_hdr->col_lens_) < 0) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return left;
}

/**
 * @brief 在当前node中查找第一个>target的key_idx
 *
 * @return key_idx，范围为[1,num_key)，如果返回的key_idx=num_key，则表示target大于等于最后一个key
 * @note 注意此处的范围从1开始
 */
int IxNodeHandle::upper_bound(const char *target) const {
    // Todo:
    // 查找当前节点中第一个大于target的key，并返回key的位置给上层
    // 提示: 可以采用多种查找方式：顺序遍历、二分查找等；使用ix_compare()函数进行比较
    // 注意：此处的范围从1开始
    
    // 采用二分查找方法查找第一个大于target的key
    int left = 1;  // 从1开始，符合B+树内部节点的要求
    int right = page_hdr->num_key;
    while (left < right) {
        int mid = left + (right - left) / 2;
        if (ix_compare(get_key(mid), target, file_hdr->col_types_, file_hdr->col_lens_) <= 0) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return left;
}


/**
 * @brief 用于叶子结点根据key来查找该结点中的键值对
 * 值value作为传出参数，函数返回是否查找成功
 *
 * @param key 目标key
 * @param[out] value 传出参数，目标key对应的Rid
 * @return 目标key是否存在
 */
bool IxNodeHandle::leaf_lookup(const char *key, Rid **value) {
    // Todo:
    // 1. 在叶子节点中获取目标key所在位置
    // 2. 判断目标key是否存在
    // 3. 如果存在，获取key对应的Rid，并赋值给传出参数value
    // 提示：可以调用lower_bound()和get_rid()函数。

    // 确保是叶子节点
    assert(is_leaf_page());

    // 在叶子节点中查找目标key所在位置
    int pos = lower_bound(key);
    
    // 如果pos等于num_key，说明没有找到大于等于key的位置
    if (pos == page_hdr->num_key) {
        return false;
    }
    
    // 检查找到的位置是否真的等于key
    if (ix_compare(key, get_key(pos), file_hdr->col_types_, file_hdr->col_lens_) != 0) {
        return false;
    }
    
    // 找到了匹配的key，获取对应的Rid
    *value = get_rid(pos);
    return true;
}

/**
 * 用于内部结点（非叶子节点）查找目标key所在的孩子结点（子树）
 * @param key 目标key
 * @return page_id_t 目标key所在的孩子节点（子树）的存储页面编号
 */
page_id_t IxNodeHandle::internal_lookup(const char *key) {
    // Todo:
    // 1. 查找当前非叶子节点中目标key所在孩子节点（子树）的位置
    // 2. 获取该孩子节点（子树）所在页面的编号
    // 3. 返回页面编号

    // 确保当前节点是内部节点
    assert(!is_leaf_page());

     // 使用lower_bound找到第一个大于或等于key的键的索引
     int child_idx = lower_bound(key);
    
     // lower_bound返回的索引 i 对应于我们应该跟随的子指针
     // keys: [k0, k1, k2, ...]
     // ptrs: [p0, p1, p2, p3, ...]
     // if key < k0, lower_bound returns 0, we take p0.
     // if k0 <= key < k1, lower_bound returns 1, we take p1.
     return value_at(child_idx);
}

/**
 * @brief 在指定位置插入n个连续的键值对
 * 将key的前n位插入到原来keys中的pos位置；将rid的前n位插入到原来rids中的pos位置
 *
 * @param pos 要插入键值对的位置
 * @param (key, rid) 连续键值对的起始地址，也就是第一个键值对，可以通过(key, rid)来获取n个键值对
 * @param n 键值对数量
 * @note [0,pos)           [pos,num_key)
 *                            key_slot
 *                            /      \
 *                           /        \
 *       [0,pos)     [pos,pos+n)   [pos+n,num_key+n)
 *                      key           key_slot
 */
void IxNodeHandle::insert_pairs(int pos, const char *key, const Rid *rid, int n) {
    // ix_debug_detail_log << "[insert_pairs] call, page_no=" << get_page_no()
    //                    << ", pos=" << pos << ", n=" << n
    //                    << ", num_key=" << page_hdr->num_key << std::endl;
    assert(pos >= 0 && pos <= page_hdr->num_key);
    int num_key = page_hdr->num_key;

    // 如果pos不是在尾部，需要将pos及之后的数据向后移动n个位置
    if (pos < num_key) {
        // ix_debug_detail_log << "[insert_pairs] BEFORE move, page_no=" << get_page_no() << std::endl;
        // debug_print_key_rid_array(this);
        // 用for循环手动搬运，避免memmove重叠覆盖
        for (int i = num_key - 1; i >= pos; --i) {
            assert(i + n < get_max_size());
            assert(i < get_max_size());
            memcpy(get_key(i + n), get_key(i), file_hdr->col_tot_len_);
            memcpy(get_rid(i + n), get_rid(i), sizeof(Rid));
        }
        // ix_debug_detail_log << "[insert_pairs] AFTER move, page_no=" << get_page_no() << std::endl;
        // debug_print_key_rid_array(this);
    }

    // 在pos位置插入n个key和rid
    for (int i = 0; i < n; i++) {
        // ix_debug_detail_log << "[insert_pairs] memcpy at pos=" << (pos + i) << ", key_ptr=" << static_cast<const void*>(key + i * file_hdr->col_tot_len_) << ", rid_ptr=" << static_cast<const void*>(rid + i) << std::endl;
        // ix_debug_detail_log << "  key=";
        // debug_print_key(key + i * file_hdr->col_tot_len_, file_hdr);
        // ix_debug_detail_log << ", rid=";
        // debug_print_rid(rid + i);
        // ix_debug_detail_log << std::endl;
        memcpy(get_key(pos + i), key + i * file_hdr->col_tot_len_, file_hdr->col_tot_len_);
        memcpy(get_rid(pos + i), rid + i, sizeof(Rid));
    }

    // 更新节点的键值对数量
    page_hdr->num_key += n;
    // ix_debug_detail_log << "[insert_pairs] AFTER memcpy, page_no=" << get_page_no() << std::endl;
    // debug_print_key_rid_array(this);
}

/**
 * @brief 用于在结点中插入单个键值对。
 * 函数返回插入后的键值对数量
 *
 * @param (key, value) 要插入的键值对
 * @return int 键值对数量
 */
int IxNodeHandle::insert(const char *key, const Rid &value) {
    // Todo:
    // 1. 查找要插入的键值对应该插入到当前节点的哪个位置
    // 2. 如果key重复则不插入
    // 3. 如果key不重复则插入键值对
    // 4. 返回完成插入操作之后的键值对数量

    // 查找key应该插入的位置
    int pos = lower_bound(key);

    // 检查是否为重复键
    if (pos < page_hdr->num_key) {
        char *exist_key = get_key(pos);
        // 如果找到了相同的key，则不插入
        if (ix_compare(exist_key, key, file_hdr->col_types_, file_hdr->col_lens_) == 0) {
            return page_hdr->num_key;  // 返回当前键值对数量，不变
        }
    }

    // key不重复，插入键值对
    insert_pairs(pos, key, &value, 1);

    // 返回插入后的键值对数量
    return page_hdr->num_key;
}

/**
 * @brief 用于在结点中的指定位置删除单个键值对
 *
 * @param pos 要删除键值对的位置
 */
void IxNodeHandle::erase_pair(int pos) {
    // Todo:
    // 1. 删除该位置的key
    // 2. 删除该位置的rid
    // 3. 更新结点的键值对数量

    // 检查pos的合法性
    assert(pos >= 0 && pos < page_hdr->num_key);

    int num_key = page_hdr->num_key;
    int max_size = get_max_size();
    if (pos < num_key - 1) {
        assert(pos + 1 < max_size);
        assert(num_key - pos - 1 >= 0 && num_key - pos - 1 <= max_size);
        memmove(get_key(pos), get_key(pos + 1), (num_key - pos - 1) * file_hdr->col_tot_len_);
        memmove(get_rid(pos), get_rid(pos + 1), (num_key - pos - 1) * sizeof(Rid));
    }
    page_hdr->num_key--;
}

/**
 * @brief 用于在结点中删除指定key的键值对。函数返回删除后的键值对数量
 *
 * @param key 要删除的键值对key值
 * @return 完成删除操作后的键值对数量
 */
int IxNodeHandle::remove(const char *key) {
    // Todo:
    // 1. 查找要删除键值对的位置
    // 2. 如果要删除的键值对存在，删除键值对
    // 3. 返回完成删除操作后的键值对数量

    // 查找要删除的键值对位置
    int pos = lower_bound(key);

    // 检查键值对是否存在
    if (pos < page_hdr->num_key) {
        char *exist_key = get_key(pos);
        // 如果找到了相同的key，则删除
        if (ix_compare(exist_key, key, file_hdr->col_types_, file_hdr->col_lens_) == 0) {
            erase_pair(pos);
        }
    }

    // 返回删除后的键值对数量
    return page_hdr->num_key;
}

IxIndexHandle::IxIndexHandle(DiskManager *disk_manager, BufferPoolManager *buffer_pool_manager, int fd)
    : disk_manager_(disk_manager), buffer_pool_manager_(buffer_pool_manager), fd_(fd) {
    // index_meta_ 可设为默认值
    file_hdr_ = new IxFileHdr();
    char* buf = new char[PAGE_SIZE];
    memset(buf, 0, PAGE_SIZE);
    disk_manager_->read_page(fd, IX_FILE_HDR_PAGE, buf, PAGE_SIZE);
    file_hdr_->deserialize(buf);
    delete[] buf;
}

IxIndexHandle::IxIndexHandle(DiskManager *disk_manager, BufferPoolManager *buffer_pool_manager, int fd, const IndexMeta& index_meta)
    : disk_manager_(disk_manager), buffer_pool_manager_(buffer_pool_manager), fd_(fd), index_meta_(index_meta) {
    file_hdr_ = new IxFileHdr();
    char* buf = new char[PAGE_SIZE];
    memset(buf, 0, PAGE_SIZE);
    disk_manager_->read_page(fd, IX_FILE_HDR_PAGE, buf, PAGE_SIZE);
    file_hdr_->deserialize(buf);
    delete[] buf;

    // disk_manager管理的fd对应的文件中，设置从file_hdr_->num_pages开始分配page_no
    int now_page_no = disk_manager_->get_fd2pageno(fd);
    disk_manager_->set_fd2pageno(fd, now_page_no + 1);
}

/**
 * @brief 用于查找指定键所在的叶子结点
 * @param key 要查找的目标key值
 * @param operation 查找到目标键值对后要进行的操作类型
 * @param transaction 事务参数，如果不需要则默认传入nullptr
 * @return [leaf node] and [root_is_latched] 返回目标叶子结点以及根结点是否加锁
 * @note need to Unlatch and unpin the leaf node outside!
 * 注意：用了FindLeafPage之后一定要unlatch叶结点，否则下次latch该结点会堵塞！
 */
std::pair<IxNodeHandle *, bool> IxIndexHandle::find_leaf_page(const char *key, Operation operation,
                            Transaction *transaction, bool find_first) {
    // Todo:
    // 1. 获取根节点
    // 2. 从根节点开始不断向下查找目标key
    // 3. 找到包含该key值的叶子结点停止查找，并返回叶子节点

    // 获取根节点
    int root_page_no = file_hdr_->root_page_;
    IxNodeHandle *node = fetch_node(root_page_no);
    bool root_is_latched = false;  // 标记根节点是否被加锁

    // 从根节点开始，一直向下直到找到叶子节点
    while (!node->is_leaf_page()) {
        int child_page_no = node->internal_lookup(key);  // 查找子节点

        // 获取子节点
        IxNodeHandle *child_node = fetch_node(child_page_no);

        // 释放当前节点，因为我们已经找到了下一层的节点
        buffer_pool_manager_->unpin_page(node->get_page_id(), false);
        delete node;

        // 更新node为子节点，继续向下查找
        node = child_node;
    }

    // 此时node已经是叶子节点，返回node和root_is_latched
    return std::make_pair(node, root_is_latched);
}

/**
 * @brief 用于查找指定键在叶子结点中的对应的值result
 *
 * @param key 查找的目标key值
 * @param result 用于存放结果的容器
 * @param transaction 事务指针
 * @return bool 返回目标键值对是否存在
 */
bool IxIndexHandle::get_value(const char *key, std::vector<Rid> *result, Transaction *transaction) {
    // Todo:
    // 1. 获取目标key值所在的叶子结点
    // 2. 在叶子节点中查找目标key值的位置，并读取key对应的rid
    // 3. 把rid存入result参数中
    // 提示：使用完buffer_pool提供的page之后，记得unpin page；记得处理并发的上锁

    // 获取叶子节点
    std::pair<IxNodeHandle *, bool> leaf_pair = find_leaf_page(key, Operation::FIND, transaction);
    IxNodeHandle *leaf = leaf_pair.first;
    // bool root_is_latched = leaf_pair.second;

    // 在叶子节点中查找key
    Rid *rid = nullptr;
    bool found = leaf->leaf_lookup(key, &rid);

    // 如果找到了，将结果添加到result中
    if (found && rid != nullptr) {
        result->push_back(*rid);
    }

    // 使用完叶子节点后，记得unpin
    buffer_pool_manager_->unpin_page(leaf->get_page_id(), false);

    // 释放资源
    delete leaf;

    return found;
}

/**
 * @brief  将传入的一个node拆分(Split)成两个结点，在node的右边生成一个新结点new node
 * @param node 需要拆分的结点
 * @return 拆分得到的new_node
 * @note need to unpin the new node outside
 * 注意：本函数执行完毕后，原node和new node都需要在函数外面进行unpin
 */
IxNodeHandle *IxIndexHandle::split(IxNodeHandle *node) {
    // Todo:
    // 1. 将原结点的键值对平均分配，右半部分分裂为新的右兄弟结点
    //    需要初始化新节点的page_hdr内容
    // 2. 如果新的右兄弟结点是叶子结点，更新新旧节点的prev_leaf和next_leaf指针
    //    为新节点分配键值对，更新旧节点的键值对数记录
    // 3. 如果新的右兄弟结点不是叶子结点，更新该结点的所有孩子结点的父节点信息(使用IxIndexHandle::maintain_child())

    // 创建新的右兄弟节点
    IxNodeHandle *new_node = create_node();

    // 初始化新节点
    new_node->page_hdr->is_leaf = node->is_leaf_page();
    new_node->page_hdr->num_key = 0;
    new_node->page_hdr->parent = node->get_parent_page_no();

    // 计算分裂点：将原节点的键值对平均分配
    int num_key = node->get_size();
    
    if (node->is_leaf_page()) {
        int split_point = (num_key + 1) / 2; // 确保左边节点在重分配后有更多或相等的键
        // ix_debug_detail_log << "[split] leaf split, node=" << node->get_page_no()
        //                    << " new_node=" << new_node->get_page_no()
        //                    << ", split_point=" << split_point
        //                    << ", num_key=" << num_key << std::endl;
        // for (int i = split_point; i < num_key; i++) {
        //     ix_debug_log << "  move key=";
        //     debug_print_key(node->get_key(i), file_hdr_);
        //     ix_debug_log << ", rid=";
        //     debug_print_rid(node->get_rid(i));
        //     ix_debug_log << std::endl;
        // }
        new_node->insert_pairs(0, node->get_key(split_point), node->get_rid(split_point), num_key - split_point);
        node->set_size(split_point);

        // 更新叶子节点链表
        new_node->set_prev_leaf(node->get_page_no());
        new_node->set_next_leaf(node->get_next_leaf());

        // 如果node不是最后一个叶子节点，更新node原来的下一个叶子节点的prev指针
        if (node->get_next_leaf() != IX_NO_PAGE) {
            IxNodeHandle *next_leaf = fetch_node(node->get_next_leaf());
            next_leaf->set_prev_leaf(new_node->get_page_no());
            buffer_pool_manager_->unpin_page(next_leaf->get_page_id(), true);
            delete next_leaf;
        } else {
            // 如果node是最后一个叶子节点，更新file_hdr中的last_leaf指针
            file_hdr_->last_leaf_ = new_node->get_page_no();
        }
        // 更新原节点的next指针
        node->set_next_leaf(new_node->get_page_no());
    } else {
        // 内部节点分裂：中间key被上移，不保留在子节点中
        int middle_idx = num_key / 2;
        // 要移动到新节点的键的数量
        int key_move_count = num_key - middle_idx - 1;
        // 要移动到新节点的子节点指针的数量
        int rid_move_count = key_move_count + 1;

        // ix_debug_detail_log << "[split] internal split, node=" << node->get_page_no()
        //                    << " new_node=" << new_node->get_page_no()
        //                    << ", middle_idx=" << middle_idx
        //                    << ", num_key=" << num_key
        //                    << ", key_move_count=" << key_move_count
        //                    << ", rid_move_count=" << rid_move_count << std::endl;
        // for (int i = 0; i < key_move_count; i++) {
        //     ix_debug_log << "  move key=";
        //     debug_print_key(node->get_key(middle_idx + 1 + i), file_hdr_);
        //     ix_debug_log << ", rid=";
        //     debug_print_rid(node->get_rid(middle_idx + 1 + i));
        //     ix_debug_log << std::endl;
        // }
        memcpy(new_node->get_key(0), node->get_key(middle_idx + 1), key_move_count * file_hdr_->col_tot_len_);
        memcpy(new_node->get_rid(0), node->get_rid(middle_idx + 1), rid_move_count * sizeof(Rid));
        new_node->set_size(key_move_count);

        // 更新原节点的大小，middle_idx 处的键将被提升，所以原节点大小为 middle_idx
        node->set_size(middle_idx);

        // 更新被移动到新节点的子节点的父指针
        for (int i = 0; i < rid_move_count; i++) {
            maintain_child(new_node, i);
        }
    }

    return new_node;
}

/**
 * @brief Insert key & value pair into internal page after split
 * 拆分(Split)后，向上找到old_node的父结点
 * 将new_node的第一个key插入到父结点，其位置在 父结点指向old_node的孩子指针 之后
 * 如果插入后>=maxsize，则必须继续拆分父结点，然后在其父结点的父结点再插入，即需要递归
 * 直到找到的old_node为根结点时，结束递归（此时将会新建一个根R，关键字为key，old_node和new_node为其孩子）
 *
 * @param (old_node, new_node) 原结点为old_node，old_node被分裂之后产生了新的右兄弟结点new_node
 * @param key 要插入parent的key
 * @note 一个结点插入了键值对之后需要分裂，分裂后左半部分的键值对保留在原结点，在参数中称为old_node，
 * 右半部分的键值对分裂为新的右兄弟节点，在参数中称为new_node（参考Split函数来理解old_node和new_node）
 * @note 本函数执行完毕后，new node和old node都需要在函数外面进行unpin
 */
void IxIndexHandle::insert_into_parent(IxNodeHandle *old_node, const char *key, IxNodeHandle *new_node,
                                     Transaction *transaction) {
    // Todo:
    // 1. 分裂前的结点（原结点, old_node）是否为根结点，如果为根结点需要分配新的root
    // 2. 获取原结点（old_node）的父亲结点
    // 3. 获取key对应的rid，并将(key, rid)插入到父亲结点
    // 4. 如果父亲结点仍需要继续分裂，则进行递归插入
    // 提示：记得unpin page

    // 1. 检查old_node是否为根节点
    if (old_node->is_root_page()) {
        // 如果是根节点，创建一个新的根页面
        IxNodeHandle *new_root = create_node();
        new_root->page_hdr->is_leaf = false;
        new_root->page_hdr->parent = IX_NO_PAGE;
        
        // 新的根节点有一个key（被提升的key）和两个子节点（old_node和new_node）
        // 设置被提升的key
        memcpy(new_root->get_key(0), key, file_hdr_->col_tot_len_);
        
        // 设置左子节点指针 (指向 old_node)
        new_root->get_rid(0)->page_no = old_node->get_page_no();
        
        // 设置右子节点指针 (指向 new_node)
        new_root->get_rid(1)->page_no = new_node->get_page_no();
        
        // 更新新根节点的key数量
        new_root->page_hdr->num_key = 1;

        // 更新old_node和new_node的父节点为新根节点
        old_node->set_parent_page_no(new_root->get_page_no());
        new_node->set_parent_page_no(new_root->get_page_no());

        // 更新文件头中的根页面
        file_hdr_->root_page_ = new_root->get_page_no();

        // Unpin新根节点的页面
        buffer_pool_manager_->unpin_page(new_root->get_page_id(), true);
        delete new_root;

        return;
    }

    // 2. 如果old_node不是根节点，获取其父节点
    IxNodeHandle *parent = fetch_node(old_node->get_parent_page_no());

    // 3. 在父节点中为新节点找到正确的位置并插入
    int insert_pos = parent->lower_bound(key);
    parent->insert_pair(insert_pos, key, new_node->get_page_no());

    // 4. 检查父节点是否需要分裂
    if (parent->get_size() == parent->get_max_size()) {
        // 内部节点分裂，中间key需要上移
        int middle_idx = parent->get_size() / 2;
        // 使用堆分配内存来存储中间键
        char *middle_key = new char[file_hdr_->col_tot_len_];
        // 在分裂前拷贝需要上移的key
        memcpy(middle_key, parent->get_key(middle_idx), file_hdr_->col_tot_len_);

        IxNodeHandle *new_parent = split(parent);

        // 递归插入到上层
        insert_into_parent(parent, middle_key, new_parent, transaction);

        // 释放为中间键分配的堆内存
        delete[] middle_key;

        buffer_pool_manager_->unpin_page(new_parent->get_page_id(), true);
        delete new_parent;
    }

    buffer_pool_manager_->unpin_page(parent->get_page_id(), true);
    delete parent;
}

/**
 * @brief 在内部节点中插入一个 key 和一个指向新子节点的指针
 */
void IxNodeHandle::insert_pair(int key_idx, const char* key, int child_page_no) {
    assert(!is_leaf_page());
    assert(key_idx >= 0 && key_idx <= page_hdr->num_key);
    int max_size = get_max_size();
    assert(key_idx + 1 < max_size);
    assert(key_idx < max_size);
    memmove(get_key(key_idx + 1), get_key(key_idx), (page_hdr->num_key - key_idx) * file_hdr->col_tot_len_);
    memmove(get_rid(key_idx + 2), get_rid(key_idx + 1), (page_hdr->num_key - key_idx) * sizeof(Rid));
    memcpy(get_key(key_idx), key, file_hdr->col_tot_len_);
    get_rid(key_idx + 1)->page_no = child_page_no;
    page_hdr->num_key++;
}

/**
 * @brief 将指定键值对插入到B+树中
 * @param (key, value) 要插入的键值对
 * @param transaction 事务指针
 * @return page_id_t 插入到的叶结点的page_no
 */
page_id_t IxIndexHandle::insert_entry(const char *key, const Rid &value, Transaction *transaction) {
    // 唯一性检查
    if (index_meta_.unique) {
        std::vector<Rid> result;
        if (get_value(key, &result, transaction) && !result.empty()) {
            std::vector<std::string> col_names;
            for (const auto& col : index_meta_.cols) {
                col_names.push_back(col.name);
            }
            // 确保表名不为空，如果为空则使用默认值
            std::string tab_name = index_meta_.tab_name.empty() ? "unknown_table" : index_meta_.tab_name;
            // 如果列名为空，添加默认列名
            if (col_names.empty()) {
                col_names.push_back("unknown_column");
            }
            throw UniqueIndexViolationError(tab_name, col_names);
        }
    }
    // Todo:
    // 1. 查找key值应该插入到哪个叶子节点
    // 2. 在该叶子节点中插入键值对
    // 3. 如果结点已满，分裂结点，并把新结点的相关信息插入父节点
    // 提示：记得unpin page；若当前叶子节点是最右叶子节点，则需要更新file_hdr_.last_leaf；记得处理并发的上锁

    // 1. 查找key值应该插入到哪个叶子节点
    std::pair<IxNodeHandle *, bool> leaf_pair = find_leaf_page(key, Operation::INSERT, transaction);
    IxNodeHandle *leaf = leaf_pair.first;
    bool root_is_latched = leaf_pair.second;

    // 记录插入到的叶子节点的page_no
    page_id_t leaf_page_no = leaf->get_page_no();

    // 2. 在该叶子节点中插入键值对
    int size_after_insert = leaf->insert(key, value);

    // 3. 如果结点已满，分裂结点，并把新结点的相关信息插入父节点
    if (size_after_insert == leaf->get_max_size()) {
        // 立即分裂
        IxNodeHandle *new_leaf = split(leaf);

        // 获取新叶子节点的第一个key作为上升key
        char *mid_key = new_leaf->get_key(0);

        // 把新结点的相关信息插入父节点
        insert_into_parent(leaf, mid_key, new_leaf, transaction);

        // 如果当前叶子节点是最右叶子节点，需要更新file_hdr_.last_leaf
        if (leaf->get_page_no() == file_hdr_->last_leaf_) {
            file_hdr_->last_leaf_ = new_leaf->get_page_no();
        }

        // unpin新叶子节点
        buffer_pool_manager_->unpin_page(new_leaf->get_page_id(), true);
        delete new_leaf;
    }

    // unpin叶子节点
    buffer_pool_manager_->unpin_page(leaf->get_page_id(), true);
    delete leaf;

    return leaf_page_no;
}

/**
 * @brief 用于删除B+树中含有指定key的键值对
 * @param key 要删除的key值
 * @param transaction 事务指针
 */
bool IxIndexHandle::delete_entry(const char *key, Transaction *transaction) {
    // Todo:
    // 1. 获取该键值对所在的叶子结点
    // 2. 在该叶子结点中删除键值对
    // 3. 如果删除成功需要调用CoalesceOrRedistribute来进行合并或重分配操作，并根据函数返回结果判断是否有结点需要删除
    // 4. 如果需要并发，并且需要删除叶子结点，则需要在事务的delete_page_set中添加删除结点的对应页面；记得处理并发的上锁

    // 1. 获取该键值对所在的叶子结点
    std::pair<IxNodeHandle *, bool> leaf_pair = find_leaf_page(key, Operation::DELETE, transaction);
    IxNodeHandle *leaf = leaf_pair.first;
    bool root_is_latched = leaf_pair.second;

    // 2. 在该叶子结点中删除键值对
    int size_before_delete = leaf->get_size();
    int size_after_delete = leaf->remove(key);

    // 检查是否成功删除了键值对
    bool deleted = (size_after_delete < size_before_delete);

    if (deleted) {
        // 3. 如果删除成功需要调用CoalesceOrRedistribute来进行合并或重分配操作
        bool should_delete = coalesce_or_redistribute(leaf, transaction, &root_is_latched);

        // 4. 根据函数返回结果判断是否有结点需要删除
        if (should_delete) {
            // 如果需要并发，并且需要删除叶子结点，则需要在事务的delete_page_set中添加删除结点的对应页面
            if (transaction != nullptr) {
                transaction->append_index_deleted_page(leaf->page);
            } else {
                // 如果没有事务，直接删除节点
                erase_leaf(leaf);
                buffer_pool_manager_->delete_page(leaf->get_page_id());
                release_node_handle(*leaf);
            }
        }
    }

    // unpin叶子节点
    buffer_pool_manager_->unpin_page(leaf->get_page_id(), deleted); // 如果有改动，则标记为脏页
    delete leaf;

    return deleted;
}

/**
 * @brief 用于处理合并和重分配的逻辑，用于删除键值对后调用
 *
 * @param node 执行完删除操作的结点
 * @param transaction 事务指针
 * @param root_is_latched 传出参数：根节点是否上锁，用于并发操作
 * @return 是否需要删除结点
 * @note User needs to first find the sibling of input page.
 * If sibling's size + input page's size >= 2 * page's minsize, then redistribute.
 * Otherwise, merge(Coalesce).
 */
bool IxIndexHandle::coalesce_or_redistribute(IxNodeHandle *node, Transaction *transaction, bool *root_is_latched) {
    // Todo:
    // 1. 判断node结点是否为根节点
    //    1.1 如果是根节点，需要调用AdjustRoot() 函数来进行处理，返回根节点是否需要被删除
    //    1.2 如果不是根节点，并且不需要执行合并或重分配操作，则直接返回false，否则执行2
    // 2. 获取node结点的父亲结点
    // 3. 寻找node结点的兄弟结点（优先选取前驱结点）
    // 4. 如果node结点和兄弟结点的键值对数量之和，能够支撑两个B+树结点（即node.size+neighbor.size >=
    // NodeMinSize*2)，则只需要重新分配键值对（调用Redistribute函数）
    // 5. 如果不满足上述条件，则需要合并两个结点，将右边的结点合并到左边的结点（调用Coalesce函数）

    // 1. 判断node结点是否为根节点
    if (node->get_parent_page_no() == IX_NO_PAGE) {
        // 1.1 如果是根节点，需要调用AdjustRoot() 函数来进行处理
        return adjust_root(node);
    }

    // 检查node节点的大小是否小于最小值
    if (node->get_size() >= node->get_min_size()) {
        // 1.2 如果不是根节点，并且不需要执行合并或重分配操作，则直接返回false
        return false;
    }

    // 2. 获取node结点的父亲结点
    IxNodeHandle *parent = fetch_node(node->get_parent_page_no());

    // 3. 寻找node结点的兄弟结点（优先选取前驱结点）
    int node_idx = parent->find_child(node);
    int neighbor_idx;

    // 优先选择前驱节点作为兄弟节点
    if (node_idx > 0) {
        neighbor_idx = node_idx - 1;  // 前驱节点
    } else {
        neighbor_idx = node_idx + 1;  // 后继节点
    }

    IxNodeHandle *neighbor_node = fetch_node(parent->value_at(neighbor_idx));

    // 4. 检查是否需要重分配键值对或合并节点
    if (node->get_size() + neighbor_node->get_size() >= 2 * node->get_min_size()) {
        // 如果键值对总数足够支撑两个节点，则进行重分配
        redistribute(neighbor_node, node, parent, node_idx);
        buffer_pool_manager_->unpin_page(parent->get_page_id(), true);
        buffer_pool_manager_->unpin_page(neighbor_node->get_page_id(), true);
        delete parent;
        delete neighbor_node;
        return false;
    } else {
        // 5. 如果键值对总数不足以支撑两个节点，则需要合并
        bool parent_should_delete = coalesce(&neighbor_node, &node, &parent, node_idx, transaction, root_is_latched);

        // 释放资源
        buffer_pool_manager_->unpin_page(parent->get_page_id(), true);
        buffer_pool_manager_->unpin_page(neighbor_node->get_page_id(), true);
        delete parent;
        delete neighbor_node;

        return parent_should_delete;
    }
}

/**
 * @brief 用于当根结点被删除了一个键值对之后的处理
 * @param old_root_node 原根节点
 * @return bool 根结点是否需要被删除
 * @note size of root page can be less than min size and this method is only called within coalesce_or_redistribute()
 */
bool IxIndexHandle::adjust_root(IxNodeHandle *old_root_node) {
    // Todo:
    // 1. 如果old_root_node是内部结点，并且大小为1，则直接把它的孩子更新成新的根结点
    // 2. 如果old_root_node是叶结点，且大小为0，则直接更新root page
    // 3. 除了上述两种情况，不需要进行操作

    // 1. 如果old_root_node是内部结点，并且大小为1
    if (!old_root_node->is_leaf_page() && old_root_node->get_size() == 1) {
        // 获取唯一子节点的页面编号
        int child_page_no = old_root_node->value_at(0);

        // 将子节点设置为新的根节点
        IxNodeHandle *new_root = fetch_node(child_page_no);
        new_root->set_parent_page_no(IX_NO_PAGE);  // 根节点的父节点为IX_NO_PAGE

        // 更新file_hdr_的root_page_
        file_hdr_->root_page_ = child_page_no;

        // 释放新根节点
        buffer_pool_manager_->unpin_page(new_root->get_page_id(), true);
        delete new_root;

        return true;  // 旧根节点需要被删除
    }

    // 2. 如果old_root_node是叶结点，且大小为0
    if (old_root_node->is_leaf_page() && old_root_node->get_size() == 0) {
        // 创建一个新的空根节点
        file_hdr_->root_page_ = IX_NO_PAGE;  // 没有根节点

        return true;  // 旧根节点需要被删除
    }

    // 3. 除了上述两种情况，不需要进行操作
    return false;  // 不需要删除节点
}

/**
 * @brief 重新分配node和兄弟结点neighbor_node的键值对
 * Redistribute key & value pairs from one page to its sibling page. If index == 0, move sibling page's first key
 * & value pair into end of input "node", otherwise move sibling page's last key & value pair into head of input "node".
 *
 * @param neighbor_node sibling page of input "node"
 * @param node input from method coalesceOrRedistribute()
 * @param parent the parent of "node" and "neighbor_node"
 * @param index node在parent中的rid_idx
 * @note node是之前刚被删除过一个key的结点
 * index=0，则neighbor是node后继结点，表示：node(left)      neighbor(right)
 * index>0，则neighbor是node前驱结点，表示：neighbor(left)  node(right)
 * 注意更新parent结点的相关kv对
 */
void IxIndexHandle::redistribute(IxNodeHandle *neighbor_node, IxNodeHandle *node, IxNodeHandle *parent, int index) {
    // Todo:
    // 1. 通过index判断neighbor_node是否为node的前驱结点
    // 2. 从neighbor_node中移动一个键值对到node结点中
    // 3. 更新父节点中的相关信息，并且修改移动键值对对应孩字结点的父结点信息（maintain_child函数）
    // 注意：neighbor_node的位置不同，需要移动的键值对不同，需要分类讨论

    // 1. 通过index判断neighbor_node是否为node的前驱结点
    bool neighbor_is_prev = (index > 0);  // 如果index>0，则neighbor是node前驱结点

    if (neighbor_is_prev) {
        // neighbor是前驱结点，移动neighbor的最后一个键值对到node的开头
        int last_idx = neighbor_node->get_size() - 1;
        // ix_debug_log << "[redistribute] move from neighbor(last_idx=" << last_idx << ") to node(0): key=";
        // debug_print_key(neighbor_node->get_key(last_idx), file_hdr_);
        // ix_debug_log << ", rid=";
        // debug_print_rid(neighbor_node->get_rid(last_idx));
        // ix_debug_log << std::endl;

        // 2. 从neighbor_node中移动一个键值对到node结点中
        char *last_key = neighbor_node->get_key(last_idx);
        Rid *last_rid = neighbor_node->get_rid(last_idx);

        // 将键值对插入到node的开头
        node->insert_pairs(0, last_key, last_rid, 1);

        // 从neighbor_node中删除最后一个键值对
        neighbor_node->erase_pair(last_idx);

        // 3. 更新父节点中的相关信息
        // 更新父节点中对应node的key
        parent->set_key(index, node->get_key(0));

        // 如果是内部节点，需要维护子节点的父指针
        if (!node->is_leaf_page()) {
            maintain_child(node, 0);
        }
    } else {
        // neighbor是后继结点，移动neighbor的第一个键值对到node的末尾
        // ix_debug_log << "[redistribute] move from neighbor(0) to node(end): key=";
        // debug_print_key(neighbor_node->get_key(0), file_hdr_);
        // ix_debug_log << ", rid=";
        // debug_print_rid(neighbor_node->get_rid(0));
        // ix_debug_log << std::endl;

        // 2. 从neighbor_node中移动一个键值对到node结点中
        char *first_key = neighbor_node->get_key(0);
        Rid *first_rid = neighbor_node->get_rid(0);

        // 将键值对插入到node的末尾
        node->insert_pairs(node->get_size(), first_key, first_rid, 1);

        // 从neighbor_node中删除第一个键值对
        neighbor_node->erase_pair(0);

        // 3. 更新父节点中的相关信息
        // 更新父节点中对应neighbor的key
        parent->set_key(index + 1, neighbor_node->get_key(0));

        // 如果是内部节点，需要维护子节点的父指针
        if (!node->is_leaf_page()) {
            maintain_child(node, node->get_size() - 1);
        }
    }
}

/**
 * @brief 合并(Coalesce)函数是将node和其直接前驱进行合并，也就是和它左边的neighbor_node进行合并；
 * 假设node一定在右边。如果上层传入的index=0，说明node在左边，那么交换node和neighbor_node，保证node在右边；合并到左结点，实际上就是删除了右结点；
 * Move all the key & value pairs from one page to its sibling page, and notify buffer pool manager to delete this page.
 * Parent page must be adjusted to take info of deletion into account. Remember to deal with coalesce or redistribute
 * recursively if necessary.
 *
 * @param neighbor_node sibling page of input "node" (neighbor_node是node的前结点)
 * @param node input from method coalesceOrRedistribute() (node结点是需要被删除的)
 * @param parent parent page of input "node"
 * @param index node在parent中的rid_idx
 * @return true means parent node should be deleted, false means no deletion happend
 * @note Assume that *neighbor_node is the left sibling of *node (neighbor -> node)
 */
bool IxIndexHandle::coalesce(IxNodeHandle **neighbor_node, IxNodeHandle **node, IxNodeHandle **parent, int index,
    Transaction *transaction, bool *root_is_latched) {
    // Todo:
    // 1. 用index判断neighbor_node是否为node的前驱结点，若不是则交换两个结点，让neighbor_node作为左结点，node作为右结点
    // 2. 把node结点的键值对移动到neighbor_node中，并更新node结点孩子结点的父节点信息（调用maintain_child函数）
    // 3. 释放和删除node结点，并删除parent中node结点的信息，返回parent是否需要被删除
    // 提示：如果是叶子结点且为最右叶子结点，需要更新file_hdr_.last_leaf

    // 1. 用index判断neighbor_node是否为node的前驱结点，若不是则交换两个结点
    bool neighbor_is_right = (index == 0);
    if (neighbor_is_right) {
    // 如果neighbor_node是右结点，交换node和neighbor_node
    std::swap(*neighbor_node, *node);
    index = 1;  // 交换后，node就在parent的第1个位置(原本neighbor的位置)
    }

    // 现在neighbor_node一定是左节点，node一定是右节点

    // 2. 把node结点的键值对移动到neighbor_node中
    int node_size = (*node)->get_size();

    // 获取node在parent中的key
    int parent_key_idx = index;  // node在parent中的key_idx

    // 如果是叶子节点，直接移动键值对
    if ((*node)->is_leaf_page()) {
    // for (int i = 0; i < node_size; i++) {
    //     ix_debug_log << "[coalesce] move from node(key_idx=" << i << ") to neighbor(end): key=";
    //     debug_print_key((*node)->get_key(i), file_hdr_);
    //     ix_debug_log << ", rid=";
    //     debug_print_rid((*node)->get_rid(i));
    //     ix_debug_log << std::endl;
    // }

    // 更新叶子节点链表
    (*neighbor_node)->set_next_leaf((*node)->get_next_leaf());

    // 如果node是最右叶子结点，需要更新file_hdr_.last_leaf
    if ((*node)->get_page_no() == file_hdr_->last_leaf_) {
    file_hdr_->last_leaf_ = (*neighbor_node)->get_page_no();
    }

    } else {
    // 如果是内部节点，先将父节点中对应的key插入到neighbor_node中
    char *parent_key = (*parent)->get_key(parent_key_idx);
    Rid dummy_rid = {.page_no = (*node)->value_at(0)};
    (*neighbor_node)->insert_pairs((*neighbor_node)->get_size(), parent_key, &dummy_rid, 1);

    // 然后将node中的所有键值对移动到neighbor_node中
    for (int i = 1; i < node_size; i++) {  // 从索引1开始，因为索引0的孩子已经被处理
    char *key = (*node)->get_key(i);
    Rid node_rid = {.page_no = (*node)->value_at(i)};
    (*neighbor_node)->insert_pairs((*neighbor_node)->get_size(), key, &node_rid, 1);
    }

    // 更新所有移动过的子节点的父指针
    for (int i = (*neighbor_node)->get_size() - node_size; i < (*neighbor_node)->get_size(); i++) {
    maintain_child(*neighbor_node, i);
    }
    }

    // 3. 删除parent中的node结点信息
    (*parent)->erase_pair(parent_key_idx);

    // 检查parent是否需要进行合并或重分配
    bool parent_should_delete = false;
    if ((*parent)->get_size() < (*parent)->get_min_size()) {
    parent_should_delete = coalesce_or_redistribute(*parent, transaction, root_is_latched);
    }

    return parent_should_delete;
}

/**
 * @brief 这里把iid转换成了rid，即iid的slot_no作为node的rid_idx(key_idx)
 * node其实就是把slot_no作为键值对数组的下标
 * 换而言之，每个iid对应的索引槽存了一对(key,rid)，指向了(要建立索引的属性首地址,插入/删除记录的位置)
 *
 * @param iid
 * @return Rid
 * @note iid和rid存的不是一个东西，rid是上层传过来的记录位置，iid是索引内部生成的索引槽位置
 */
 Rid IxIndexHandle::get_rid(const Iid &iid) const {
    IxNodeHandle *node = fetch_node(iid.page_no);
    if (iid.slot_no >= node->get_size()) {
        buffer_pool_manager_->unpin_page(node->get_page_id(), false);
        delete node;
        throw IndexEntryNotFoundError();
    }
    
    Rid result = *node->get_rid(iid.slot_no);
    
    // 检查是否为w_id=9
    if (file_hdr_->col_num_ > 0 && file_hdr_->col_types_[0] == TYPE_INT) {
        char *key = node->get_key(iid.slot_no);
        int key_value = *(int *)key;
        if (key_value == 9) {
            // std::cout << "[DEBUG-w_id=9] get_rid: iid={" << iid.page_no << ", " << iid.slot_no
            //           << "} -> rid={" << result.page_no << ", " << result.slot_no << "}" << std::endl;
        }
    }
    
    buffer_pool_manager_->unpin_page(node->get_page_id(), false);
    delete node;
    return result;
}

/**
 * @brief FindLeafPage + lower_bound
 *
 * @param key
 * @return Iid
 * @note 上层传入的key本来是int类型，通过(const char *)&key进行了转换
 * 可用*(int *)key转换回去
 */
Iid IxIndexHandle::lower_bound(const char *key) {
    // 找到key所在的叶子节点
    std::pair<IxNodeHandle *, bool> leaf_pair = find_leaf_page(key, Operation::FIND, nullptr);
    IxNodeHandle *leaf = leaf_pair.first;

    // 在叶子节点中查找第一个大于等于key的位置
    int slot_no = leaf->lower_bound(key);

    // 创建Iid
    Iid iid = {.page_no = leaf->get_page_no(), .slot_no = slot_no};

    // 释放叶子节点资源
    buffer_pool_manager_->unpin_page(leaf->get_page_id(), false);
    delete leaf;

    return iid;
}

/**
 * @brief FindLeafPage + upper_bound
 *
 * @param key
 * @return Iid
 */
Iid IxIndexHandle::upper_bound(const char *key) {
    // 找到key所在的叶子节点
    std::pair<IxNodeHandle *, bool> leaf_pair = find_leaf_page(key, Operation::FIND, nullptr);
    IxNodeHandle *leaf = leaf_pair.first;


    // 在叶子节点中查找第一个大于key的位置
    int slot_no = leaf->upper_bound(key);

    // 创建Iid
    Iid iid = {.page_no = leaf->get_page_no(), .slot_no = slot_no};

    // 如果已经超出当前叶子节点，且不是最后一个叶子节点
    if (slot_no >= leaf->get_size() && leaf->get_next_leaf() != IX_NO_PAGE) {
        // 获取下一个叶子节点
        iid.page_no = leaf->get_next_leaf();
        iid.slot_no = 0;
    }

    // 释放叶子节点资源
    buffer_pool_manager_->unpin_page(leaf->get_page_id(), false);
    delete leaf;

    return iid;
}

/**
 * @brief 指向最后一个叶子的最后一个结点的后一个
 * 用处在于可以作为IxScan的最后一个
 *
 * @return Iid
 */
Iid IxIndexHandle::leaf_end() const {
    // 检查索引是否为空
    if (file_hdr_->root_page_ == IX_NO_PAGE || file_hdr_->last_leaf_ == IX_NO_PAGE) {
        // 如果索引为空，返回一个表示结束的Iid
        Iid iid = {.page_no = IX_NO_PAGE, .slot_no = 0};
        return iid;
    }
    
    IxNodeHandle *node = fetch_node(file_hdr_->last_leaf_);
    Iid iid = {.page_no = file_hdr_->last_leaf_, .slot_no = node->get_size()};
    buffer_pool_manager_->unpin_page(node->get_page_id(), false);  // unpin it!
    return iid;
}

/**
 * @brief 指向第一个叶子的第一个结点
 * 用处在于可以作为IxScan的第一个
 *
 * @return Iid
 */
Iid IxIndexHandle::leaf_begin() const {
    // 检查索引是否为空
    if (file_hdr_->root_page_ == IX_NO_PAGE || file_hdr_->first_leaf_ == IX_NO_PAGE) {
        // 如果索引为空，返回一个表示结束的Iid
        return leaf_end();
    }
    
    Iid iid = {.page_no = file_hdr_->first_leaf_, .slot_no = 0};
    return iid;
}

/**
 * @brief 获取一个指定结点
 *
 * @param page_no
 * @return IxNodeHandle*
 * @note pin the page, remember to unpin it outside!
 */
IxNodeHandle *IxIndexHandle::fetch_node(int page_no) const {
    Page *page = buffer_pool_manager_->fetch_page(PageId{fd_, page_no});
    IxNodeHandle *node = new IxNodeHandle(file_hdr_, page);

    return node;
}

/**
 * @brief 创建一个新结点
 *
 * @return IxNodeHandle*
 * @note pin the page, remember to unpin it outside!
 * 注意：对于Index的处理是，删除某个页面后，认为该被删除的页面是free_page
 * 而first_free_page实际上就是最新被删除的页面，初始为IX_NO_PAGE
 * 在最开始插入时，一直是create node，那么first_page_no一直没变，一直是IX_NO_PAGE
 * 与Record的处理不同，Record将未插入满的记录页认为是free_page
 */
IxNodeHandle *IxIndexHandle::create_node() {
    IxNodeHandle *node;
    file_hdr_->num_pages_++;

    PageId new_page_id = {.fd = fd_, .page_no = INVALID_PAGE_ID};
    // 从3开始分配page_no，第一次分配之后，new_page_id.page_no=3，file_hdr_.num_pages=4
    Page *page = buffer_pool_manager_->new_page(&new_page_id);
    node = new IxNodeHandle(file_hdr_, page);
    return node;
}

/**
 * @brief 从node开始更新其父节点的第一个key，一直向上更新直到根节点
 *
 * @param node
 */
void IxIndexHandle::maintain_parent(IxNodeHandle *node) {
    IxNodeHandle *curr = node;
    while (curr->get_parent_page_no() != IX_NO_PAGE) {
        // Load its parent
        IxNodeHandle *parent = fetch_node(curr->get_parent_page_no());
        int rank = parent->find_child(curr);
        char *parent_key = parent->get_key(rank);
        char *child_first_key = curr->get_key(0);
        if (memcmp(parent_key, child_first_key, file_hdr_->col_tot_len_) == 0) {
            assert(buffer_pool_manager_->unpin_page(parent->get_page_id(), true));
            break;
        }
        memcpy(parent_key, child_first_key, file_hdr_->col_tot_len_);  // 修改了parent node
        curr = parent;

        assert(buffer_pool_manager_->unpin_page(parent->get_page_id(), true));
    }
}

/**
 * @brief 要删除leaf之前调用此函数，更新leaf前驱结点的next指针和后继结点的prev指针
 *
 * @param leaf 要删除的leaf
 */
void IxIndexHandle::erase_leaf(IxNodeHandle *leaf) {
    assert(leaf->is_leaf_page());

    IxNodeHandle *prev = fetch_node(leaf->get_prev_leaf());
    prev->set_next_leaf(leaf->get_next_leaf());
    buffer_pool_manager_->unpin_page(prev->get_page_id(), true);

    IxNodeHandle *next = fetch_node(leaf->get_next_leaf());
    next->set_prev_leaf(leaf->get_prev_leaf());  // 注意此处是SetPrevLeaf()
    buffer_pool_manager_->unpin_page(next->get_page_id(), true);
}

/**
 * @brief 删除node时，更新file_hdr_.num_pages
 *
 * @param node
 */
void IxIndexHandle::release_node_handle(IxNodeHandle &node) {
    file_hdr_->num_pages_--;
}

/**
 * @brief 将node的第child_idx个孩子结点的父节点置为node
 */
void IxIndexHandle::maintain_child(IxNodeHandle *node, int child_idx) {
    if (!node->is_leaf_page()) {
        //  Current node is inner node, load its child and set its parent to current node
        int child_page_no = node->value_at(child_idx);
        IxNodeHandle *child = fetch_node(child_page_no);
        child->set_parent_page_no(node->get_page_no());
        buffer_pool_manager_->unpin_page(child->get_page_id(), true);
    }
}

std::unique_ptr<IxScan> IxIndexHandle::create_scan(const char* lower_key, const char* upper_key,
    bool lower_inclusive, bool upper_inclusive) {
    // 找到范围的起始位置
    Iid begin_iid, end_iid;

    // 找到下界位置
    if (lower_key == nullptr) {
        // 从第一个叶子节点的第一个键值开始
        begin_iid = leaf_begin();
    } else {
        // 根据lower_key确定起始位置
        if (lower_inclusive) {
            // 包含等于下界的值，使用lower_bound查找第一个大于等于lower_key的位置
            begin_iid = lower_bound(lower_key);
        } else {
            // 不包含等于下界的值，使用upper_bound查找第一个大于lower_key的位置
            begin_iid = upper_bound(lower_key);
        }
    }

    // 找到上界位置
    if (upper_key == nullptr) {
        // 扫描到最后一个叶子节点的结束位置
        end_iid = leaf_end();
    } else {
        // 根据upper_key确定结束位置
        if (upper_inclusive) {
            // 包含等于上界的值，使用upper_bound查找第一个大于upper_key的位置
            end_iid = upper_bound(upper_key);
        } else {
            // 不包含等于上界的值，使用lower_bound查找第一个大于等于upper_key的位置
            end_iid = lower_bound(upper_key);
        }
    }

    // 创建并返回扫描对象
    return std::make_unique<IxScan>(this, begin_iid, end_iid, buffer_pool_manager_);
}

void IxIndexHandle::print_tree(const std::string& outfile_path) {
    std::ofstream outfile(outfile_path, std::ios::out);
    if (!outfile) {
        std::cerr << "Failed to open file: " << outfile_path << std::endl;
        return;
    }

    // 打印file_hdr_的first_leaf_和last_leaf_
    outfile << "[file_hdr_] first_leaf_ = " << file_hdr_->first_leaf_ << ", last_leaf_ = " << file_hdr_->last_leaf_ << std::endl;

    if (file_hdr_->root_page_ == IX_NO_PAGE) {
        outfile << "Empty tree" << std::endl;
        outfile.close();
        return;
    }

    std::queue<std::pair<page_id_t, int>> q;  // 使用页面ID和层级
    q.push({file_hdr_->root_page_, 0});

    int current_level = -1;
    outfile << "=== B+ Tree Structure ===" << std::endl;
    
    while (!q.empty()) {
        auto node_pair = q.front();
        page_id_t page_no = node_pair.first;
        int level = node_pair.second;
        q.pop();

        // 打印层级分隔符
        if (level > current_level) {
            current_level = level;
            outfile << "\n--- Level " << level << " ---" << std::endl;
        }

        // 获取节点
        IxNodeHandle* node = fetch_node(page_no);
        
        // 打印节点基本信息
        outfile << "Node[" << node->get_page_no() << "] ";
        outfile << (node->is_leaf_page() ? "(Leaf) " : "(Internal) ");
        outfile << "parent_page_no=" << node->get_parent_page_no() << ", ";
        outfile << "num_key=" << node->get_size();
        if (node->is_leaf_page()) {
            outfile << ", prev_leaf=" << node->get_prev_leaf();
            outfile << ", next_leaf=" << node->get_next_leaf();
        }
        outfile << std::endl;

        // 打印节点的key和rid
        if (node->is_leaf_page()) {
            outfile << "  Keys&Rids: ";
            for (int i = 0; i < node->get_size(); i++) {
                char* key = node->get_key(i);
                Rid* rid = node->get_rid(i);
                outfile << "[";
                // 打印key
                for (int j = 0; j < file_hdr_->col_num_; j++) {
                    ColType type = file_hdr_->col_types_[j];
                    int len = file_hdr_->col_lens_[j];
                    if (j > 0) outfile << ",";
                    int offset = 0;
                    for (int k = 0; k < j; k++) offset += file_hdr_->col_lens_[k];
                    switch (type) {
                        case TYPE_INT: outfile << *(int*)(key + offset); break;
                        case TYPE_FLOAT: outfile << *(float*)(key + offset); break;
                        case TYPE_STRING: outfile << "'" << std::string(key + offset, strnlen(key + offset, len)) << "'"; break;
                        default: outfile << "?";
                    }
                }
                outfile << "|rid=(" << rid->page_no << "," << rid->slot_no << ")] ";
            }
            outfile << std::endl;
            // 打印key范围
            if (node->get_size() > 0) {
                char* min_key = node->get_key(0);
                char* max_key = node->get_key(node->get_size() - 1);
                outfile << "  KeyRange: min=[";
                for (int j = 0; j < file_hdr_->col_num_; j++) {
                    ColType type = file_hdr_->col_types_[j];
                    int len = file_hdr_->col_lens_[j];
                    if (j > 0) outfile << ",";
                    int offset = 0;
                    for (int k = 0; k < j; k++) offset += file_hdr_->col_lens_[k];
                    switch (type) {
                        case TYPE_INT: outfile << *(int*)(min_key + offset); break;
                        case TYPE_FLOAT: outfile << *(float*)(min_key + offset); break;
                        case TYPE_STRING: outfile << "'" << std::string(min_key + offset, strnlen(min_key + offset, len)) << "'"; break;
                        default: outfile << "?";
                    }
                }
                outfile << "], max=[";
                for (int j = 0; j < file_hdr_->col_num_; j++) {
                    ColType type = file_hdr_->col_types_[j];
                    int len = file_hdr_->col_lens_[j];
                    if (j > 0) outfile << ",";
                    int offset = 0;
                    for (int k = 0; k < j; k++) offset += file_hdr_->col_lens_[k];
                    switch (type) {
                        case TYPE_INT: outfile << *(int*)(max_key + offset); break;
                        case TYPE_FLOAT: outfile << *(float*)(max_key + offset); break;
                        case TYPE_STRING: outfile << "'" << std::string(max_key + offset, strnlen(max_key + offset, len)) << "'"; break;
                        default: outfile << "?";
                    }
                }
                outfile << "]" << std::endl;
            }
        } else {
            // 打印内部节点的key
            outfile << "  Keys: ";
            for (int i = 0; i < node->get_size(); i++) {
                char* key = node->get_key(i);
                outfile << "[";
                for (int j = 0; j < file_hdr_->col_num_; j++) {
                    ColType type = file_hdr_->col_types_[j];
                    int len = file_hdr_->col_lens_[j];
                    if (j > 0) outfile << ",";
                    int offset = 0;
                    for (int k = 0; k < j; k++) offset += file_hdr_->col_lens_[k];
                    switch (type) {
                        case TYPE_INT: outfile << *(int*)(key + offset); break;
                        case TYPE_FLOAT: outfile << *(float*)(key + offset); break;
                        case TYPE_STRING: outfile << "'" << std::string(key + offset, strnlen(key + offset, len)) << "'"; break;
                        default: outfile << "?";
                    }
                }
                outfile << "] ";
            }
            outfile << std::endl;
            // 打印内部节点的所有子指针和parent_page_no
            outfile << "  Children: ";
            for (int i = 0; i <= node->get_size(); i++) {
                int child_page_no = node->value_at(i);
                IxNodeHandle* child = fetch_node(child_page_no);
                outfile << child_page_no << "(parent=" << child->get_parent_page_no() << ") ";
                q.push({child_page_no, level + 1});
                buffer_pool_manager_->unpin_page(child->get_page_id(), false);
                delete child;
            }
            outfile << std::endl;
        }

        outfile << std::endl;
        buffer_pool_manager_->unpin_page(node->get_page_id(), false);
        delete node;
    }

    outfile.close();
}