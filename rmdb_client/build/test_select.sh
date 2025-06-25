#!/bin/bash
# Rucbase 顺序扫描测试脚本
cmake .. -DCMAKE_BUILD_TYPE=Release
make rmdb_client -j8
./rmdb_client << EOF
# =================================================================
# SELECT 语句测试套件
# =================================================================

# -----------------------------------------------------------------
# 设置: 清理环境并创建测试表
# -----------------------------------------------------------------
DROP TABLE student_grades;
DROP TABLE comparison_test;

# 1. 创建一个用于通用测试的表
# 包含整数、浮点数和字符串类型，以覆盖所有比较逻辑
CREATE TABLE student_grades (student_id INT, course CHAR(20), score FLOAT);

# 2. 插入多样化的测试数据
# 包含正数、负数、零、重复值和不同长度的字符串
INSERT INTO student_grades VALUES (1, 'Math', 95.5);
INSERT INTO student_grades VALUES (2, 'Physics', 88.0);
INSERT INTO student_grades VALUES (3, 'Chemistry', 72.5);
INSERT INTO student_grades VALUES (4, 'Math', 88.0);
INSERT INTO student_grades VALUES (5, 'English', 60.0);
INSERT INTO student_grades VALUES (6, 'History', 99.9);
INSERT INTO student_grades VALUES (7, 'Art', -10.0); # 测试负浮点数
INSERT INTO student_grades VALUES (8, 'Physics', 95.5);
INSERT INTO student_grades VALUES (9, 'Music', 0.0);   # 测试零浮点数
INSERT INTO student_grades VALUES (10, 'Chemistry', 72.5);
INSERT INTO student_grades VALUES (11, 'Computer', 95.5);
INSERT INTO student_grades VALUES (12, 'Biology', 85.0);
INSERT INTO student_grades VALUES (13, 'Art', 90.0); # 相同课程，不同分数
INSERT INTO student_grades VALUES (14, 'Arts', 91.0); # 用于比较的字符串前缀

# -----------------------------------------------------------------
# 测试用例组 1: 基本的 SELECT 和 WHERE 子句 (您的原始测试)
# -----------------------------------------------------------------

# 1.1 测试无条件全表扫描
# 预期: 返回所有 14 条记录
SELECT * FROM student_grades;

# 1.2 测试 INT 类型比较
# 预期: 1 条记录 (id=5)
SELECT * FROM student_grades WHERE student_id = 5;
# 预期: 13 条记录 (id<>5)
SELECT * FROM student_grades WHERE student_id <> 5;
# 预期: 2 条记录 (id=1, 2)
SELECT * FROM student_grades WHERE student_id < 3;
# 预期: 5 条记录 (id=10, 11, 12, 13, 14)
SELECT * FROM student_grades WHERE student_id > 9;
# 预期: 3 条记录 (id=1, 2, 3)
SELECT * FROM student_grades WHERE student_id <= 3;
# 预期: 6 条记录 (id=9, 10, 11, 12, 13, 14)
SELECT * FROM student_grades WHERE student_id >= 9;

# 1.3 测试 FLOAT 类型比较
# 预期: 3 条记录 (score=95.5)
SELECT * FROM student_grades WHERE score = 95.5;
# 预期: 5 条记录 (score > 90.0)
SELECT * FROM student_grades WHERE score > 90.0;
# 预期: 3 条记录 (score < 70.0)
SELECT * FROM student_grades WHERE score < 70.0;
# 预期: 7 条记录 (score >= 88.0)
SELECT * FROM student_grades WHERE score >= 88.0;

# 1.4 测试 CHAR 类型比较
# 预期: 2 条记录 (course='Math')
SELECT * FROM student_grades WHERE course = 'Math';
# 预期: 2 条记录 ('Art', 'Art')
SELECT * FROM student_grades WHERE course < 'Arts';
# 预期: 3 条记录 ('Physics', 'Physics', 'History')
SELECT * FROM student_grades WHERE course > 'Music';

# 1.5 测试多条件 (AND)
# 预期: 1 条记录 (score=88.0, course='Physics')
SELECT * FROM student_grades WHERE score = 88.0 AND course = 'Physics';
# 预期: 2 条记录 (id>5, score<70) -> (id=7, score=-10), (id=9, score=0)
SELECT * FROM student_grades WHERE student_id > 5 AND score < 70;

# 1.6 测试没有结果的查询
# 预期: 0 条记录
SELECT * FROM student_grades WHERE student_id = 999;
# 预期: 0 条记录
SELECT * FROM student_grades WHERE course = 'NonExistent';

# -----------------------------------------------------------------
# 测试用例组 2: 投影 (SELECT 特定列)
# -----------------------------------------------------------------

# 2.1 无条件选择列的子集
# 预期: 所有 14 条记录，但只有 student_id 和 score 列
SELECT student_id, score FROM student_grades;

# 2.2 带条件选择单列
# 预期: 2 条记录 ('Math', 'Math')
SELECT course FROM student_grades WHERE score = 88.0;

# 2.3 带条件选择多列
# 预期: 5 条带有 course 和 score 的记录
SELECT course, score FROM student_grades WHERE student_id > 9;

# -----------------------------------------------------------------
# 测试用例组 3: 逻辑 OR 条件
# -----------------------------------------------------------------

# 3.1 对同一列进行简单的 OR 操作
# 预期: 2 条记录 (id=5 或 id=7)
SELECT * FROM student_grades WHERE student_id = 5 OR student_id = 7;

# 3.2 对不同列进行 OR 操作 (FLOAT 和 CHAR)
# 预期: 5 条记录 (score < 65 -> id=5,7,9) 或 (course='Math' -> id=1,4). 唯一 ID: 1, 4, 5, 7, 9
SELECT * FROM student_grades WHERE score < 65.0 OR course = 'Math';

# -----------------------------------------------------------------
# 测试用例组 4: 列与列的比较
# -----------------------------------------------------------------

# 4.1 为列比较进行设置
CREATE TABLE comparison_test (id INT, val1 INT, val2 INT, name1 CHAR(10), name2 CHAR(10));
INSERT INTO comparison_test VALUES (1, 10, 10, 'abc', 'abc');
INSERT INTO comparison_test VALUES (2, 20, 30, 'def', 'xyz');
INSERT INTO comparison_test VALUES (3, 40, 30, 'hello', 'world');
INSERT INTO comparison_test VALUES (4, 50, 50, 'test', 'test');

# 4.2 测试两个 INT 列之间的相等性
# 预期: 2 条记录 (id=1, 4)
SELECT * FROM comparison_test WHERE val1 = val2;

# 4.3 测试两个 INT 列之间的不等性
# 预期: 1 条记录 (id=3)
SELECT * FROM comparison_test WHERE val1 > val2;

# 4.4 测试两个 CHAR 列之间的相等性
# 预期: 2 条记录 (id=1, 4)
SELECT * FROM comparison_test WHERE name1 = name2;

# 4.5 测试两个 CHAR 列之间的不等性
# 预期: 1 条记录 (id=2)
SELECT * FROM comparison_test WHERE name1 < name2;

# -----------------------------------------------------------------
# 最后清理
# -----------------------------------------------------------------
DROP TABLE student_grades;
DROP TABLE comparison_test;
EOF