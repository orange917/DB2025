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
drop table records;
drop table empty_table;
drop table single_row;
drop table large_data;
drop table precision_test;
drop table char_test;
drop table employee;
drop table orders;
drop table products;

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

# -----------------------------------------------------------------
# 测试20: 综合测试
# -----------------------------------------------------------------
echo "=== 测试20: 综合测试 ===";
select course, COUNT(*) as student_count, AVG(score) as avg_score, MAX(score) as max_score, MIN(score) as min_score, SUM(score) as total_score from grade group by course having COUNT(*) >= 2 order by avg_score desc;
select region, product, COUNT(*) as sales_count, SUM(amount) as total_amount, AVG(amount) as avg_amount from sales group by region, product having SUM(amount) > 1000 order by total_amount desc limit 5;
select dept, COUNT(*) as emp_count, AVG(salary) as avg_salary, SUM(salary) as total_salary from employee group by dept having COUNT(*) > 1 order by avg_salary desc;

EOF

# 运行聚合函数测试
echo "开始聚合函数测试..."
./rmdb_client < aggregation_test.sql

# 清理临时文件
rm -f aggregation_test.sql 