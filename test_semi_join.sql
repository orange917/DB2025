-- 半连接测试脚本
-- 创建测试表
create table departments (dept_id int, dept_name char(20));
create table employees (emp_id int, emp_name char(20), dept_id int, salary int);

-- 插入测试数据
insert into departments values(1, 'HR');
insert into departments values(2, 'Engineering');
insert into departments values(3, 'Sales');
insert into departments values(4, 'Marketing');

insert into employees values(101, 'Alice', 1, 70000);
insert into employees values(102, 'Bob', 2, 80000);
insert into employees values(103, 'Charlie', 2, 90000);
insert into employees values(104, 'David', 1, 75000);

-- 测试半连接：查询有员工的部门
select dept_id, dept_name from departments SEMI JOIN employees ON departments.dept_id = employees.dept_id;

-- 测试半连接：查询有高薪员工的部门（工资大于80000）
select dept_id, dept_name from departments SEMI JOIN employees ON departments.dept_id = employees.dept_id AND employees.salary > 80000;

-- 测试半连接：查询有多个员工的部门
select dept_id, dept_name from departments SEMI JOIN employees ON departments.dept_id = employees.dept_id AND employees.emp_id > 102; 