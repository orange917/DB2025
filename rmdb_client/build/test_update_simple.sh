#!/bin/bash
# 简单的UPDATE测试脚本

# 编译
cmake .. -DCMAKE_BUILD_TYPE=Release
make rmdb_client -j8

# 创建测试脚本
cat > update_simple_test.sql << 'EOF'
# 清理环境
drop table warehouse;

# 创建测试表（不创建索引）
create table warehouse (w_id int, name char(8));

# 插入测试数据
insert into warehouse values (10, 'qweruiop');
insert into warehouse values (534, 'asdfhjkl');

# 查看初始数据
select * from warehouse;

# 测试UPDATE操作（没有索引）
update warehouse set w_id = 507 where w_id = 534;

# 查看UPDATE后的数据
select * from warehouse;
EOF

# 运行测试
echo "开始简单UPDATE测试..."
./rmdb_client < update_simple_test.sql

# 清理临时文件
rm -f update_simple_test.sql 