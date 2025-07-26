-- 测试NUL字符问题修复
-- 创建测试表
CREATE TABLE test_nul (
    id INT,
    name CHAR(10)
);

-- 插入测试数据
INSERT INTO test_nul VALUES (1, 'test1');
INSERT INTO test_nul VALUES (2, 'test2');
INSERT INTO test_nul VALUES (3, 'test3');

-- 查询数据，检查是否还有NUL字符
SELECT * FROM test_nul;

-- 测试聚合查询
SELECT COUNT(*) FROM test_nul;
SELECT id, COUNT(*) FROM test_nul GROUP BY id;

-- 清理
DROP TABLE test_nul; 