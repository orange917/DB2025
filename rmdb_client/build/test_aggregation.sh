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

# -----------------------------------------------------------------
# 测试15: 多表连接聚合测试
# -----------------------------------------------------------------
echo "=== 测试15: 多表连接聚合测试 ===";
create table employee (emp_id int, name char(20), dept char(20), salary float);
create table orders (order_id int, emp_id int, amount float, date char(10));
create table products (prod_id int, name char(20), price float, category char(20));

insert into employee values(1, 'Alice', 'Sales', 5000);
insert into employee values(2, 'Bob', 'Sales', 5500);
insert into employee values(3, 'Charlie', 'IT', 6000);
insert into employee values(4, 'David', 'IT', 6500);
insert into employee values(5, 'Eve', 'HR', 4500);

insert into orders values(1, 1, 1000, '2024-01-01');
insert into orders values(2, 1, 1500, '2024-01-02');
insert into orders values(3, 2, 2000, '2024-01-03');
insert into orders values(4, 3, 800, '2024-01-04');
insert into orders values(5, 4, 1200, '2024-01-05');
insert into orders values(6, 5, 900, '2024-01-06');

insert into products values(1, 'Laptop', 1200, 'Electronics');
insert into products values(2, 'Phone', 800, 'Electronics');
insert into products values(3, 'Desk', 300, 'Furniture');
insert into products values(4, 'Chair', 200, 'Furniture');

select e.dept, COUNT(*) as emp_count, AVG(e.salary) as avg_salary from employee e group by e.dept;
select e.dept, SUM(o.amount) as total_sales from employee e, orders o where e.emp_id = o.emp_id group by e.dept;
select p.category, COUNT(*) as prod_count, AVG(p.price) as avg_price from products p group by p.category;

EOF

# 运行聚合函数测试
echo "开始聚合函数测试..."
./rmdb_client < aggregation_test.sql

# 清理临时文件
rm -f aggregation_test.sql 