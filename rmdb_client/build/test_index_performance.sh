#!/bin/bash

# 测试索引性能的脚本
# 对比有无索引的查询时间

echo "=== 索引性能测试脚本 ==="
echo "测试表: warehouse (w_id int, name char(8))"
echo "测试数据: 3000条记录"
echo "测试查询: 3000次等值查询"
echo ""

# 清理之前的测试数据
echo "清理之前的测试数据..."
echo "drop table warehouse;" | ./rmdb_client > /dev/null 2>&1

# 创建表
echo "创建表 warehouse..."
echo "create table warehouse (w_id int, name char(8));" | ./rmdb_client

# 生成插入数据的SQL
echo "生成测试数据..."
for i in {1..3000}; do
    # 生成随机的8位字符串
    name=$(printf "%08d" $i)
    echo "insert into warehouse values($i, '$name');"
done > insert_data.sql

# 插入数据
echo "插入3000条测试数据..."
cat insert_data.sql | ./rmdb_client > /dev/null 2>&1

# 生成查询SQL（无索引）
echo "生成无索引查询SQL..."
for i in {1..3000}; do
    echo "select * from warehouse where w_id = $i;"
done > queries_no_index.sql

# 测试无索引查询时间
echo "=== 开始无索引查询测试 ==="
echo "执行3000次等值查询（无索引）..."
start_time=$(date +%s)
cat queries_no_index.sql | ./rmdb_client > /dev/null 2>&1
end_time=$(date +%s)

no_index_time=$((end_time - start_time))
echo "无索引查询总时间: ${no_index_time} 秒"

# 创建索引
echo ""
echo "=== 创建索引 ==="
echo "create index warehouse(w_id);" | ./rmdb_client

# 测试有索引查询时间
echo ""
echo "=== 开始有索引查询测试 ==="
echo "执行3000次等值查询（有索引）..."
start_time=$(date +%s)
cat queries_no_index.sql | ./rmdb_client > /dev/null 2>&1
end_time=$(date +%s)

with_index_time=$((end_time - start_time))
echo "有索引查询总时间: ${with_index_time} 秒"

# 计算性能提升
echo ""
echo "=== 性能对比结果 ==="
echo "无索引查询时间: ${no_index_time} 秒"
echo "有索引查询时间: ${with_index_time} 秒"

if [ $with_index_time -gt 0 ]; then
    speedup=$((no_index_time / with_index_time))
    improvement=$(((no_index_time - with_index_time) * 100 / no_index_time))
    echo "性能提升倍数: ${speedup}x"
    echo "性能提升百分比: ${improvement}%"
    
    # 检查是否达到70%的要求
    if [ $with_index_time -lt $((no_index_time * 7 / 10)) ]; then
        echo "✅ 索引性能测试通过！有索引查询时间少于无索引的70%"
    else
        echo "❌ 索引性能测试未通过！有索引查询时间超过无索引的70%"
    fi
else
    echo "❌ 有索引查询时间为0，可能存在错误"
fi

# 清理临时文件
rm -f insert_data.sql queries_no_index.sql

echo ""
echo "=== 测试完成 ===" 