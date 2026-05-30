# std::filesystem 完全指南

## 一、概述

std::filesystem 是 C++17 引入的标准库，用于处理文件系统操作。它提供了一个跨平台的统一接口，让开发者可以轻松地进行文件和目录的操作。

**核心认知：std::filesystem = "C++对操作系统文件系统的统一抽象层"**

---

## 二、核心组件

### 1. 路径类（path）—— "字符串的升级版"

```cpp
#include <filesystem>
namespace fs = std::filesystem;

// 创建路径对象
fs::path p = "D:\\test\\a.txt";
```

**常用方法：**

- **filename()**
  - 功能：取最后一段
  - 示例：`D:\test\a.txt → a.txt`

- **extension()**
  - 功能：文件后缀
  - 示例：`a.txt → .txt`

- **parent_path()**
  - 功能：上一级目录
  - 示例：`D:\test\a.txt → D:\test`

### 2. 遍历类（核心）—— "目录就是一棵树"

#### directory_iterator（只遍历当前层）
```cpp
// 只遍历当前层（不进子目录）
for (auto &e : fs::directory_iterator(path)) {
    // e 是路径对象
    std::cout << e.path() << std::endl;
}
```

#### recursive_directory_iterator（深度优先遍历）
```cpp
// 深度优先遍历整棵目录树
for (auto &e : fs::recursive_directory_iterator(path)) {
    // e 是路径对象
    std::cout << e.path() << std::endl;
}
```

### 3. 判断类—— "文件系统状态查询"

```cpp
// 文件/目录是否存在
fs::exists(path)

// 是否文件夹
fs::is_directory(path)

// 是否普通文件（不是文件夹/链接）
fs::is_regular_file(path)
```

### 4. 操作类—— "直接改文件系统"

#### 创建目录
```cpp
// 创建单层目录
fs::create_directory("D:\\test");

// 递归创建目录
fs::create_directories("D:\\a\\b\\c");  // 自动创建多层
```

#### 删除
```cpp
// 删除文件或空目录
fs::remove(path);

// 强制删除（危险🔥）
fs::remove_all(path);  // 删除整个目录树
```

#### 重命名 / 移动
```cpp
// 文件移动 + 改名
fs::rename("a.txt", "b.txt");
```

#### 复制
```cpp
// 复制文件（可加 copy_options）
fs::copy("a.txt", "b.txt");
```

### 5. 当前目录—— "程序运行位置"

```cpp
// 获取当前路径
fs::current_path();  // 程序在哪个目录运行

// 修改当前路径
fs::current_path("D:\\test");
```

---

## 三、实际应用示例

### 示例1：遍历目录
```cpp
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

void list_files(const fs::path& path) {
    std::cout << "Listing files in: " << path << std::endl;
    
    // 只显示当前层
    std::cout << "=== Current layer ===" << std::endl;
    for (const auto& entry : fs::directory_iterator(path)) {
        std::cout << entry.path().filename() 
                  << (fs::is_directory(entry) ? " (dir)" : " (file)")
                  << std::endl;
    }
    
    // 递归显示所有
    std::cout << "\n=== Recursive ===" << std::endl;
    for (const auto& entry : fs::recursive_directory_iterator(path)) {
        std::string indent(entry.depth() * 4, ' ');
        std::cout << indent << entry.path().filename()
                  << (fs::is_directory(entry) ? " (dir)" : " (file)")
                  << std::endl;
    }
}
```

### 示例2：查找特定类型文件
```cpp
void find_files(const fs::path& path, const std::string& ext) {
    for (const auto& entry : fs::recursive_directory_iterator(path)) {
        if (entry.path().extension() == ext) {
            std::cout << "Found: " << entry.path() << std::endl;
        }
    }
}
```

### 示例3：创建目录结构
```cpp
void create_project_structure() {
    fs::path project_root = "MyProject";
    
    // 创建基础目录
    fs::create_directories(project_root / "src");
    fs::create_directories(project_root / "include");
    fs::create_directories(project_root / "build");
    
    std::cout << "Project structure created!" << std::endl;
}
```

### 示例4：清理临时文件
```cpp
void cleanup_temp_files() {
    fs::path temp_dir = "temp";
    
    if (fs::exists(temp_dir)) {
        // 删除整个目录树
        fs::remove_all(temp_dir);
        std::cout << "Temp files cleaned!" << std::endl;
    }
}
```

---

## 四、重要注意事项

### 1. 平台兼容性
- std::filesystem 自动处理不同操作系统的路径分隔符（/ 或 \）
- 路径拼接使用 `/` 操作符，会自动适应平台

### 2. 异常处理
文件系统操作可能会抛出异常：
```cpp
try {
    fs::create_directories("D:\\a\\b\\c");
} catch (const fs::filesystem_error& e) {
    std::cerr << "Filesystem error: " << e.what() << std::endl;
}
```

### 3. 性能考虑
- `exists()` 操作可能涉及系统调用
- 遍历大型目录树时要注意性能
- 使用 `status()` 或 `symlink_status()` 可以缓存状态信息

---

## 五、底层理解

| filesystem函数 | 底层实现 |
|--------------|---------|
| create_directory | mkdir |
| remove | delete |
| directory_iterator | FindFirstFile / readdir |
| path | 字符串 + OS路径解析 |

std::filesystem 实质是对操作系统文件 API 的封装，提供了统一的接口。

---

## 六、最佳实践

1. **使用 `fs::path` 而不是字符串**
   - 更安全，自动处理路径格式

2. **使用 `/` 操作符进行路径拼接**
   ```cpp
   fs::path p = "D:/test" / "subdir" / "file.txt";
   ```

3. **检查路径是否存在**
   ```cpp
   if (fs::exists(path)) {
       // 安全操作
   }
   ```

4. **使用 try-catch 处理异常**
   文件系统操作可能失败

5. **注意权限问题**
   不是所有操作都能成功，可能受到权限限制

---

## 七、总结

std::filesystem 是 C++17 提供的强大工具，它：

- 提供了跨平台的文件系统操作接口
- 简化了文件和目录的管理
- 提供了安全的路径处理
- 支持复杂的文件系统遍历和操作

掌握 std::filesystem 对于现代 C++ 开发者来说是非常重要的技能，特别是在处理文件系统相关的应用程序时。