# CRUD执行器模块说明

本文档说明 `crud.cpp` 和 `crud.h` 的作用。该模块负责执行SELECT、INSERT、UPDATE、DELETE操作，是SQL语句和CSV数据之间的桥梁。

## 1. 整体作用

CRUD执行器接收已解析的SQL命令结构，执行相应的数据库操作，并返回操作结果。它直接操作CSV表数据，实现基本的数据管理功能。

整体流程：

```text
SqlCommand结构
  -> 根据命令类型执行相应操作
  -> 通过CsvDatabase访问数据
  -> 修改或查询表数据
  -> 返回SqlResult结果
```

## 2. CrudExecutor::CrudExecutor

```cpp
CrudExecutor::CrudExecutor(CsvDatabase *database)
    : m_database(database)
{
}
```

作用：构造CRUD执行器对象，关联CSV数据库实例。

## 3. CrudExecutor::selectRows

```cpp
SqlResult CrudExecutor::selectRows(const SqlCommand &command)
```

作用：执行SELECT语句，查询并返回符合条件的数据。

执行流程：

1. 获取目标表对象
2. 确定要查询的列索引：
   - 如果是 `*`，选择所有列
   - 否则根据字段名查找列索引
3. 初始化结果对象，设置表头
4. 遍历表中的每一行，检查是否满足WHERE条件
5. 对于满足条件的行，提取指定列的值构成结果行
6. 设置统计信息并返回结果

例如对于SQL：`SELECT ID, NAME FROM student WHERE AGE > 20;`
- 选择ID和NAME两列
- 只返回AGE大于20的行
- 返回匹配行的ID和NAME字段

## 4. CrudExecutor::insertRow

```cpp
SqlResult CrudExecutor::insertRow(const SqlCommand &command)
```

作用：执行INSERT语句，向表中插入新记录。

执行流程：

1. 获取目标表对象
2. 创建与表头大小相同的新行，初始值为空
3. 根据字段名将插入值填入对应列
4. 确定主键字段：
   - score表：主键是 STU_ID 和 COURSE_ID
   - 其他表：主键是 ID
5. 检查必需的主键字段是否提供且不为空
6. 遍历现有行，检查主键冲突
7. 将新行添加到表中
8. 保存到CSV文件
9. 返回操作结果

例如对于SQL：`INSERT INTO student (ID, NAME, AGE) VALUES ('2025004', '赵六', 22);`
- 创建新行 ["2025004", "赵六", "22", ""]
- 检查ID不与其他记录重复
- 添加到表中并保存

## 5. CrudExecutor::updateRows

```cpp
SqlResult CrudExecutor::updateRows(const SqlCommand &command)
```

作用：执行UPDATE语句，修改符合条件的记录。

执行流程：

1. 获取目标表对象
2. 验证要更新的字段名是否存在
3. 记录要更新的列索引
4. 备份原始数据以防保存失败
5. 遍历表中的每一行，检查是否满足WHERE条件
6. 对于满足条件的行，更新指定列的值
7. 保存更改到CSV文件
8. 返回操作结果

例如对于SQL：`UPDATE student SET AGE = 22, GRADE = '及格' WHERE ID = '2025002';`
- 查找ID为'2025002'的行
- 将该行的AGE字段更新为'22'
- 将该行的GRADE字段更新为'及格'

## 6. CrudExecutor::deleteRows

```cpp
SqlResult CrudExecutor::deleteRows(const SqlCommand &command, bool allowDeleteAll)
```

作用：执行DELETE语句，删除符合条件的记录。

执行流程：

1. 获取目标表对象
2. 检查是否为全表删除操作：
   - 如果没有WHERE条件且不允许全表删除，返回确认请求
   - 否则继续执行
3. 备份原始数据以防保存失败
4. 创建新的行列表，只保留不满足删除条件的行
5. 统计被删除的行数
6. 用新行列表替换表数据
7. 保存到CSV文件
8. 返回操作结果

例如对于SQL：`DELETE FROM student WHERE AGE > 20;`
- 删除AGE大于20的所有行
- 保留AGE小于等于20的行

## 7. CrudExecutor::showTable

```cpp
SqlResult CrudExecutor::showTable(const QString &tableName)
```

作用：显示指定表的所有数据。

执行流程：

1. 获取目标表对象
2. 创建包含所有行的结果对象
3. 设置表头和所有数据行
4. 返回完整表数据

## 8. CrudExecutor::error

```cpp
SqlResult CrudExecutor::error(const QString &message) const
```

作用：创建错误结果。

执行流程：

1. 创建SqlResult对象
2. 设置 `success` 为 false
3. 设置错误信息
4. 返回结果

## 9. 安全措施

### 数据完整性
- INSERT操作检查主键完整性，防止插入缺少主键的记录
- INSERT操作检查主键冲突，防止重复主键
- UPDATE和DELETE操作通过WHERE条件限制影响范围

### 事务保护
- 在修改数据前备份原始数据
- 保存失败时恢复原始数据
- 确保数据一致性

### 操作确认
- DELETE操作在没有WHERE条件时要求确认，防止意外删除整表

## 10. 小结

`crud.cpp` 的作用可以概括为：

```text
执行已解析的SQL命令，实现对CSV数据的增删改查操作，并确保数据完整性和操作安全性。
```

它与SQL解析器配合工作，SQL解析器负责理解SQL语法，CRUD执行器负责执行具体的数据操作。CRUD模块不处理SQL语法，只处理已解析的命令结构。

职责划分：

```text
csv.cpp          负责CSV文件和内存表
sqlexecutor.cpp  负责SQL解析和命令分发
crud.cpp         负责实际的增删改查操作
mainwindow.cpp   负责界面显示
```