-- MVCC快照隔离测试脚本
-- 测试场景：验证事务T1不会看到在其开始后提交的T2的修改

-- 清理环境
DROP TABLE IF EXISTS student;

-- 创建测试表
CREATE TABLE student (
    id INT,
    score INT
);

-- 插入初始数据
INSERT INTO student VALUES (1, 80);
INSERT INTO student VALUES (2, 90);

-- 查看初始数据
SELECT * FROM student;

-- 开始事务T1并获取初始快照
BEGIN;
SELECT '=== T1开始，查看初始数据 ===' as msg;
SELECT * FROM student;

-- 在另一个连接中模拟T2的操作
-- T2: BEGIN; SELECT * FROM student; UPDATE student SET score = 95 WHERE id = 2; COMMIT;

SELECT '=== T1更新id=1的记录 ===' as msg;
UPDATE student SET score = 82 WHERE id = 1;

SELECT '=== T1查看数据（不应该看到T2的修改，score(id=2)应该仍是90） ===' as msg;
SELECT * FROM student;

-- 提交T1
COMMIT;

SELECT '=== T1提交后，查看最终数据 ===' as msg;
SELECT * FROM student; 