#!/bin/bash
# Rucbase 精确时间测量脚本

# 编译
cmake .. -DCMAKE_BUILD_TYPE=Release
make rmdb_client -j8

echo "=========================================="
echo "Rucbase 精确时间测量测试"
echo "=========================================="

# 创建测试数据
cat > setup_data.sql << 'EOF'
drop table if exists benchmark_table;
create table benchmark_table (id int, name char(20), value float);

# 插入1000条测试数据
EOF

# 生成1000条测试数据 - 使用兼容 sh 的语法
i=1
while [ $i -le 100000 ]; do
    printf "insert into benchmark_table values (%d, 'test%03d', %d.%d);\n" $i $i $i $((i % 10)) >> setup_data.sql
    i=$((i + 1))
done

cat >> setup_data.sql << 'EOF'
EOF

# 设置测试数据
echo "设置测试数据..."
./rmdb_client < setup_data.sql

echo ""
echo "=== 无索引查询时间测量 ==="

# 测试1: 无索引 - 等值查询
echo "1. 无索引等值查询 (id = 50):"
echo "select * from benchmark_table where id = 50;" | time ./rmdb_client

# 测试2: 无索引 - 范围查询
echo "2. 无索引范围查询 (id > 20 and id < 80):"
echo "select * from benchmark_table where id > 20 and id < 80;" | time ./rmdb_client

# 测试3: 无索引 - 字符串等值查询
echo "3. 无索引字符串等值查询 (name = 'test050'):"
echo "select * from benchmark_table where name = 'test050';" | time ./rmdb_client

# 测试4: 无索引 - 字符串范围查询
echo "4. 无索引字符串范围查询 (name > 'test020' and name < 'test080'):"
echo "select * from benchmark_table where name > 'test020' and name < 'test080';" | time ./rmdb_client

echo ""
echo "=== 有索引查询时间测量 ==="

# 创建索引
echo "创建索引..."
echo "create index benchmark_table(id);" | ./rmdb_client

# 测试5: 有索引 - 等值查询
echo "5. 有索引等值查询 (id = 50):"
echo "select * from benchmark_table where id = 50;" | time ./rmdb_client

# 测试6: 有索引 - 范围查询
echo "6. 有索引范围查询 (id > 20 and id < 80):"
echo "select * from benchmark_table where id > 20 and id < 80;" | time ./rmdb_client

# 删除索引
echo "drop index benchmark_table(id);" | ./rmdb_client

# 创建字符串索引
echo "创建字符串索引..."
echo "create index benchmark_table(name);" | ./rmdb_client

# 测试7: 有索引 - 字符串等值查询
echo "7. 有索引字符串等值查询 (name = 'test050'):"
echo "select * from benchmark_table where name = 'test050';" | time ./rmdb_client

# 测试8: 有索引 - 字符串范围查询
echo "8. 有索引字符串范围查询 (name > 'test020' and name < 'test080'):"
echo "select * from benchmark_table where name > 'test020' and name < 'test080';" | time ./rmdb_client

# 删除索引
echo "drop index benchmark_table(name);" | ./rmdb_client

echo ""
echo "=== 性能分析 ==="
echo "请比较上述查询的执行时间："
echo "- 有索引查询应该明显快于无索引查询"
echo "- 性能要求：有索引时间 < 无索引时间的70%"
echo "- 等值查询的性能提升应该最明显"
echo "- 范围查询也有一定提升"
echo ""
echo "=== 复杂查询测试 ==="

# 测试9: 复杂多条件查询
echo "9. 复杂多条件查询 (无索引):"
echo "select * from benchmark_table where id > 100 and id < 900 and name > 'test050' and name < 'test950';" | time ./rmdb_client

# 创建复合索引
echo "创建复合索引..."
echo "create index benchmark_table(id,name);" | ./rmdb_client

# 测试10: 复杂多条件查询（有索引）
echo "10. 复杂多条件查询 (有索引):"
echo "select * from benchmark_table where id > 100 and id < 900 and name > 'test050' and name < 'test950';" | time ./rmdb_client

# 删除复合索引
echo "drop index benchmark_table(id,name);" | ./rmdb_client

echo ""
echo "=== 性能总结 ==="
echo "如果索引实现正确，你应该看到："
echo "1. 等值查询：有索引比无索引快很多"
echo "2. 范围查询：有索引比无索引快一些"
echo "3. 复杂查询：有索引比无索引快很多"
echo "4. 数据量越大，性能差异越明显"

# 清理
echo "清理测试数据..."
echo "drop table benchmark_table;" | ./rmdb_client
rm -f setup_data.sql

echo ""
echo "=========================================="
echo "时间测量测试完成"
echo "==========================================" 