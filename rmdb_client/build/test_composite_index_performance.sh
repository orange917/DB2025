#!/bin/bash

# 测试多列索引性能的脚本
# 对比有无复合索引的查询时间

echo "=== 多列索引性能测试脚本 ==="
echo "测试表: warehouse (w_id int, name char(8), flo float)"
echo "测试数据: 3000条记录"
echo "测试查询: 3000次复合条件查询 (w_id AND flo)"
echo ""

# 清理之前的测试数据
echo "清理之前的测试数据..."
echo "drop table warehouse;" | ./rmdb_client > /dev/null 2>&1

# 创建表
echo "创建表 warehouse..."
echo "create table warehouse (w_id int, name char(8), flo float);" | ./rmdb_client

# 生成插入数据的SQL（小数点后一位）
echo "生成测试数据..."
for i in {1..3000}; do
    name=$(printf "%08d" $i)
    flo=$(awk -v n=$i 'BEGIN{printf "%.1f", 1024.5 - (n-1)*0.3}')
    echo "insert into warehouse values($i, '$name', $flo);"
done > insert_data.sql
# 显示前10条插入语句
echo ""
echo "前10条插入语句示例："
head -10 insert_data.sql
echo ""
# 插入数据
echo "插入3000条测试数据..."
cat insert_data.sql | ./rmdb_client > /dev/null 2>&1

# 生成查询SQL（小数点后一位，与插入数据保持一致）
echo "生成查询SQL..."
for i in {1..3000}; do
    flo=$(awk -v n=$i 'BEGIN{printf "%.1f", 1024.5 - (n-1)*0.3}')
    printf "select * from warehouse where w_id = %d and flo = %.1f;\n" $i $flo
done > queries_no_index.sql

# 测试无索引查询时间
echo "=== 开始无索引查询测试 ==="
echo "执行3000次复合条件查询（无索引）..."
echo "注意：由于查询较多，这里只显示前5个查询的结果作为示例"
echo ""

# 显示前5个查询的结果
head -5 queries_no_index.sql | while read query; do
    echo "执行查询: $query"
    echo "$query" | ./rmdb_client
    echo ""
done

echo "继续执行剩余查询（无索引）..."
start_time=$(date +%s)
cat queries_no_index.sql | ./rmdb_client > /dev/null 2>&1
end_time=$(date +%s)

no_index_time=$((end_time - start_time))
echo "无索引查询总时间: ${no_index_time} 秒"

# 创建复合索引
echo ""
echo "=== 创建复合索引 ==="
echo "create index warehouse(w_id,flo);" | ./rmdb_client

# 测试有索引查询时间
echo ""
echo "=== 开始有索引查询测试 ==="
echo "执行3000次复合条件查询（有索引）..."
echo "注意：由于查询较多，这里只显示前5个查询的结果作为示例"
echo ""

# 显示前5个查询的结果
head -5 queries_no_index.sql | while read query; do
    echo "执行查询: $query"
    echo "$query" | ./rmdb_client
    echo ""
done

echo "继续执行剩余查询（有索引）..."
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
        echo "✅ 复合索引性能测试通过！有索引查询时间少于无索引的70%"
    else
        echo "❌ 复合索引性能测试未通过！有索引查询时间超过无索引的70%"
    fi
else
    echo "❌ 有索引查询时间为0，可能存在错误"
fi

# 清理临时文件
rm -f insert_data.sql queries_no_index.sql

echo ""
echo "=== 测试完成 ===" 