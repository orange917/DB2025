/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "sm_manager.h"

#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include <fstream>
#include <set>

#include "index/ix.h"
#include "record/rm.h"
#include "record_printer.h"

/**
 * @description: 判断是否为一个文件夹
 * @return {bool} 返回是否为一个文件夹
 * @param {string&} db_name 数据库文件名称，与文件夹同名
 */
bool SmManager::is_dir(const std::string& db_name) {
    struct stat st;
    return stat(db_name.c_str(), &st) == 0 && S_ISDIR(st.st_mode);
}

/**
 * @description: 创建数据库，所有的数据库相关文件都放在数据库同名文件夹下
 * @param {string&} db_name 数据库名称
 */
void SmManager::create_db(const std::string& db_name) {
    if (is_dir(db_name)) {
        throw DatabaseExistsError(db_name);
    }
    //为数据库创建一个子目录
    std::string cmd = "mkdir " + db_name;
    if (system(cmd.c_str()) < 0) {  // 创建一个名为db_name的目录
        throw UnixError();
    }
    if (chdir(db_name.c_str()) < 0) {  // 进入名为db_name的目录
        throw UnixError();
    }
    //创建系统目录
    DbMeta *new_db = new DbMeta();
    new_db->name_ = db_name;

    // 注意，此处ofstream会在当前目录创建(如果没有此文件先创建)和打开一个名为DB_META_NAME的文件
    std::ofstream ofs(DB_META_NAME);

    // 将new_db中的信息，按照定义好的operator<<操作符，写入到ofs打开的DB_META_NAME文件中
    ofs << *new_db;  // 注意：此处重载了操作符<<

    delete new_db;

    // 创建日志文件
    disk_manager_->create_file(LOG_FILE_NAME);

    // 回到根目录
    if (chdir("..") < 0) {
        throw UnixError();
    }
}

/**
 * @description: 删除数据库，同时需要清空相关文件以及数据库同名文件夹
 * @param {string&} db_name 数据库名称，与文件夹同名
 */
void SmManager::drop_db(const std::string& db_name) {
    if (!is_dir(db_name)) {
        throw DatabaseNotFoundError(db_name);
    }
    std::string cmd = "rm -r " + db_name;
    if (system(cmd.c_str()) < 0) {
        throw UnixError();
    }
}

/**
 * @description: 打开数据库，找到数据库对应的文件夹，并加载数据库元数据和相关文件
 * @param {string&} db_name 数据库名称，与文件夹同名
 */
void SmManager::open_db(const std::string& db_name) {
    std::lock_guard<std::mutex> lock(meta_mutex_);
    // 先检查一下是否存在目录
    if(!is_dir(db_name)) {
        throw DatabaseNotFoundError(db_name);
    }
    // 进入数据库目录
    if(chdir(db_name.c_str()) < 0) {
        throw UnixError();
    }

    // 打开数据库元数据文件
    std::ifstream ifs(DB_META_NAME);
    ifs >> db_;

    if (!ifs) {
        throw DatabaseNotFoundError(db_name);
    }

    // 加载所有表的文件句柄
    for (auto &entry : db_.tabs_) {
        auto &tab = entry.second;
        fhs_[tab.name] = rm_manager_->open_file(tab.name);

        // 打开索引文件 - 遍历表中的所有索引
        for (auto &index : tab.indexes) {
            auto ih = ix_manager_->open_index(tab.name, index.cols);
            std::string index_name = ix_manager_->get_index_name(tab.name, index.cols);
            ihs_[index_name] = std::move(ih);
        }
    }
}

/**
 * @description: 把数据库相关的元数据刷入磁盘中
 */
void SmManager::flush_meta() {
    // std::lock_guard<std::mutex> lock(meta_mutex_);
    // 默认清空文件
    std::ofstream ofs(DB_META_NAME);
    ofs << db_;
}

/**
 * @description: 关闭数据库并把数据落盘
 */
void SmManager::close_db() {
    std::lock_guard<std::mutex> lock(meta_mutex_);
    // 刷新一下所有的page
    for(auto &rm_file : fhs_) {
        buffer_pool_manager_->flush_all_pages(rm_file.second->GetFd());
    }

    // 回到根目录
    if (chdir("..") < 0) {
        throw UnixError();
    }

    // 关闭所有的文件句柄
    for(auto &rm_file : fhs_) {
        rm_manager_->close_file(rm_file.second.get());
    }
    fhs_.clear();

    // 关闭所有的索引句柄
    for(auto &ix_file : ihs_) {
        ix_manager_->close_index(ix_file.second.get());
    }
    ihs_.clear();

    // 清空数据库元数据
    db_.tabs_.clear();
    db_.name_.clear();
}

/**
 * @description: 显示所有的表,通过测试需要将其结果写入到output.txt,详情看题目文档
 * @param {Context*} context 
 */
void SmManager::show_tables(Context* context) {
    std::lock_guard<std::mutex> lock(meta_mutex_);
    std::fstream outfile;
    outfile.open("output.txt", std::ios::out | std::ios::app);
    outfile << "| Tables |\n";
    RecordPrinter printer(1);
    printer.print_separator(context);
    printer.print_record({"Tables"}, context);
    printer.print_separator(context);
    for (auto &entry : db_.tabs_) {
        auto &tab = entry.second;
        printer.print_record({tab.name}, context);
        outfile << "| " << tab.name << " |\n";
    }
    printer.print_separator(context);
    outfile.close();
}

/**
 * @description: 显示表的元数据
 * @param {string&} tab_name 表名称
 * @param {Context*} context 
 */
void SmManager::desc_table(const std::string& tab_name, Context* context) {
    std::lock_guard<std::mutex> lock(meta_mutex_);
    TabMeta &tab = db_.get_table(tab_name);

    std::vector<std::string> captions = {"Field", "Type", "Index"};
    RecordPrinter printer(captions.size());
    // Print header
    printer.print_separator(context);
    printer.print_record(captions, context);
    printer.print_separator(context);
    // Print fields
    for (auto &col : tab.cols) {
        std::vector<std::string> field_info = {col.name, coltype2str(col.type), col.index ? "YES" : "NO"};
        printer.print_record(field_info, context);
    }
    // Print footer
    printer.print_separator(context);
}

/**
 * @description: 创建表
 * @param {string&} tab_name 表的名称
 * @param {vector<ColDef>&} col_defs 表的字段
 * @param {Context*} context
 */
void SmManager::create_table(const std::string& tab_name, const std::vector<ColDef>& col_defs, Context* context) {
    std::lock_guard<std::mutex> lock(meta_mutex_);
    // 检查表名是否为空
    if (tab_name.empty()) {
        throw TableNotFoundError("Table name cannot be empty");
    }

    // 检查表是否已存在
    if (db_.is_table(tab_name)) {
        throw TableExistsError(tab_name);
    }

    // 检查列定义是否为空
    if (col_defs.empty()) {
        throw InvalidValueCountError();
    }

    // 检查列名是否重复
    std::set<std::string> col_names;
    for (const auto& col_def : col_defs) {
        if (col_names.find(col_def.name) != col_names.end()) {
            throw ColumnNotFoundError(col_def.name);
            std::cout << "Column name '" << col_def.name << "' is duplicated." << std::endl;
        }
        col_names.insert(col_def.name);
    }

    // 创建表元数据
    int curr_offset = 0;
    TabMeta tab;
    tab.name = tab_name;
    for (auto &col_def : col_defs) {
        ColMeta col = {.tab_name = tab_name,
                       .name = col_def.name,
                       .type = col_def.type,
                       .len = col_def.len,
                       .offset = curr_offset,
                       .index = false};
        curr_offset += col_def.len;
        tab.cols.push_back(col);
    }

    // 创建并打开记录文件
    int record_size = curr_offset;
    try {
        rm_manager_->create_file(tab_name, record_size);
        db_.tabs_[tab_name] = tab;
        fhs_.emplace(tab_name, rm_manager_->open_file(tab_name));
        flush_meta();
    } catch (const std::exception& e) {
        // 如果创建失败，清理已创建的资源
        if (db_.tabs_.find(tab_name) != db_.tabs_.end()) {
            db_.tabs_.erase(tab_name);
        }
        if (fhs_.find(tab_name) != fhs_.end()) {
            fhs_.erase(tab_name);
        }
        throw;
    }
}

/**
 * @description: 删除表
 * @param {string&} tab_name 表的名称
 * @param {Context*} context
 */
 void SmManager::drop_table(const std::string& tab_name, Context* context) {
    std::lock_guard<std::mutex> lock(meta_mutex_);
    if(!db_.is_table(tab_name)) {
        throw TableNotFoundError(tab_name);
    }

    // Step 1: Clean up and delete all associated indexes
    auto indexes_to_drop = db_.tabs_[tab_name].indexes;
    for (const auto& index_meta : indexes_to_drop) {
        std::vector<std::string> col_names;
        for (const auto& col : index_meta.cols) {
            col_names.push_back(col.name);
        }
        std::string index_name = ix_manager_->get_index_name(tab_name, col_names);
        auto ih_iter = ihs_.find(index_name);
        if (ih_iter != ihs_.end()) {
            int fd = ih_iter->second->get_fd();
            // 从缓冲池中丢弃所有索引页，不再写入磁盘
            buffer_pool_manager_->discard_all_pages(fd);
            // 关闭索引文件句柄
            ix_manager_->close_index(ih_iter->second.get());
            ihs_.erase(ih_iter);
        }
        // 删除物理索引文件
        ix_manager_->destroy_index(tab_name, index_meta.cols);
    }

    // Step 2: Clean up table file handle
    auto fh_iter = fhs_.find(tab_name);
    if (fh_iter != fhs_.end()) {
        int fd = fh_iter->second->GetFd();
        buffer_pool_manager_->discard_all_pages(fd);
        rm_manager_->close_file(fh_iter->second.get());
        fhs_.erase(fh_iter);
    }

    // Step 3: 删除物理表文件
    rm_manager_->destroy_file(tab_name);

    // Step 4: Remove the table's metadata.
    db_.tabs_.erase(tab_name);

    // Step 5: Persist the metadata changes.
    flush_meta();
}

/**
 * @description: 创建索引
 * @param {string&} tab_name 表的名称
 * @param {vector<string>&} col_names 索引包含的字段名称
 * @param {Context*} context
 */
void SmManager::create_index(const std::string& tab_name, const std::vector<std::string>& col_names, Context* context) {
    std::lock_guard<std::mutex> lock(meta_mutex_);
    // 检查表是否存在
    if(!db_.is_table(tab_name)) {
        throw TableNotFoundError(tab_name);
    }
    // 检查表的物理文件是否存在
    if (!disk_manager_->is_file(tab_name)) {
        throw TableNotFoundError(tab_name);
    }

    TabMeta& tab = db_.get_table(tab_name);

    // 检查索引是否已经存在
    if(tab.is_index(col_names)) {
        throw IndexExistsError(tab_name, col_names);
    }

    // 检查字段是否存在
    for (const auto& col_name : col_names) {
        if (!tab.is_col(col_name)) {
            throw ColumnNotFoundError(col_name);
        }
    }

    // 创建索引元数据
    std::vector<ColMeta> cols;
    for (const auto& col_name : col_names) {
        auto col_it = tab.get_col(col_name);
        cols.push_back(*col_it);
    }

    // 创建索引文件
    std::string index_name = ix_manager_->get_index_name(tab_name, col_names);
    if (ihs_.find(index_name) != ihs_.end() || ix_manager_->exists(tab_name, col_names)) {
        throw IndexExistsError(tab_name, col_names);
    }

    // 创建索引
    ix_manager_->create_index(tab_name, cols);
    IndexMeta index_meta;
    index_meta.tab_name = tab_name;
    index_meta.cols = cols;
    index_meta.col_num = cols.size();
    index_meta.col_tot_len = 0;
    index_meta.unique = true; // 唯一索引
    for(const auto& col : cols) {
        index_meta.col_tot_len += col.len;
    }
    auto ih = ix_manager_->open_index(tab_name, cols, &index_meta);
    ihs_[index_name] = std::move(ih);

    // 将索引信息添加到表的元数据中
    for (const auto& col_name : col_names) {
        auto col_it = tab.get_col(col_name);
        col_it->index = true;
    }

    // 添加索引到表的索引列表
    tab.indexes.push_back(index_meta);

    
    // 将已有的数据插入到索引中
    auto file_handle = fhs_.at(tab_name).get();
    auto scan = file_handle->create_scan();
    try {
        while (!scan->is_end()) {
            auto rid = scan->rid();
            auto record = file_handle->get_record(rid, context);
            if (record) {
                // 从记录中提取键值并插入到索引中
                char* key = new char[index_meta.col_tot_len];
                int offset = 0;
                for (const auto& col : cols) {
                    memcpy(key + offset, record->data + col.offset, col.len);
                    offset += col.len;
                }
                ihs_[index_name]->insert_entry(key, rid, nullptr); // 若唯一性冲突会抛异常
                delete[] key;
            }
            scan->next();
        }
    } catch (const std::exception& e) {
        // 回滚：删除索引文件和元数据
        ihs_.erase(index_name);
        ix_manager_->destroy_index(tab_name, cols);
        tab.indexes.pop_back();
        // 回滚列的索引标志
        for (const auto& col_name : col_names) {
            auto col_it = tab.get_col(col_name);
            col_it->index = false; // 确保标志位被重置
        }
        flush_meta();
        throw; // 继续抛出异常
    }

    // 索引条目已成功插入内存（缓冲池），现在需要将它们持久化到磁盘
    buffer_pool_manager_->flush_all_pages(ihs_[index_name]->get_fd());

    // 获取当前工作目录
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        std::string current_path(cwd);
        std::string tree_file = tab_name + "_" + index_name + "_tree.txt";
        std::string full_path = current_path + "/" + tree_file;

        std::cout << "Generating B+ tree structure file at: " << full_path << std::endl;

        // 打印B+树结构到文件
        ihs_[index_name]->print_tree(tree_file);

        // 检查文件是否创建成功
        std::ifstream f(tree_file.c_str());
        if (f.good()) {
            std::cout << "B+ tree structure file created successfully" << std::endl;
        } else {
            std::cout << "Failed to create B+ tree structure file" << std::endl;
        }
    } else {
        std::cerr << "Failed to get current directory path" << std::endl;
    }

    // 将修改后的元数据写入磁盘
    flush_meta();
}

/**
 * @description: 删除索引
 * @param {string&} tab_name 表名称
 * @param {vector<string>&} col_names 索引包含的字段名称
 * @param {Context*} context
 */
 void SmManager::drop_index(const std::string& tab_name, const std::vector<std::string>& col_names, Context* context) {
    // Note: This function is called by drop_table, so it doesn't lock the mutex itself.
    // The caller (e.g., drop_table or the SQL execution engine) is responsible for locking.
    TabMeta& tab = db_.get_table(tab_name);

    // 检查索引是否存在于元数据中
    if (!tab.is_index(col_names)) {
        // 如果元数据中没有，可能是一个悬挂的索引文件，仍然尝试删除
        // 但首先要检查表是否存在
        if (!db_.is_table(tab_name)) {
            throw TableNotFoundError(tab_name);
        }
        // 如果表存在但索引元数据不存在，则抛出索引未找到错误
        throw IndexNotFoundError(tab_name, col_names);
    }

    // 获取索引名
    std::string index_name = ix_manager_->get_index_name(tab_name, col_names);

   // 关闭并删除索引 handle
   auto ih_iter = ihs_.find(index_name);
   if (ih_iter != ihs_.end()) {
       // 在关闭和删除文件前，从缓冲池中刷新并删除所有相关页面
       int fd = ih_iter->second->get_fd();
       buffer_pool_manager_->flush_all_pages(fd);

       ix_manager_->close_index(ih_iter->second.get());
       ihs_.erase(ih_iter);
   }

   // 删除物理索引文件
   ix_manager_->destroy_index(tab_name, col_names);

    // 从元数据中移除索引
    auto index_iter = tab.get_index_meta(col_names);
    tab.indexes.erase(index_iter);

    // 更新受影响列的索引标记
    for (const auto& col_name : col_names) {
        bool still_indexed = false;
        // 检查此列是否被其他索引使用
        for (const auto& other_index : tab.indexes) {
            for (const auto& other_col : other_index.cols) {
                if (other_col.name == col_name) {
                    still_indexed = true;
                    break;
                }
            }
            if (still_indexed) break;
        }
        // 如果没有其他索引使用此列，则更新其元数据
        if (!still_indexed) {
            auto col_it = tab.get_col(col_name);
            if (col_it != tab.cols.end()) {
                col_it->index = false;
            }
        }
    }
    flush_meta();
}

/**
 * @description: 显示表的索引
 * @param {string&} tab_name 表名称
 * @param {Context*} context 
 */
void SmManager::show_index(const std::string& tab_name, Context* context) {
    std::lock_guard<std::mutex> lock(meta_mutex_);
    std::fstream outfile;
    outfile.open("output.txt", std::ios::out | std::ios::app);
    if (!db_.is_table(tab_name)) {
        outfile << "No such table: " << tab_name << "\n";
        outfile.close();
        return;
    }
    TabMeta& tab = db_.get_table(tab_name);
    
    // Use RecordPrinter to write to context output buffer
    std::vector<std::string> captions = {"Table", "Type", "Columns"};
    RecordPrinter printer(captions.size());
    printer.print_separator(context);
    printer.print_record(captions, context);
    printer.print_separator(context);
    
    for (const auto& index : tab.indexes) {
        // Build column list string
        std::string col_list = "(";
        for (size_t i = 0; i < index.cols.size(); ++i) {
            col_list += index.cols[i].name;
            if (i + 1 < index.cols.size()) col_list += ",";
        }
        col_list += ")";
        
        // Print to context buffer
        std::vector<std::string> index_info = {tab.name, "unique", col_list};
        printer.print_record(index_info, context);
        
        // Print to file for testing
        outfile << "| " << tab.name << " | unique | " << col_list << " |\n";
    }
    
    printer.print_separator(context);
    outfile.close();
}