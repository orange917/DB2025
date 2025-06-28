#!/bin/bash
# Rucbase 复合索引性能测试脚本
# 用于比较有索引和无索引时的查询性能差异

echo "开始构建项目..."
cmake .. -DCMAKE_BUILD_TYPE=Release
make rmdb_client -j8

echo "开始性能测试..."

# 创建临时文件来存储SQL命令
cat > temp_without_index.sql << 'EOF'
# =================================================================
# 复合索引性能测试套件
# =================================================================

# 继续添加测试命令
cat >> temp_without_index.sql << 'EOF'

# 删除索引
drop index compositetest(category, value);
drop index compositetest(id, value);
drop index compositetest(id);

# -----------------------------------------------------------------
# 测试用例组 1: 无索引查询性能测试
# -----------------------------------------------------------------
echo "=== 无索引查询性能测试 ==="

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
SELECT * FROM compositetest WHERE category = 'cat1' AND name > "sBRrz"


EOF

# 记录无索引测试开始时间
echo "开始执行无索引性能测试..."
no_index_start_time=$(date +%s.%N)

# 执行无索引测试
./rmdb_client < temp_without_index.sql

# 记录无索引测试结束时间并计算耗时
no_index_end_time=$(date +%s.%N)
no_index_time=$(echo "$no_index_end_time - $no_index_start_time" | bc)

# 清理临时文件
rm -f temp_without_index.sql

echo "------------------------------------------"
echo "无索引查询总执行时间: ${no_index_time} 秒"
echo "------------------------------------------"



echo "测试脚本执行完成！"