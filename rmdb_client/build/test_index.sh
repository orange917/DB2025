#!/bin/bash
# Rucbase 索引性能测试脚本

# 编译
cmake .. -DCMAKE_BUILD_TYPE=Release
make rmdb_client -j8

# 创建性能测试脚本
cat > performance_test.sql << 'EOF'
# -----------------------------------------------------------------
# 设置: 清理环境并创建测试表
# -----------------------------------------------------------------
drop table warehouse;
create table warehouse (w_id int, name char(8));

# 插入大量测试数据
insert into warehouse values (10, 'qweruiop');
insert into warehouse values (534, 'asdfhjkl');
insert into warehouse values (100, 'qwerghjk');
insert into warehouse values (500, 'bgtyhnmj');
insert into warehouse values (200, 'zxcvbnml');
insert into warehouse values (300, 'poiuytre');
insert into warehouse values (400, 'mnbvcxzl');
insert into warehouse values (600, 'lkjhgfds');
insert into warehouse values (700, 'qazwsxed');
insert into warehouse values (800, 'rfvtgbyh');
insert into warehouse values (900, 'ujmikolp');
insert into warehouse values (150, 'asdfqwer');
insert into warehouse values (250, 'zxcvbnml');
insert into warehouse values (350, 'poiuytre');
insert into warehouse values (450, 'mnbvcxzl');
insert into warehouse values (550, 'lkjhgfds');
insert into warehouse values (650, 'qazwsxed');
insert into warehouse values (750, 'rfvtgbyh');
insert into warehouse values (850, 'ujmikolp');
insert into warehouse values (950, 'asdfqwer');

# -----------------------------------------------------------------
# 测试1: 无索引查询性能基准
# -----------------------------------------------------------------
select * from warehouse where w_id = 10;
select * from warehouse where w_id < 534 and w_id > 100;
select * from warehouse where name = 'qweruiop';
select * from warehouse where name > 'qwerghjk';
select * from warehouse where name > 'aszdefgh' and name < 'qweraaaa';

# -----------------------------------------------------------------
# 测试2: 单列索引性能测试
# -----------------------------------------------------------------
create index warehouse(w_id);
select * from warehouse where w_id = 10;
select * from warehouse where w_id < 534 and w_id > 100;
drop index warehouse(w_id);

create index warehouse(name);
select * from warehouse where name = 'qweruiop';
select * from warehouse where name > 'qwerghjk';
select * from warehouse where name > 'aszdefgh' and name < 'qweraaaa';
drop index warehouse(name);

# -----------------------------------------------------------------
# 测试3: 复合索引性能测试
# -----------------------------------------------------------------
create index warehouse(w_id,name);
select * from warehouse where w_id = 100 and name = 'qwerghjk';
select * from warehouse where w_id < 600 and name > 'bztyhnmj';
drop index warehouse(w_id,name);

# -----------------------------------------------------------------
# 最后清理
# -----------------------------------------------------------------
drop table warehouse;
EOF

# 运行性能测试
echo "开始索引性能测试..."
./rmdb_client < performance_test.sql

# 清理临时文件
rm -f performance_test.sql