# 学生查询系统

## 项目概述

这是一个基于Qt框架开发的学生信息查询系统，支持使用SQL语句对学生、课程、班级和成绩数据进行查询、插入、更新和删除操作。

## 项目架构

### 数据层
- **CSV文件**: 四个CSV文件作为数据存储
  - `tab_student.csv`: 学生信息表 (ID, NAME, SEX, CLAZZ_ID)
  - `tab_course.csv`: 课程信息表 (ID, NAME, CREDIT)
  - `tab_class.csv`: 班级信息表 (ID, NAME)
  - `tab_score.csv`: 成绩信息表 (STU_ID, COURSE_ID, RESULT)

- **CsvTable类**: 管理单个CSV表的数据结构和文件操作
- **CsvDatabase类**: 统一管理所有CSV表

### SQL处理层
- **SqlExecutor类**: 解析和执行SQL语句
- **SqlCommand结构**: 存储解析后的SQL命令结构
- **解析函数**: 
  - `parse()`: 解析SQL语句
  - `parseWhere()`: 解析WHERE条件
  - `findWhereKeyword()`: 智能查找WHERE关键字
  - `splitCommaAware()`: 逗号感知分割
  - `cleanValue()`: SQL值清理

### 数据操作层
- **CrudExecutor类**: 执行实际的增删改查操作
- **操作函数**:
  - `selectRows()`: 执行SELECT语句
  - `insertRow()`: 执行INSERT语句
  - `updateRows()`: 执行UPDATE语句
  - `deleteRows()`: 执行DELETE语句

### 界面层
- **MainWindow类**: 主界面，提供SQL执行和结果显示
- **LoginDialog类**: 登录界面

## 功能特性

### SQL支持
- **SELECT语句**: 支持字段选择、WHERE条件过滤
- **INSERT语句**: 支持数据插入，包含主键检查
- **UPDATE语句**: 支持数据更新
- **DELETE语句**: 支持数据删除，包含安全确认

### 安全机制
- 主键唯一性检查
- 全表删除确认机制
- 数据备份机制
- 事务保护（失败回滚）

### 智能功能
- 字段名映射（如`class_id`自动映射到`CLAZZ_ID`）
- 表名别名（如`studentTable`自动映射到`student`）
- 智能WHERE关键字识别（避免字符串中的"where"被误识别）
- 逗号感知分割（正确处理引号内的逗号）

### 操作符支持
- 比较操作符: `=`, `!=`, `>`, `<`, `>=`, `<=`
- 逻辑操作符: `AND`

## 使用方法

### 基本查询
```sql
-- 查询所有学生
SELECT * FROM student;

-- 查询特定字段
SELECT ID, NAME FROM student;

-- 带条件查询
SELECT * FROM student WHERE NAME = '张三';
SELECT * FROM score WHERE RESULT >= 80;
```

### 数据插入
```sql
INSERT INTO student (ID, NAME, SEX, CLAZZ_ID) VALUES ('2025001', '新学生', '男', '101');
```

### 数据更新
```sql
UPDATE student SET NAME = '更新姓名' WHERE ID = '2025001';
```

### 数据删除
```sql
DELETE FROM student WHERE ID = '2025001';
```

## 项目亮点

1. **完整的SQL解析**: 支持复杂的SQL语法解析
2. **数据安全**: 多重保护机制防止数据损坏
3. **灵活映射**: 支持多种字段名和表名写法
4. **错误处理**: 完善的错误提示和异常处理
5. **事务保护**: 操作失败时自动回滚
6. **智能识别**: 避免字符串中的关键字被误识别

## 技术栈

- **编程语言**: C++
- **框架**: Qt
- **数据格式**: CSV
- **设计模式**: 分层架构，MVC模式

## 文件结构

```
├── csv.cpp/h          # CSV数据处理
├── sqlexecutor.cpp/h  # SQL解析和执行
├── crud.cpp/h         # CRUD操作
├── mainwindow.cpp/h   # 主界面
├── logindialog.cpp/h  # 登录界面
├── 资料/              # CSV数据文件
│   ├── tab_student.csv
│   ├── tab_course.csv
│   ├── tab_class.csv
│   └── tab_score.csv
└── 说明文档.md        # 项目说明文档
```

## 设计理念

该项目采用分层架构设计，将数据存储、SQL处理、数据操作和用户界面分离，实现了高内聚低耦合的设计目标。通过智能的SQL解析和安全的数据操作机制，为用户提供了一个功能完善、安全可靠的学生信息查询系统。