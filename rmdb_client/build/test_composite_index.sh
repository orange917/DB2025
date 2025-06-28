#!/bin/bash
# 复合索引性能测试脚本

# 编译
cmake .. -DCMAKE_BUILD_TYPE=Release
make rmdb_client -j8

# 创建复合索引测试脚本
cat > composite_index_test.sql << 'EOF'
# -----------------------------------------------------------------
# 复合索引性能测试
# -----------------------------------------------------------------

# 清理环境
drop table composite_test;

# 创建测试表
create table composite_test (id int, name char(10), value float, category char(5));

# 插入测试数据 (1000条记录)
insert into composite_test values (1, 'test001', 10.1, 'cat1');
insert into composite_test values (2, 'test002', 20.2, 'cat1');
insert into composite_test values (3, 'test003', 30.3, 'cat2');
insert into composite_test values (4, 'test004', 40.4, 'cat2');
insert into composite_test values (5, 'test005', 50.5, 'cat1');
insert into composite_test values (6, 'test006', 60.6, 'cat3');
insert into composite_test values (7, 'test007', 70.7, 'cat3');
insert into composite_test values (8, 'test008', 80.8, 'cat1');
insert into composite_test values (9, 'test009', 90.9, 'cat2');
insert into composite_test values (10, 'test010', 100.0, 'cat3');

# 继续插入更多数据...
insert into composite_test values (11, 'test011', 11.1, 'cat1');
insert into composite_test values (12, 'test012', 22.2, 'cat2');
insert into composite_test values (13, 'test013', 33.3, 'cat3');
insert into composite_test values (14, 'test014', 44.4, 'cat1');
insert into composite_test values (15, 'test015', 55.5, 'cat2');
insert into composite_test values (16, 'test016', 66.6, 'cat3');
insert into composite_test values (17, 'test017', 77.7, 'cat1');
insert into composite_test values (18, 'test018', 88.8, 'cat2');
insert into composite_test values (19, 'test019', 99.9, 'cat3');
insert into composite_test values (20, 'test020', 110.0, 'cat1');

# 批量插入更多数据
insert into composite_test values (21, 'test021', 21.1, 'cat2');
insert into composite_test values (22, 'test022', 32.2, 'cat3');
insert into composite_test values (23, 'test023', 43.3, 'cat1');
insert into composite_test values (24, 'test024', 54.4, 'cat2');
insert into composite_test values (25, 'test025', 65.5, 'cat3');
insert into composite_test values (26, 'test026', 76.6, 'cat1');
insert into composite_test values (27, 'test027', 87.7, 'cat2');
insert into composite_test values (28, 'test028', 98.8, 'cat3');
insert into composite_test values (29, 'test029', 109.9, 'cat1');
insert into composite_test values (30, 'test030', 120.0, 'cat2');

# 继续插入到100条...
insert into composite_test values (31, 'test031', 31.1, 'cat3');
insert into composite_test values (32, 'test032', 42.2, 'cat1');
insert into composite_test values (33, 'test033', 53.3, 'cat2');
insert into composite_test values (34, 'test034', 64.4, 'cat3');
insert into composite_test values (35, 'test035', 75.5, 'cat1');
insert into composite_test values (36, 'test036', 86.6, 'cat2');
insert into composite_test values (37, 'test037', 97.7, 'cat3');
insert into composite_test values (38, 'test038', 108.8, 'cat1');
insert into composite_test values (39, 'test039', 119.9, 'cat2');
insert into composite_test values (40, 'test040', 130.0, 'cat3');

# 继续插入到100条...
insert into composite_test values (41, 'test041', 41.1, 'cat1');
insert into composite_test values (42, 'test042', 52.2, 'cat2');
insert into composite_test values (43, 'test043', 63.3, 'cat3');
insert into composite_test values (44, 'test044', 74.4, 'cat1');
insert into composite_test values (45, 'test045', 85.5, 'cat2');
insert into composite_test values (46, 'test046', 96.6, 'cat3');
insert into composite_test values (47, 'test047', 107.7, 'cat1');
insert into composite_test values (48, 'test048', 118.8, 'cat2');
insert into composite_test values (49, 'test049', 129.9, 'cat1');
insert into composite_test values (50, 'test050', 140.0, 'cat2');

# 继续插入到100条...
insert into composite_test values (51, 'test051', 51.1, 'cat3');
insert into composite_test values (52, 'test052', 62.2, 'cat1');
insert into composite_test values (53, 'test053', 73.3, 'cat2');
insert into composite_test values (54, 'test054', 84.4, 'cat3');
insert into composite_test values (55, 'test055', 95.5, 'cat1');
insert into composite_test values (56, 'test056', 106.6, 'cat2');
insert into composite_test values (57, 'test057', 117.7, 'cat3');
insert into composite_test values (58, 'test058', 128.8, 'cat1');
insert into composite_test values (59, 'test059', 139.9, 'cat2');
insert into composite_test values (60, 'test060', 150.0, 'cat3');

# 继续插入到100条...
insert into composite_test values (61, 'test061', 61.1, 'cat1');
insert into composite_test values (62, 'test062', 72.2, 'cat2');
insert into composite_test values (63, 'test063', 83.3, 'cat3');
insert into composite_test values (64, 'test064', 94.4, 'cat1');
insert into composite_test values (65, 'test065', 105.5, 'cat2');
insert into composite_test values (66, 'test066', 116.6, 'cat3');
insert into composite_test values (67, 'test067', 127.7, 'cat1');
insert into composite_test values (68, 'test068', 138.8, 'cat2');
insert into composite_test values (69, 'test069', 149.9, 'cat3');
insert into composite_test values (70, 'test070', 160.0, 'cat1');

# 继续插入到100条...
insert into composite_test values (71, 'test071', 71.1, 'cat2');
insert into composite_test values (72, 'test072', 82.2, 'cat3');
insert into composite_test values (73, 'test073', 93.3, 'cat1');
insert into composite_test values (74, 'test074', 104.4, 'cat2');
insert into composite_test values (75, 'test075', 115.5, 'cat3');
insert into composite_test values (76, 'test076', 126.6, 'cat1');
insert into composite_test values (77, 'test077', 137.7, 'cat2');
insert into composite_test values (78, 'test078', 148.8, 'cat3');
insert into composite_test values (79, 'test079', 159.9, 'cat1');
insert into composite_test values (80, 'test080', 170.0, 'cat2');

# 继续插入到100条...
insert into composite_test values (81, 'test081', 81.1, 'cat3');
insert into composite_test values (82, 'test082', 92.2, 'cat1');
insert into composite_test values (83, 'test083', 103.3, 'cat2');
insert into composite_test values (84, 'test084', 114.4, 'cat3');
insert into composite_test values (85, 'test085', 125.5, 'cat1');
insert into composite_test values (86, 'test086', 136.6, 'cat2');
insert into composite_test values (87, 'test087', 147.7, 'cat3');
insert into composite_test values (88, 'test088', 158.8, 'cat1');
insert into composite_test values (89, 'test089', 169.9, 'cat2');
insert into composite_test values (90, 'test090', 180.0, 'cat3');

# 继续插入到100条...
insert into composite_test values (91, 'test091', 91.1, 'cat1');
insert into composite_test values (92, 'test092', 102.2, 'cat2');
insert into composite_test values (93, 'test093', 113.3, 'cat3');
insert into composite_test values (94, 'test094', 124.4, 'cat1');
insert into composite_test values (95, 'test095', 135.5, 'cat2');
insert into composite_test values (96, 'test096', 146.6, 'cat3');
insert into composite_test values (97, 'test097', 157.7, 'cat1');
insert into composite_test values (98, 'test098', 168.8, 'cat2');
insert into composite_test values (99, 'test099', 179.9, 'cat3');
insert into composite_test values (100, 'test100', 190.0, 'cat1');

# -----------------------------------------------------------------
# 测试1: 无索引查询基准
# -----------------------------------------------------------------
echo "=== 无索引查询基准测试 ===";
select * from composite_test where id = 50;
select * from composite_test where id > 20 and id < 80;
select * from composite_test where name = 'test050';
select * from composite_test where name > 'test020' and name < 'test080';
select * from composite_test where id = 50 and name = 'test050';
select * from composite_test where id > 20 and name > 'test020';
select * from composite_test where id > 20 and id < 80 and name > 'test020' and name < 'test080';

# -----------------------------------------------------------------
# 测试2: 单列索引测试
# -----------------------------------------------------------------
echo "=== 单列索引测试 ===";
create index composite_test(id);
select * from composite_test where id = 50;
select * from composite_test where id > 20 and id < 80;
drop index composite_test(id);

create index composite_test(name);
select * from composite_test where name = 'test050';
select * from composite_test where name > 'test020' and name < 'test080';
drop index composite_test(name);

# -----------------------------------------------------------------
# 测试3: 复合索引测试
# -----------------------------------------------------------------
echo "=== 复合索引测试 ===";
create index composite_test(id,name);
select * from composite_test where id = 50 and name = 'test050';
select * from composite_test where id > 20 and name > 'test020';
select * from composite_test where id > 20 and id < 80 and name > 'test020' and name < 'test080';
drop index composite_test(id,name);

# -----------------------------------------------------------------
# 测试4: 三列复合索引测试
# -----------------------------------------------------------------
echo "=== 三列复合索引测试 ===";
create index composite_test(id,name,value);
select * from composite_test where id = 50 and name = 'test050' and value = 140.0;
select * from composite_test where id > 20 and name > 'test020' and value > 50.0;
select * from composite_test where id = 50 and name = 'test050';
select * from composite_test where id > 20 and name > 'test020';
drop index composite_test(id,name,value);

# -----------------------------------------------------------------
# 测试5: 字符串复合索引测试
# -----------------------------------------------------------------
echo "=== 字符串复合索引测试 ===";
create index composite_test(name,category);
select * from composite_test where name = 'test050' and category = 'cat2';
select * from composite_test where name > 'test020' and category = 'cat1';
select * from composite_test where name > 'test020' and name < 'test080' and category = 'cat2';
drop index composite_test(name,category);

# -----------------------------------------------------------------
# 最后清理
# -----------------------------------------------------------------
drop table composite_test;
EOF

# 运行复合索引测试
echo "开始复合索引性能测试..."
echo "注意观察："
echo "1. 复合索引查询应该比无索引查询快"
echo "2. 复合索引应该能有效支持最左前缀匹配"
echo "3. 等值条件应该比范围条件更有效"
echo ""

./rmdb_client < composite_index_test.sql

# 清理临时文件
rm -f composite_index_test.sql

echo ""
echo "=== 复合索引测试完成 ==="
echo "如果复合索引实现正确，你应该看到："
echo "1. 复合索引查询明显快于无索引查询"
echo "2. 最左前缀匹配正常工作"
echo "3. 多列条件查询性能良好" 