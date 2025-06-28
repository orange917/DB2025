#!/bin/bash
# Rucbase 索引性能基准测试脚本

# 编译
cmake .. -DCMAKE_BUILD_TYPE=Release
make rmdb_client -j8

echo "=========================================="
echo "Rucbase 索引性能基准测试"
echo "=========================================="

# 创建基准测试脚本
cat > benchmark_test.sql << 'EOF'
# 清理环境
drop table if exists benchmark_table;

# 创建测试表
create table benchmark_table (id int, name char(20), value float);

# 插入大量测试数据 (100条记录)
insert into benchmark_table values (1, 'test001', 1.1);
insert into benchmark_table values (2, 'test002', 2.2);
insert into benchmark_table values (3, 'test003', 3.3);
insert into benchmark_table values (4, 'test004', 4.4);
insert into benchmark_table values (5, 'test005', 5.5);
insert into benchmark_table values (6, 'test006', 6.6);
insert into benchmark_table values (7, 'test007', 7.7);
insert into benchmark_table values (8, 'test008', 8.8);
insert into benchmark_table values (9, 'test009', 9.9);
insert into benchmark_table values (10, 'test010', 10.0);
insert into benchmark_table values (11, 'test011', 11.1);
insert into benchmark_table values (12, 'test012', 12.2);
insert into benchmark_table values (13, 'test013', 13.3);
insert into benchmark_table values (14, 'test014', 14.4);
insert into benchmark_table values (15, 'test015', 15.5);
insert into benchmark_table values (16, 'test016', 16.6);
insert into benchmark_table values (17, 'test017', 17.7);
insert into benchmark_table values (18, 'test018', 18.8);
insert into benchmark_table values (19, 'test019', 19.9);
insert into benchmark_table values (20, 'test020', 20.0);
insert into benchmark_table values (21, 'test021', 21.1);
insert into benchmark_table values (22, 'test022', 22.2);
insert into benchmark_table values (23, 'test023', 23.3);
insert into benchmark_table values (24, 'test024', 24.4);
insert into benchmark_table values (25, 'test025', 25.5);
insert into benchmark_table values (26, 'test026', 26.6);
insert into benchmark_table values (27, 'test027', 27.7);
insert into benchmark_table values (28, 'test028', 28.8);
insert into benchmark_table values (29, 'test029', 29.9);
insert into benchmark_table values (30, 'test030', 30.0);
insert into benchmark_table values (31, 'test031', 31.1);
insert into benchmark_table values (32, 'test032', 32.2);
insert into benchmark_table values (33, 'test033', 33.3);
insert into benchmark_table values (34, 'test034', 34.4);
insert into benchmark_table values (35, 'test035', 35.5);
insert into benchmark_table values (36, 'test036', 36.6);
insert into benchmark_table values (37, 'test037', 37.7);
insert into benchmark_table values (38, 'test038', 38.8);
insert into benchmark_table values (39, 'test039', 39.9);
insert into benchmark_table values (40, 'test040', 40.0);
insert into benchmark_table values (41, 'test041', 41.1);
insert into benchmark_table values (42, 'test042', 42.2);
insert into benchmark_table values (43, 'test043', 43.3);
insert into benchmark_table values (44, 'test044', 44.4);
insert into benchmark_table values (45, 'test045', 45.5);
insert into benchmark_table values (46, 'test046', 46.6);
insert into benchmark_table values (47, 'test047', 47.7);
insert into benchmark_table values (48, 'test048', 48.8);
insert into benchmark_table values (49, 'test049', 49.9);
insert into benchmark_table values (50, 'test050', 50.0);
insert into benchmark_table values (51, 'test051', 51.1);
insert into benchmark_table values (52, 'test052', 52.2);
insert into benchmark_table values (53, 'test053', 53.3);
insert into benchmark_table values (54, 'test054', 54.4);
insert into benchmark_table values (55, 'test055', 55.5);
insert into benchmark_table values (56, 'test056', 56.6);
insert into benchmark_table values (57, 'test057', 57.7);
insert into benchmark_table values (58, 'test058', 58.8);
insert into benchmark_table values (59, 'test059', 59.9);
insert into benchmark_table values (60, 'test060', 60.0);
insert into benchmark_table values (61, 'test061', 61.1);
insert into benchmark_table values (62, 'test062', 62.2);
insert into benchmark_table values (63, 'test063', 63.3);
insert into benchmark_table values (64, 'test064', 64.4);
insert into benchmark_table values (65, 'test065', 65.5);
insert into benchmark_table values (66, 'test066', 66.6);
insert into benchmark_table values (67, 'test067', 67.7);
insert into benchmark_table values (68, 'test068', 68.8);
insert into benchmark_table values (69, 'test069', 69.9);
insert into benchmark_table values (70, 'test070', 70.0);
insert into benchmark_table values (71, 'test071', 71.1);
insert into benchmark_table values (72, 'test072', 72.2);
insert into benchmark_table values (73, 'test073', 73.3);
insert into benchmark_table values (74, 'test074', 74.4);
insert into benchmark_table values (75, 'test075', 75.5);
insert into benchmark_table values (76, 'test076', 76.6);
insert into benchmark_table values (77, 'test077', 77.7);
insert into benchmark_table values (78, 'test078', 78.8);
insert into benchmark_table values (79, 'test079', 79.9);
insert into benchmark_table values (80, 'test080', 80.0);
insert into benchmark_table values (81, 'test081', 81.1);
insert into benchmark_table values (82, 'test082', 82.2);
insert into benchmark_table values (83, 'test083', 83.3);
insert into benchmark_table values (84, 'test084', 84.4);
insert into benchmark_table values (85, 'test085', 85.5);
insert into benchmark_table values (86, 'test086', 86.6);
insert into benchmark_table values (87, 'test087', 87.7);
insert into benchmark_table values (88, 'test088', 88.8);
insert into benchmark_table values (89, 'test089', 89.9);
insert into benchmark_table values (90, 'test090', 90.0);
insert into benchmark_table values (91, 'test091', 91.1);
insert into benchmark_table values (92, 'test092', 92.2);
insert into benchmark_table values (93, 'test093', 93.3);
insert into benchmark_table values (94, 'test094', 94.4);
insert into benchmark_table values (95, 'test095', 95.5);
insert into benchmark_table values (96, 'test096', 96.6);
insert into benchmark_table values (97, 'test097', 97.7);
insert into benchmark_table values (98, 'test098', 98.8);
insert into benchmark_table values (99, 'test099', 99.9);
insert into benchmark_table values (100, 'test100', 100.0);

# 测试1: 无索引查询 (基准测试)
echo "=== 无索引查询基准测试 ===";
select * from benchmark_table where id = 50;
select * from benchmark_table where id > 20 and id < 80;
select * from benchmark_table where name = 'test050';
select * from benchmark_table where name > 'test020' and name < 'test080';
select * from benchmark_table where value = 50.0;
select * from benchmark_table where value > 20.0 and value < 80.0;

# 测试2: 有索引查询 (性能测试)
echo "=== 有索引查询性能测试 ===";
create index benchmark_table(id);
select * from benchmark_table where id = 50;
select * from benchmark_table where id > 20 and id < 80;
drop index benchmark_table(id);

create index benchmark_table(name);
select * from benchmark_table where name = 'test050';
select * from benchmark_table where name > 'test020' and name < 'test080';
drop index benchmark_table(name);

create index benchmark_table(value);
select * from benchmark_table where value = 50.0;
select * from benchmark_table where value > 20.0 and value < 80.0;
drop index benchmark_table(value);

# 测试3: 复合索引测试
echo "=== 复合索引性能测试 ===";
create index benchmark_table(id,name);
select * from benchmark_table where id = 50 and name = 'test050';
select * from benchmark_table where id > 20 and name > 'test020';
drop index benchmark_table(id,name);

# 清理
drop table benchmark_table;
EOF

echo "开始运行基准测试..."
echo "注意：请观察查询执行时间，有索引的查询应该明显快于无索引查询"
echo ""

# 运行基准测试并测量总时间
echo "=== 测量总执行时间 ==="
time ./rmdb_client < benchmark_test.sql

# 清理临时文件
rm -f benchmark_test.sql

echo ""
echo "=========================================="
echo "基准测试完成"
echo "=========================================="
echo "性能要求：有索引查询时间应小于无索引查询时间的70%"
echo "如果看到明显的性能提升，说明索引实现正确" 