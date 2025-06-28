#!/bin/bash
# Rucbase 复合索引性能测试脚本
# 用于比较有索引和无索引时的查询性能差异

echo "开始构建项目..."
cmake .. -DCMAKE_BUILD_TYPE=Release
make rmdb_client -j8

echo "开始性能测试..."

# 继续构建SQL脚本，添加索引创建和有索引测试部分
cat >> temp_index.sql << 'EOF'

# -----------------------------------------------------------------
# 创建复合索引
# -----------------------------------------------------------------
echo "=== 创建复合索引 ==="

# 创建多个复合索引来测试不同场景
create index compositetest(category, value);
create index compositetest(id, value);
create index compositetest(id);


echo "复合索引创建完成"

# -----------------------------------------------------------------
# 测试用例组 2: 有索引查询性能测试
# -----------------------------------------------------------------
echo "=== 有索引查询性能测试 ==="

# 2.1 单列条件查询
echo "测试1: 单列等值查询 (category = 'cat1')"
SELECT * FROM compositetest WHERE category = 'cat1';

echo "测试2: 单列范围查询 (id > 2500)"
SELECT * FROM compositetest WHERE value > 2500;

# 2.2 复合条件查询 (AND)
echo "测试3: 双列AND查询 (id < 2500 AND value > 500)"
SELECT * FROM compositetest WHERE id < 2500 AND value > 500;

echo "测试4: 双列AND查询 (category = 'cat1' AND value > 500)"
SELECT * FROM compositetest WHERE category = 'cat1' AND value > 500;

echo "测试5: 三列AND查询 (category = 'cat1' AND value > 500 AND id > 20000)"
SELECT * FROM compositetest WHERE category = 'cat1' AND value > 500 AND id > 20000;

echo "测试6: 复合条件查询 (category = 'cat1' AND val)"
SELECT * FROM compositetest WHERE category = 'cat1' AND name > "sBRrz";


# -----------------------------------------------------------------
# 清理
# -----------------------------------------------------------------
EOF

# 记录有索引测试开始时间
echo "开始执行有索引性能测试..."
index_start_time=$(date +%s.%N)

# 执行有索引测试
./rmdb_client < temp_index.sql

# 记录有索引测试结束时间并计算耗时
index_end_time=$(date +%s.%N)
index_time=$(echo "$index_end_time - $index_start_time" | bc)

echo "------------------------------------------"
echo "有索引查询总执行时间: ${index_time} 秒"
echo "------------------------------------------"



echo "=========================================="
echo "性能测试完成"
echo "有索引总执行时间: ${index_time} 秒"
echo "=========================================="

# 清理临时文件
rm -f temp_index.sql

echo "测试脚本执行完成！"