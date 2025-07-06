-- MVCC测试脚本
-- 创建student表
CREATE TABLE student (
    id INT,
    score INT
);

-- 插入测试数据
INSERT INTO student VALUES (1, 80);
INSERT INTO student VALUES (2, 90);

-- 验证数据插入是否正确
SELECT * FROM student;

-- 测试UPDATE语句的表达式支持
UPDATE student SET score=score+5 WHERE id=1;

-- 验证UPDATE是否成功
SELECT * FROM student;

-- 测试事务功能
BEGIN;
UPDATE student SET score=score+10 WHERE id=2;
SELECT * FROM student;
COMMIT;

-- 最终验证
SELECT * FROM student;

-- 清理
DROP TABLE student; 