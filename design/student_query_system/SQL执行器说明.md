# SQL执行器模块说明

本文档说明 `sqlexecutor.cpp` 和 `sqlexecutor.h` 的作用。该模块负责解析SQL语句并执行相应的数据库操作。

## 1. 整体作用

SQL执行器接收用户输入的SQL语句，将其解析为内部命令结构，然后执行相应的查询、插入、更新或删除操作。它作为SQL语言和底层数据操作之间的桥梁。

整体流程：

```text
用户SQL语句
  -> 解析为SqlCommand结构
  -> 根据命令类型执行相应操作
  -> 通过CrudExecutor操作数据
  -> 返回SqlResult结果
```

## 2. findWhereKeyword

```cpp
static int findWhereKeyword(const QString &text)
```

作用：在SQL语句中查找WHERE关键字的位置，同时考虑引号内的内容。

执行流程：

1. 初始化 `inSingleQuote` 和 `inDoubleQuote` 为 false
2. 遍历文本中的每个字符
3. 如果是单引号且不在双引号内，切换 `inSingleQuote` 状态
4. 如果是双引号且不在单引号内，切换 `inDoubleQuote` 状态
5. 如果在引号内，跳过继续下一个字符
6. 如果不在引号内，检查当前位置是否有 "where" 关键字
7. 验证 "where" 前后是否为空白字符或字符串边界
8. 返回找到的 "where" 位置，未找到则返回 -1

这个函数确保不会将字符串常量中的 "where" 误识别为SQL关键字。

## 3. SqlExecutor::SqlExecutor()

```cpp
SqlExecutor::SqlExecutor()
    : m_crudExecutor(&m_database)
{
}
```

作用：构造SQL执行器对象，初始化内部的CRUD执行器，使其能够操作数据库。

## 4. SqlExecutor::load

```cpp
bool SqlExecutor::load(QString *errorMessage)
```

作用：加载所有CSV表数据到内存中。

执行流程：

1. 调用 `m_database.loadAll(errorMessage)`
2. 返回加载结果

## 5. SqlExecutor::execute

```cpp
SqlResult SqlExecutor::execute(const QString &sql, bool allowDeleteAll = false)
```

作用：执行SQL语句并返回结果。

执行流程：

1. 调用 `parse` 方法解析SQL语句
2. 如果解析失败，返回错误结果
3. 根据命令类型执行相应操作：
   - "select" -> 调用 `m_crudExecutor.selectRows(command)`
   - "insert" -> 调用 `m_crudExecutor.insertRow(command)`
   - "update" -> 调用 `m_crudExecutor.updateRows(command)`
   - "delete" -> 调用 `m_crudExecutor.deleteRows(command, allowDeleteAll)`
4. 返回操作结果

## 6. SqlExecutor::showTable

```cpp
SqlResult SqlExecutor::showTable(const QString &tableName)
```

作用：显示指定表的所有数据。

执行流程：

1. 调用 `m_crudExecutor.showTable(tableName)`
2. 返回查询结果

## 7. SqlExecutor::parse

```cpp
SqlResult SqlExecutor::parse(const QString &sql, SqlCommand *command) const
```

作用：解析SQL语句为内部命令结构。

支持的SQL语句类型：

### SELECT语句
```regex
^select\s+(.+)\s+from\s+([A-Za-z_][A-Za-z0-9_]*)(?:\s+where\s+(.+))?$
```

解析内容：
- 捕获1：字段列表
- 捕获2：表名
- 捕获3：WHERE条件（可选）

### INSERT语句
```regex
^insert\s+into\s+([A-Za-z_][A-Za-z0-9_]*)\s*\((.+)\)\s*values\s*\((.+)\)$
```

解析内容：
- 捕获1：表名
- 捕获2：字段列表
- 捕获3：值列表

### UPDATE语句
```regex
^update\s+([A-Za-z_][A-Za-z0-9_]*)\s+set\s+(.+)$
```

解析内容：
- 捕获1：表名
- 捕获2：SET子句（需要进一步解析WHERE条件）

### DELETE语句
```regex
^delete\s+from\s+([A-Za-z_][A-Za-z0-9_]*)(?:\s+where\s+(.+))?$
```

解析内容：
- 捕获1：表名
- 捕获2：WHERE条件（可选）

## 8. SqlExecutor::parseWhere

```cpp
SqlResult SqlExecutor::parseWhere(const QString &whereText, QVector<SqlCondition> *conditions) const
```

作用：解析WHERE子句中的条件。

执行流程：

1. 使用正则表达式 `\s+and\s+` 按AND操作符分割条件
2. 遍历每个条件部分
3. 使用正则表达式 `^\s*([A-Za-z_][A-Za-z0-9_]*)\s*(>=|<=|!=|=|>|<)\s*(.+)\s*$` 解析单个条件
4. 提取字段名、操作符和值
5. 添加到条件向量中

支持的操作符：`>=`, `<=`, `!=`, `=`, `>`, `<`

## 9. SqlExecutor::splitCommaAware

```cpp
QStringList SqlExecutor::splitCommaAware(const QString &text) const
```

作用：智能地按逗号分割文本，但忽略引号内的逗号。

执行流程：

1. 遍历文本中的每个字符
2. 如果是单引号且不在双引号内，切换 `inSingleQuote` 状态
3. 如果是双引号且不在单引号内，切换 `inDoubleQuote` 状态
4. 如果是逗号且不在任何引号内，将当前项添加到结果列表并清空
5. 其他情况下，将字符添加到当前项
6. 最后添加剩余的当前项

例如：`field1, "value, with comma", field3` 会分割为：
```cpp
["field1", " \"value, with comma\" ", " field3"]
```

## 10. SqlExecutor::cleanValue

```cpp
QString SqlExecutor::cleanValue(const QString &value) const
```

作用：清理SQL值，移除引号并处理转义字符。

执行流程：

1. 去除前后空白字符
2. 如果长度大于等于2，检查首尾字符是否为相同类型的引号
3. 如果是，则移除首尾引号
4. 替换转义的单引号 `\'` 为普通单引号 `'`
5. 替换转义的双引号 `\"` 为普通双引号 `"`

例如：
- 输入 `'It\'s a test'` 输出 `It's a test`
- 输入 `"He said \"hello\""` 输出 `He said "hello"`

## 11. SqlExecutor::error

```cpp
SqlResult SqlExecutor::error(const QString &message) const
```

作用：创建错误结果。

执行流程：

1. 创建SqlResult对象
2. 设置 `success` 为 false
3. 设置 `message` 为传入的错误信息
4. 返回结果

## 12. 私有成员变量

```cpp
CsvDatabase m_database;
CrudExecutor m_crudExecutor;
```

- `m_database`：管理CSV数据库
- `m_crudExecutor`：执行实际的增删改查操作

## 13. 小结

`sqlexecutor.cpp` 的作用可以概括为：

```text
解析用户输入的SQL语句，将其转换为内部命令结构，并执行相应的数据库操作。
```

它不直接操作CSV文件，而是通过CrudExecutor模块间接操作数据。SQL执行器专注于语法解析和命令分发，数据的实际操作由CRUD模块完成。

职责划分：

```text
csv.cpp          负责CSV文件和内存表
sqlexecutor.cpp  负责SQL解析和命令分发
crud.cpp         负责实际的增删改查操作
mainwindow.cpp   负责界面显示
```