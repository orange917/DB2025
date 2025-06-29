#!/bin/bash
# 浮点数精度调试测试脚本

# 编译
cmake .. -DCMAKE_BUILD_TYPE=Release
make rmdb_client -j8

# 创建精度调试测试脚本
cat > precision_debug_test.sql << 'EOF'
# -----------------------------------------------------------------
# 浮点数精度调试测试
# -----------------------------------------------------------------

drop table precision_debug;

create table precision_debug (id int, value float);

# 插入测试数据
insert into precision_debug values(1, 999999.999999);
insert into precision_debug values(2, 999999.999998);
insert into precision_debug values(3, 999999.999997);

# 测试单个值的读取
select value from precision_debug where id = 1;
select value from precision_debug where id = 2;
select value from precision_debug where id = 3;

# 测试SUM函数
select SUM(value) as sum_value from precision_debug;

# 测试AVG函数
select AVG(value) as avg_value from precision_debug;

# 测试MAX和MIN函数
select MAX(value) as max_value, MIN(value) as min_value from precision_debug;

# 测试手动计算
select value as val1 from precision_debug where id = 1;
select value as val2 from precision_debug where id = 2;
select value as val3 from precision_debug where id = 3;

drop table precision_debug;
EOF

# 运行精度调试测试
echo "开始浮点数精度调试测试..."
./rmdb_client < precision_debug_test.sql

# 清理临时文件
rm -f precision_debug_test.sql 