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

# 健壮性测试
create table grade (course char(20),id int,score float);
insert into grade values('DataStructure',1,95);
insert into grade values('DataStructure',2,93.5);
insert into grade values('DataStructure',3,94.5);
insert into grade values('ComputerNetworks',1,99);
insert into grade values('ComputerNetworks',2,88.5);
insert into grade values('ComputerNetworks',3,92.5);

-- ==========================================
-- 测试1: SELECT 列表中不能出现没有在 GROUP BY 子句中的非聚集列
-- ==========================================

-- 基础测试：单个非聚集列
select id, score from grade group by course;

-- 多个非聚集列
select id, score, course from grade group by course;

-- 混合聚集和非聚集列
select course, id, AVG(score) from grade group by course;

-- 非聚集列在聚集函数之前
select id, course, MAX(score) from grade group by course;

-- 非聚集列在聚集函数之后
select course, MAX(score), id from grade group by course;

-- 多个GROUP BY列,但SELECT中有未包含的列
select id, score from grade group by course, id;

-- 使用别名的情况
select id as student_id, score as student_score from grade group by course;

-- 使用表名前缀的情况
select grade.id, grade.score from grade group by course;

-- ==========================================
-- 测试2: WHERE 子句中不能用聚集函数作为条件表达式
-- ==========================================

-- 基础测试：单个聚集函数
select id, MAX(score) as max_score from grade where MAX(score) > 90 group by id;

-- 多个聚集函数
select course, AVG(score) from grade where MAX(score) > 90 AND MIN(score) < 95 group by course;

-- 聚集函数与其他条件混合
select id, score from grade where MAX(score) > 90 AND course = 'DataStructure' group by id;

EOF

# 运行聚合函数测试
echo "开始聚合函数测试..."
./rmdb_client < aggregation_test.sql

# 清理临时文件
rm -f aggregation_test.sql 