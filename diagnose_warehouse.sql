-- 诊断warehouse表的问题
-- 1. 检查表结构
desc warehouse;

-- 2. 检查记录数量
select count(*) from warehouse;

-- 3. 检查前几条记录
select * from warehouse limit 5;

-- 4. 检查后几条记录
select * from warehouse order by id desc limit 5;

-- 5. 检查特定范围的记录
select * from warehouse where id between -10 and 10;

-- 6. 检查字符串字段的长度
select id, length(name) as name_length, name from warehouse limit 10; 