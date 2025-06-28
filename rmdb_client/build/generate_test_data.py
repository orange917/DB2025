import random
import string

def generate_random_name(length=5):
    """生成指定长度的随机字符串"""
    return ''.join(random.choices(string.ascii_letters, k=length))

def escape_string(value):
    """转义SQL字符串中的特殊字符"""
    return value.replace("'", "''")

def generate_insert_statements(file_path, num_records=50000):
    """生成INSERT语句并写入文件（简化语法版本）"""
    try:
        with open(file_path, 'w', encoding='utf-8') as f:
            create = f"create table compositetest (id int, name char(10), value float, category char(5));\n"
            f.write(create)
            for i in range(1, num_records + 1):
                # 生成随机数据
                name = generate_random_name()
                value = round(random.uniform(0, 1000), 2)
                category = f"cat{i % 10}"  # 生成cat0-cat9的分类

                # 转义字符串值
                escaped_name = escape_string(name)

                # 构造INSERT语句（简化语法）
                insert = f"insert into composite_test values ({i}, '{escaped_name}', {value}, '{category}');\n"
                
                # 写入文件
                f.write(insert)
        
        print(f"成功生成 {num_records} 条INSERT语句到文件: {file_path}")
        
    except Exception as e:
        print(f"生成INSERT语句时出错: {e}")

if __name__ == "__main__":
    # 指定输出文件路径
    output_file = "insert_statements.txt"
    
    # 生成INSERT语句
    generate_insert_statements(output_file)    