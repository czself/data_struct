# DeepSeek 实验配置工具

这是一个基于 Qt Widgets 的 DeepSeek API 调用小工具。它可以填写 API 地址和 API Key，选择包含 `.docx` 实验报告的目录，并把提示词发送给 DeepSeek 接口，返回内容会显示在日志窗口中。

## 项目结构

```text
DeepSeekConfigTool/
├── DeepSeekConfigTool.pro
├── main.cpp
├── mainwindow.h
├── mainwindow.cpp
├── deepseekapi.h
└── deepseekapi.cpp
```

## 编译运行

```bash
qmake DeepSeekConfigTool.pro
make
./DeepSeekConfigTool
```

默认接口地址为：

```text
https://api.deepseek.com
```

使用时需要填入自己的 API Key。

更详细的实现原理见：[实现说明.md](实现说明.md)。
