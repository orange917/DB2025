#!/bin/bash
# Rucbase Sequential Scan Test Script
cmake .. -DCMAKE_BUILD_TYPE=Release
make rmdb_client -j8
./rmdb_client << EOF
# 清理环境（如果存在旧表）
DROP TABLE student_grades;

# 1. 创建一个用于测试的表
# 包含整数、浮点数和字符串类型，以覆盖所有比较逻辑
CREATE TABLE student_grades (student_id INT,course CHAR (20 ),score FLOAT);
# 2. 插入多样化的测试数据
# 包含正数、负数、零、重复值和不同长度的字符串
INSERT INTO student_grades VALUES  (1, 'Math', 95.5 );
INSERT INTO student_grades VALUES  (2, 'Physics', 88.0 );
INSERT INTO student_grades VALUES  (3, 'Chemistry', 72.5 );
INSERT INTO student_grades VALUES  (4, 'Math', 88.0 );
INSERT INTO student_grades VALUES  (5, 'English', 60.0 );
INSERT INTO student_grades VALUES  (6, 'History', 99.9 );
INSERT INTO student_grades VALUES  (7, 'Art', -10.0 ); # 测试负数
INSERT INTO student_grades VALUES  (8, 'Physics', 95.5 );
INSERT INTO student_grades VALUES  (9, 'Music', 0.0 );   # 测试零
INSERT INTO student_grades VALUES  (10, 'Chemistry', 72.5 );
INSERT INTO student_grades VALUES  (11, 'Computer', 95.5 );
INSERT INTO student_grades VALUES  (12, 'Biology', 85.0 );
INSERT INTO student_grades VALUES (12, 'Biology', 85.0);


# 3. 执行 SELECT 查询进行测试

# 3.1 测试无条件的全表扫描
# 预期: 返回所有 12 条记录
SELECT * FROM student_grades;

# 3.2 测试整数(INT)类型的比较
# 等于 (=)
# 预期: 返回 student_id 为 5 的 1 条记录
SELECT * FROM student_grades WHERE student_id = 5;
# 不等于 (!=)
# 预期: 返回除 student_id 为 5 外的 11 条记录
SELECT * FROM student_grades WHERE student_id != 5;
# 小于 (<)
# 预期: 返回 student_id 为 1, 2 的 2 条记录
SELECT * FROM student_grades WHERE student_id < 3;
# 大于 (>)
# 预期: 返回 student_id 为 10, 11, 12 的 3 条记录
SELECT * FROM student_grades WHERE student_id > 9;
# 小于等于 (<=)
# 预期: 返回 student_id 为 1, 2, 3 的 3 条记录
SELECT * FROM student_grades WHERE student_id <= 3;
# 大于等于 (>=)
# 预期: 返回 student_id 为 9, 10, 11, 12 的 4 条记录
SELECT * FROM student_grades WHERE student_id >= 9;

# 3.3 测试浮点数(FLOAT)类型的比较
# 等于 (=)
# 预期: 返回 score 为 95.5 的 3 条记录
SELECT * FROM student_grades WHERE score = 95.5;
# 大于 (>)
# 预期: 返回 score 大于 90 的 4 条记录 (95.5, 99.9, 95.5, 95.5)
SELECT * FROM student_grades WHERE score > 90.0;
# 小于 (<)
# 预期: 返回 score 小于 70 的 3 条记录 (60.0, -10.0, 0.0)
SELECT * FROM student_grades WHERE score < 70.0;
# 大于等于 (>=)
# 预期: 返回 score 为 88.0 或更高的 6 条记录
SELECT * FROM student_grades WHERE score >= 88.0;

# 3.4 测试字符串(VARCHAR)类型的比较
# 等于 (=)
# 预期: 返回 course 为 'Math' 的 2 条记录
SELECT * FROM student_grades WHERE course = 'Math';
# 小于 (<) # 按字典序比较
# 预期: 返回 'Art', 'Biology' 2 条记录
SELECT * FROM student_grades WHERE course < 'Chemistry';
# 大于 (>) # 按字典序比较
# 预期: 返回 'Physics', 'Physics' 2 条记录
SELECT * FROM student_grades WHERE course > 'Music';

# 3.5 测试多条件查询 (AND)
# 预期: 返回 score 为 88.0 且 course 为 'Physics' 的 1 条记录
SELECT * FROM student_grades WHERE score = 88.0 AND course = 'Physics';
# 预期: 返回 student_id 大于 5 且 score 小于 80 的 3 条记录 (Art, Music, Chemistry)
SELECT * FROM student_grades WHERE student_id > 5 AND score < 80;

# 3.6 测试没有结果的查询
# 预期: 返回 0 条记录
SELECT * FROM student_grades WHERE student_id = 999;
# 预期: 返回 0 条记录
SELECT * FROM student_grades WHERE course = 'NonExistent';
# 测试完成，清理环境
DROP TABLE student_grades;
EOF
DROP TABLE student_grades;