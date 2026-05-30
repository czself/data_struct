# CSV 数据模块说明

本文档说明 `csv.cpp` 和 `csv.h` 的作用。该模块负责在程序运行时把四个 CSV 文件加载到内存中，并在数据发生变化后写回 CSV 文件。

## 1. 整体作用

程序运行时，会把下面四个 CSV 文件当作四张数据表：

```text
tab_student.csv  -> student 表
tab_course.csv   -> course 表
tab_class.csv    -> class 表
tab_score.csv    -> score 表
```

每张表在内存中由一个 `CsvTable` 对象维护，四张表统一由 `CsvDatabase` 管理。

整体流程：

```text
CSV 文件
  -> 读取到内存
  -> 根据 SQL 进行查询、插入、修改、删除
  -> 在界面表格中显示结果
  -> 如果数据被修改，再写回 CSV 文件
```

## 2. normalizeName

```cpp
static QString normalizeName(const QString &name)
{
    QString result = name.trimmed().toUpper();
    result.remove('_');
    return result;
}
```

作用：统一字段名格式，方便比较。

例如：

```text
class_id   -> CLASSID
CLASS_ID   -> CLASSID
 name      -> NAME
```

这样用户 SQL 中写 `class_id`、`CLASS_ID`、`Class_Id` 时，程序都能识别成同一个字段。

## 3. parseCsvLine

```cpp
static QStringList parseCsvLine(const QString &line)
{
    return line.split(',');
}
```

作用：把 CSV 文件中的一行按逗号切开。

例如：

```text
2025754101,曹千山,,101
```

会变成：

```cpp
["2025754101", "曹千山", "", "101"]
```

其中 `,,` 表示中间有一个空字段。

## 4. CsvTable 类

`CsvTable` 表示一张 CSV 表。

它的主要成员变量：

```cpp
QString m_key;
QString m_path;
QStringList m_headers;
QVector<QStringList> m_rows;
bool m_backupCreated;
```

含义：

```text
m_key            表名，例如 student、course
m_path           CSV 文件路径
m_headers        表头字段
m_rows           所有数据行
m_backupCreated  是否已经创建过备份
```

## 5. CsvTable::CsvTable()

```cpp
CsvTable::CsvTable()
    : m_backupCreated(false)
{
}
```

作用：创建一张空表，并标记当前还没有创建备份。

## 6. CsvTable::CsvTable(const QString &key, const QString &path)

```cpp
CsvTable::CsvTable(const QString &key, const QString &path)
    : m_key(key),
      m_path(path),
      m_backupCreated(false)
{
}
```

作用：创建一张具体的 CSV 表。

例如：

```cpp
CsvTable("student", "../资料/tab_student.csv")
```

表示这张表的内部表名是 `student`，数据来源是 `../资料/tab_student.csv`。

## 7. CsvTable::load

```cpp
bool CsvTable::load(QString *errorMessage)
```

作用：从 CSV 文件读取数据。

执行流程：

1. 打开 CSV 文件。
2. 清空原来的 `m_headers` 和 `m_rows`。
3. 读取第一行，按逗号切开，作为表头保存到 `m_headers`。
4. 继续读取后面的每一行，按逗号切开，作为数据保存到 `m_rows`。
5. 如果某一行字段数量少于表头数量，就补空字符串。
6. 如果某一行字段数量多于表头数量，就删除多余字段。

例如学生表：

```text
ID,NAME,SEX,CLAZZ_ID
2025754101,曹千山,,101
```

读取后：

```cpp
m_headers = ["ID", "NAME", "SEX", "CLAZZ_ID"];

m_rows = [
    ["2025754101", "曹千山", "", "101"]
];
```

## 8. CsvTable::save

```cpp
bool CsvTable::save(QString *errorMessage)
```

作用：把内存中的表数据写回 CSV 文件。

执行流程：

1. 调用 `ensureBackup()`，第一次写入前先备份原始 CSV。
2. 用覆盖模式打开 CSV 文件。
3. 把 `m_headers` 用逗号连接，写入第一行。
4. 把 `m_rows` 中的每一行用逗号连接，逐行写入文件。

例如：

```cpp
["2025754101", "曹千山", "", "101"]
```

写回 CSV 后是：

```text
2025754101,曹千山,,101
```

## 9. CsvTable::key

```cpp
QString CsvTable::key() const
{
    return m_key;
}
```

作用：返回表的内部名字。

可能的返回值：

```text
student
course
class
score
```

## 10. CsvTable::path

```cpp
QString CsvTable::path() const
{
    return m_path;
}
```

作用：返回当前表对应的 CSV 文件路径。

例如：

```text
../资料/tab_student.csv
```

## 11. CsvTable::headers

```cpp
QStringList CsvTable::headers() const
{
    return m_headers;
}
```

作用：返回表头字段。

例如学生表：

```cpp
["ID", "NAME", "SEX", "CLAZZ_ID"]
```

## 12. CsvTable::rows const 版本

```cpp
QVector<QStringList> CsvTable::rows() const
{
    return m_rows;
}
```

作用：返回所有数据行的副本。

这个版本适合只读取数据，不修改数据。

## 13. CsvTable::rows 非 const 版本

```cpp
QVector<QStringList> &CsvTable::rows()
{
    return m_rows;
}
```

作用：返回真实的数据行引用，可以直接修改内存中的表数据。

`sqlexecutor.cpp` 中执行 `insert`、`update`、`delete` 时，会通过这个函数修改数据。

## 14. CsvTable::columnIndex

```cpp
int CsvTable::columnIndex(const QString &fieldName) const
```

作用：根据字段名查找它在表头中的列下标。

例如学生表：

```text
ID,NAME,SEX,CLAZZ_ID
```

调用：

```cpp
columnIndex("name")
```

返回：

```text
1
```

因为 `NAME` 是第 2 列，下标从 0 开始，所以返回 1。

## 15. CsvTable::actualFieldName

```cpp
QString CsvTable::actualFieldName(const QString &fieldName) const
```

作用：把用户 SQL 中写的字段名转换成 CSV 文件中的真实字段名。

特殊映射：

```text
student 表:
class_id -> CLAZZ_ID

score 表:
score -> RESULT
```

因此用户可以写：

```sql
select class_id from student;
select score from score;
```

程序会自动映射到 CSV 中的 `CLAZZ_ID` 和 `RESULT`。

## 16. CsvTable::ensureBackup

```cpp
bool CsvTable::ensureBackup(QString *errorMessage)
```

作用：第一次写入 CSV 文件前，先创建备份。

例如修改学生表时，会生成：

```text
tab_student.csv.bak
```

如果备份文件已经存在，就不会重复创建。

## 17. CsvDatabase 类

`CsvDatabase` 表示四张 CSV 表的集合。

它内部保存：

```cpp
QMap<QString, CsvTable> m_tables;
```

含义是：

```text
表名 -> CsvTable 对象
```

## 18. CsvDatabase::CsvDatabase

```cpp
CsvDatabase::CsvDatabase()
{
    m_tables.insert("student", CsvTable("student", "../资料/tab_student.csv"));
    m_tables.insert("course", CsvTable("course", "../资料/tab_course.csv"));
    m_tables.insert("class", CsvTable("class", "../资料/tab_class.csv"));
    m_tables.insert("score", CsvTable("score", "../资料/tab_score.csv"));
}
```

作用：注册四张 CSV 表。

对应关系：

```text
student -> ../资料/tab_student.csv
course  -> ../资料/tab_course.csv
class   -> ../资料/tab_class.csv
score   -> ../资料/tab_score.csv
```

## 19. CsvDatabase::loadAll

```cpp
bool CsvDatabase::loadAll(QString *errorMessage)
```

作用：把四张 CSV 表全部加载到内存中。

程序启动后，主窗口会调用它。

## 20. CsvDatabase::table

```cpp
CsvTable *CsvDatabase::table(const QString &tableName)
```

作用：根据表名获取对应的表，并返回可修改指针。

例如：

```cpp
table("student")
```

会返回学生表。

`insert`、`update`、`delete` 需要修改数据，所以会使用这个版本。

## 21. CsvDatabase::table const 版本

```cpp
const CsvTable *CsvDatabase::table(const QString &tableName) const
```

作用：根据表名获取对应的表，并返回只读指针。

这个版本适合只读取数据，不修改数据。

## 22. CsvDatabase::tableKey

```cpp
QString CsvDatabase::tableKey(const QString &tableName) const
```

作用：把用户 SQL 中输入的表名转换成程序内部表名。

支持：

```text
student / studentTable -> student
course / courseTable   -> course
class / classTable     -> class
score / scoreTable     -> score
```

因此下面两条 SQL 都可以识别：

```sql
select * from student;
select * from studentTable;
```

## 23. 小结

`csv.cpp` 的作用可以概括为一句话：

```text
把四个 CSV 文件加载成内存中的四张表，并在数据修改后负责写回文件。
```

它不负责解析 SQL，也不负责界面显示。

职责划分：

```text
csv.cpp          负责 CSV 文件和内存表
sqlexecutor.cpp 负责 SQL 解析和增删改查
mainwindow.cpp  负责界面显示
```
