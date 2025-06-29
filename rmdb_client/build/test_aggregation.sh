#!/bin/bash
# Rucbase 聚合函数测试脚本

# 编译
cmake .. -DCMAKE_BUILD_TYPE=Release
make rmdb_client -j8

# 创建聚合函数测试脚本
cat > aggregation_test.sql << 'EOF'
# -----------------------------------------------------------------
# 聚合函数测试脚本
# -----------------------------------------------------------------

# 清理环境
drop table grade;
drop table sales;
drop table test_precision;

# -----------------------------------------------------------------
# 测试1: 基础聚合函数测试 (参考示例)
# -----------------------------------------------------------------
echo "=== 测试1: 基础聚合函数测试 ===";
create table grade (course char(20), id int, score float);
insert into grade values('DataStructure', 1, 95);
insert into grade values('DataStructure', 2, 93.5);
insert into grade values('DataStructure', 4, 87);
insert into grade values('DataStructure', 3, 85);
insert into grade values('DB', 1, 94);
insert into grade values('DB', 2, 74.5);
insert into grade values('DB', 4, 83);
insert into grade values('DB', 3, 87);

select MAX(id) as max_id from grade;
select MIN(score) as min_score from grade where course = 'DB';
select AVG(score) as avg_score from grade where course = 'DataStructure';
select COUNT(course) as course_num from grade;
select COUNT(*) as row_num from grade;
select COUNT(*) as row_num from grade where score < 60;
select SUM(score) as sum_score from grade where id = 1;

# -----------------------------------------------------------------
# 测试2: 边界值和精度测试
# -----------------------------------------------------------------
echo "=== 测试2: 边界值和精度测试 ===";
create table test_precision (id int, value_float float, value_int int, name char(10));

# 插入边界值
insert into test_precision values(1, 0.0, 0, 'zero');
insert into test_precision values(2, -999.999, -999, 'negative');
insert into test_precision values(3, 999.999, 999, 'positive');
insert into test_precision values(4, 0.001, 1, 'small');
insert into test_precision values(5, 0.999, 999, 'large');
insert into test_precision values(7, 3.14159, 314, 'pi');
insert into test_precision values(8, 2.71828, 271, 'e');

# 测试COUNT函数
select COUNT(*) as total_count from test_precision;
select COUNT(id) as id_count from test_precision;
select COUNT(value_float) as float_count from test_precision;
select COUNT(value_int) as int_count from test_precision;

# 测试精度
select SUM(value_float) as sum_float from test_precision;
select AVG(value_float) as avg_float from test_precision;
select MAX(value_float) as max_float from test_precision;
select MIN(value_float) as min_float from test_precision;

# -----------------------------------------------------------------
# 测试3: 多聚合函数组合测试
# -----------------------------------------------------------------
echo "=== 测试3: 多聚合函数组合测试 ===";
create table sales (region char(20), product char(20), amount float, quantity int);

insert into sales values('North', 'Laptop', 1200.50, 5);
insert into sales values('North', 'Phone', 800.25, 10);
insert into sales values('North', 'Tablet', 600.75, 8);
insert into sales values('South', 'Laptop', 1100.00, 3);
insert into sales values('South', 'Phone', 750.50, 12);
insert into sales values('South', 'Tablet', 550.25, 6);
insert into sales values('East', 'Laptop', 1300.75, 7);
insert into sales values('East', 'Phone', 850.00, 15);
insert into sales values('West', 'Laptop', 1150.25, 4);
insert into sales values('West', 'Phone', 780.75, 9);

# 测试多个聚合函数同时使用
select COUNT(*) as total_sales, SUM(amount) as total_amount, AVG(amount) as avg_amount from sales;
select MAX(quantity) as max_qty, MIN(quantity) as min_qty, AVG(quantity) as avg_qty from sales;
select COUNT(*) as laptop_count, SUM(amount) as laptop_total from sales where product = 'Laptop';

# -----------------------------------------------------------------
# 测试4: 空表测试
# -----------------------------------------------------------------
echo "=== 测试4: 空表测试 ===";
create table empty_table (id int, value float);
select COUNT(*) from empty_table;
select SUM(value) from empty_table;
select AVG(value) from empty_table;
select MAX(value) from empty_table;
select MIN(value) from empty_table;

# -----------------------------------------------------------------
# 测试5: 单行表测试
# -----------------------------------------------------------------
echo "=== 测试5: 单行表测试 ===";
create table single_row (id int, value float, name char(10));
insert into single_row values(1, 100.5, 'single');
select COUNT(*), SUM(value), AVG(value), MAX(value), MIN(value) from single_row;

# -----------------------------------------------------------------
# 测试6: 大数据量测试
# -----------------------------------------------------------------
echo "=== 测试6: 大数据量测试 ===";
create table large_data (category char(10), value int, score float);

# 插入大量测试数据
insert into large_data values('A', 1, 10.1);
insert into large_data values('A', 2, 20.2);
insert into large_data values('A', 3, 30.3);
insert into large_data values('B', 4, 40.4);
insert into large_data values('B', 5, 50.5);
insert into large_data values('B', 6, 60.6);
insert into large_data values('C', 7, 70.7);
insert into large_data values('C', 8, 80.8);
insert into large_data values('C', 9, 90.9);
insert into large_data values('D', 10, 100.0);
insert into large_data values('D', 11, 110.1);
insert into large_data values('D', 12, 120.2);

# 测试大数据量下的聚合函数
select COUNT(*) as total_count, SUM(value) as total_value, AVG(score) as avg_score from large_data;
select MAX(score) as max_score, MIN(score) as min_score from large_data;
select COUNT(*) as category_a_count, AVG(score) as category_a_avg from large_data where category = 'A';

# -----------------------------------------------------------------
# 测试7: 精度边界测试
# -----------------------------------------------------------------
echo "=== 测试7: 精度边界测试 ===";
drop table precision_test;
create table precision_test (id int, tiny_float float, large_float float);

insert into precision_test values(1, 0.000001, 999999.999999);
insert into precision_test values(2, 0.000002, 999999.999998);
insert into precision_test values(3, 0.000003, 999999.999997);

select SUM(tiny_float) as sum_tiny, AVG(tiny_float) as avg_tiny from precision_test;
select SUM(large_float) as sum_large, AVG(large_float) as avg_large from precision_test;

# -----------------------------------------------------------------
# 测试8: 字符类型聚合测试
# -----------------------------------------------------------------
echo "=== 测试8: 字符类型聚合测试 ===";
create table char_test (id int, name char(20), category char(10));

insert into char_test values(1, 'Alice', 'A');
insert into char_test values(2, 'Bob', 'A');
insert into char_test values(3, 'Charlie', 'B');
insert into char_test values(4, 'David', 'B');
insert into char_test values(5, 'Eve', 'C');

select COUNT(name) as name_count from char_test;
select COUNT(*) as total_count from char_test where category = 'A';

# -----------------------------------------------------------------
# 测试9: 复杂WHERE条件测试
# -----------------------------------------------------------------
echo "=== 测试9: 复杂WHERE条件测试 ===";
select COUNT(*) as high_score_count from grade where score >= 90;
select AVG(score) as high_score_avg from grade where score >= 90;
select COUNT(*) as low_score_count from grade where score < 80;
select SUM(score) as low_score_sum from grade where score < 80;

# -----------------------------------------------------------------
# 最后清理
# -----------------------------------------------------------------
drop table grade;
drop table test_precision;
drop table sales;
drop table empty_table;
drop table single_row;
drop table large_data;
drop table precision_test;
drop table char_test;

# 测试点2
create table grade (course char(20),id int,score float);
insert into grade values('DataStructure',1,95);
insert into grade values('DataStructure',2,93.5);
insert into grade values('DataStructure',3,94.5);
insert into grade values('ComputerNetworks',1,99);
insert into grade values('ComputerNetworks',2,88.5);
insert into grade values('ComputerNetworks',3,92.5);
insert into grade values('C++',1,92);
insert into grade values('C++',2,89);
insert into grade values('C++',3,89.5);
select id,MAX(score) as max_score,MIN(score) as min_score,SUM(score) as sum_score from grade group by id;
select id,MAX(score) as max_score from grade group by id having COUNT(*) >  3;
insert into grade values ('ParallelCompute',1,100);
select id,MAX(score) as max_score from grade group by id having COUNT(*) >  3;
select id,MAX(score) as max_score,MIN(score) as min_score from grade group by id having COUNT(*) > 1 and MIN(score) > 88;
select course ,COUNT(*) as row_num , COUNT(id) as student_num , MAX(score) as top_score, MIN(score) as lowest_score from grade group by course;
select course, id, score from grade order by score desc;
drop table grade;

# 测试点3
create table grade (course char(20),id int,score float);
insert into grade values('DataStructure',1,95);
insert into grade values('DataStructure',2,93.5);
insert into grade values('DataStructure',3,94.5);
insert into grade values('ComputerNetworks',1,99);
insert into grade values('ComputerNetworks',2,88.5);
insert into grade values('ComputerNetworks',3,92.5);
-- SELECT 列表中不能出现没有在 GROUP BY 子句中的非聚集列
select id , score from grade group by course;
-- WHERE 子句中不能用聚集函数作为条件表达式
select id, MAX(score) as max_score from grade where MAX(score) > 90 group by id;
EOF

# 运行聚合函数测试
echo "开始聚合函数测试..."
./rmdb_client < aggregation_test.sql

# 清理临时文件
rm -f aggregation_test.sql 