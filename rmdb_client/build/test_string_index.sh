#!/bin/bash

# 测试字符串索引的脚本
echo "=== 字符串索引测试脚本 ==="

# 清理之前的测试数据
echo "drop table warehouse;" | ./rmdb_client > /dev/null 2>&1

# 创建表
echo "create table warehouse (w_id int, name char(8));" | ./rmdb_client

# 插入测试数据
echo "insert into warehouse values(1, 'qweruiop');" | ./rmdb_client
echo "insert into warehouse values(2, 'asdfhjkl');" | ./rmdb_client
echo "insert into warehouse values(3, 'qwerghjk');" | ./rmdb_client
echo "insert into warehouse values(4, 'bgtyhnmj');" | ./rmdb_client

# 显示所有数据
echo "=== 插入的数据 ==="
echo "select * from warehouse;" | ./rmdb_client

# 创建字符串索引
echo ""
echo "=== 创建字符串索引 ==="
echo "create index warehouse(name);" | ./rmdb_client

# 测试字符串索引查询
echo ""
echo "=== 测试字符串索引查询 ==="
echo "查询 name = 'qweruiop':"
echo "select * from warehouse where name = 'qweruiop';" | ./rmdb_client

echo ""
echo "查询 name = 'asdfhjkl':"
echo "select * from warehouse where name = 'asdfhjkl';" | ./rmdb_client

echo ""
echo "查询 name = 'qwerghjk':"
echo "select * from warehouse where name = 'qwerghjk';" | ./rmdb_client

echo ""
echo "查询 name = 'bgtyhnmj':"
echo "select * from warehouse where name = 'bgtyhnmj';" | ./rmdb_client

echo ""
echo "=== 测试完成 ===" 