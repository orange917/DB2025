/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include <iostream>
#include <memory>
#include <string>
#include "recovery/log_manager.h"
#include "recovery/log_recovery.h"
#include "storage/disk_manager.h"
#include "storage/buffer_pool_manager.h"
#include "system/sm_manager.h"
#include "record/rm_manager.h"
#include "index/ix_manager.h"
#include "transaction/concurrency/lock_manager.h"

/**
 * @description: 测试故障恢复功能
 * 1. 创建数据库和表
 * 2. 插入一些数据
 * 3. 模拟系统崩溃
 * 4. 重启系统并进行故障恢复
 * 5. 验证数据一致性
 */
void test_recovery() {
    std::cout << "=== 开始测试故障恢复功能 ===" << std::endl;
    
    // 创建管理器对象
    auto disk_manager = std::make_unique<DiskManager>();
    auto buffer_pool_manager = std::make_unique<BufferPoolManager>(BUFFER_POOL_SIZE, disk_manager.get());
    auto rm_manager = std::make_unique<RmManager>(disk_manager.get(), buffer_pool_manager.get());
    auto ix_manager = std::make_unique<IxManager>(disk_manager.get(), buffer_pool_manager.get());
    auto sm_manager = std::make_unique<SmManager>(disk_manager.get(), buffer_pool_manager.get(), rm_manager.get(), ix_manager.get());
    auto lock_manager = std::make_unique<LockManager>();
    auto log_manager = std::make_unique<LogManager>(disk_manager.get());
    auto recovery = std::make_unique<RecoveryManager>(disk_manager.get(), buffer_pool_manager.get(), sm_manager.get());
    
    try {
        // 1. 创建测试数据库
        std::string db_name = "test_recovery_db";
        if (!sm_manager->is_dir(db_name)) {
            sm_manager->create_db(db_name);
        }
        sm_manager->open_db(db_name);
        
        // 2. 创建测试表
        std::string table_name = "test_table";
        std::vector<ColDef> col_defs = {
            {"id", TYPE_INT, 4, false},
            {"name", TYPE_STRING, 32, false}
        };
        
        sm_manager->create_table(table_name, col_defs);
        std::cout << "创建表 " << table_name << " 成功" << std::endl;
        
        // 3. 插入测试数据
        std::cout << "开始插入测试数据..." << std::endl;
        
        // 创建事务上下文
        char data_send[BUFFER_LENGTH];
        int offset = 0;
        Context context(lock_manager.get(), log_manager.get(), nullptr, data_send, &offset);
        context.tab_name_ = table_name;
        
        // 插入记录1
        {
            RmRecord record(36); // 4 + 32
            record.size = 36;
            *(int*)record.data = 1;
            strcpy(record.data + 4, "Alice");
            
            // 写入BEGIN日志
            BeginLogRecord begin_log(1);
            log_manager->add_log_to_buffer(&begin_log);
            
            // 插入记录
            Rid rid1 = rm_manager->insert_record(table_name, record.data, &context);
            std::cout << "插入记录1: id=1, name=Alice, rid=(" << rid1.page_no << "," << rid1.slot_no << ")" << std::endl;
            
            // 写入COMMIT日志
            CommitLogRecord commit_log(1);
            log_manager->add_log_to_buffer(&commit_log);
            
            // 强制刷新日志
            log_manager->flush_log_to_disk();
        }
        
        // 插入记录2
        {
            RmRecord record(36);
            record.size = 36;
            *(int*)record.data = 2;
            strcpy(record.data + 4, "Bob");
            
            // 写入BEGIN日志
            BeginLogRecord begin_log(2);
            log_manager->add_log_to_buffer(&begin_log);
            
            // 插入记录
            Rid rid2 = rm_manager->insert_record(table_name, record.data, &context);
            std::cout << "插入记录2: id=2, name=Bob, rid=(" << rid2.page_no << "," << rid2.slot_no << ")" << std::endl;
            
            // 写入COMMIT日志
            CommitLogRecord commit_log(2);
            log_manager->add_log_to_buffer(&commit_log);
            
            // 强制刷新日志
            log_manager->flush_log_to_disk();
        }
        
        // 4. 模拟系统崩溃（不写入COMMIT日志）
        {
            RmRecord record(36);
            record.size = 36;
            *(int*)record.data = 3;
            strcpy(record.data + 4, "Charlie");
            
            // 写入BEGIN日志
            BeginLogRecord begin_log(3);
            log_manager->add_log_to_buffer(&begin_log);
            
            // 插入记录但不提交
            Rid rid3 = rm_manager->insert_record(table_name, record.data, &context);
            std::cout << "插入记录3（未提交）: id=3, name=Charlie, rid=(" << rid3.page_no << "," << rid3.slot_no << ")" << std::endl;
            
            // 不写入COMMIT日志，模拟崩溃
            log_manager->flush_log_to_disk();
        }
        
        std::cout << "模拟系统崩溃..." << std::endl;
        
        // 5. 重启系统并进行故障恢复
        std::cout << "重启系统，开始故障恢复..." << std::endl;
        
        // 重新创建管理器对象（模拟重启）
        disk_manager = std::make_unique<DiskManager>();
        buffer_pool_manager = std::make_unique<BufferPoolManager>(BUFFER_POOL_SIZE, disk_manager.get());
        rm_manager = std::make_unique<RmManager>(disk_manager.get(), buffer_pool_manager.get());
        ix_manager = std::make_unique<IxManager>(disk_manager.get(), buffer_pool_manager.get());
        sm_manager = std::make_unique<SmManager>(disk_manager.get(), buffer_pool_manager.get(), rm_manager.get(), ix_manager.get());
        lock_manager = std::make_unique<LockManager>();
        log_manager = std::make_unique<LogManager>(disk_manager.get());
        recovery = std::make_unique<RecoveryManager>(disk_manager.get(), buffer_pool_manager.get(), sm_manager.get());
        
        // 打开数据库
        sm_manager->open_db(db_name);
        
        // 执行故障恢复
        recovery->recover();
        
        // 6. 验证数据一致性
        std::cout << "验证数据一致性..." << std::endl;
        
        // 读取所有记录
        auto table_file = rm_manager->open_file(table_name);
        int record_count = table_file->get_records_num();
        std::cout << "恢复后表中的记录数: " << record_count << std::endl;
        
        // 应该只有2条记录（已提交的），第3条记录应该被回滚
        if (record_count == 2) {
            std::cout << "✓ 故障恢复成功：数据一致性验证通过" << std::endl;
        } else {
            std::cout << "✗ 故障恢复失败：数据一致性验证失败" << std::endl;
        }
        
        // 清理测试数据库
        sm_manager->close_db();
        sm_manager->destroy_db(db_name);
        
    } catch (const std::exception& e) {
        std::cerr << "测试过程中发生异常: " << e.what() << std::endl;
    }
    
    std::cout << "=== 故障恢复功能测试完成 ===" << std::endl;
}

int main() {
    test_recovery();
    return 0;
} 