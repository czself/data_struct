# 课程设计项目合集

本仓库收录了数据结构课程设计及相关开发项目，涵盖 C++/Qt 桌面应用和 Python 图形化工具。

## 项目一览

| 项目 | 语言 | 简介 |
|------|------|------|
| [student_query_system](./student_query_system) | C++ / Qt | 学生信息查询系统 — 基于 CSV 的 SQL 查询引擎 |
| [DeepSeekConfigTool](./DeepSeekConfigTool) | C++ / Qt | DeepSeek API 调用工具 — 对话式 AI 助手 |
| [PortablePyIDE](./PortablePyIDE) | Python / PyQt6 | 小学 Python 图形化 IDE — 零配置，双击即用 |

---

## student_query_system — 学生信息查询系统

基于 Qt 框架开发的学生信息查询系统，使用 CSV 文件存储数据，内置 SQL 解析引擎，支持 SELECT / INSERT / UPDATE / DELETE 操作。

**核心特性：**
- 完整的 SQL 解析器（WHERE 条件、字段映射、表名别名）
- 事务保护与失败回滚
- 主键唯一性检查、全表删除确认
- 逗号感知分割，智能 WHERE 关键字识别

**技术栈：** C++17 / Qt Widgets / CSV

**详细文档：** [student_query_system/README.md](./student_query_system/README.md)

---

## DeepSeekConfigTool — DeepSeek API 调用工具

基于 Qt 开发的 DeepSeek API 图形化调用工具，支持对话式交互，可配置 API 地址、模型和密钥。

**核心特性：**
- 对话式交互界面
- 可配置 API Base URL / Model / Key
- 实时日志输出
- 网络请求与界面分离的信号槽架构

**技术栈：** C++17 / Qt Widgets / Qt Network

**详细文档：** [DeepSeekConfigTool/实现说明.md](./DeepSeekConfigTool/实现说明.md)

---

## PortablePyIDE — 小学 Python 图形化 IDE

面向小学 3-6 年级的零配置 Python IDE，双击即可开始写代码。

**核心特性：**
- 语法高亮、代码提示、自动缩进
- 断点调试，变量实时查看
- AI 小老师（DeepSeek），用孩子能懂的话解释错误
- 多文件项目支持（设主文件后一键运行）
- 中文界面，按钮少而清晰

**技术栈：** Python / PyQt6 / QScintilla

**详细文档：** [PortablePyIDE/README.md](./PortablePyIDE/README.md)

---

## 资料

课程设计相关的 CSV 数据文件、Qt 练习源码、笔记和课程设计计划 PDF。

---

## 许可证

本项目采用 [LICENSE](./LICENSE) 中声明的许可证。
