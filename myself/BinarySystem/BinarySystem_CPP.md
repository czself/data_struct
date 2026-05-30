# C++ 二进制位运算笔记

> 本笔记用 int 类型举例，long long 类型同理
> 注意：long long 类型有 64 位，应该使用 1LL << i

---

## 1. 打印 int 的 32 位二进制

```cpp
#include <iostream>
#include <bitset>
using namespace std;

// 方法1：使用 bitset
void printBinary1(int num) {
    cout << bitset<32>(num) << endl;
}

// 方法2：手动逐位打印
void printBinary2(int num) {
    for (int i = 31; i >= 0; i--) {
        cout << ((num & (1 << i)) == 0 ? "0" : "1");
    }
    cout << endl;
}
```

**注意**：

- 不能写成 `(num & (1 << i)) == 1`
- 因为如果第 i 位是 1，结果是 2 的 i 次方，不一定是 1
- 例如：a = 0010011，a 的第 4 位是 1
- `(a & (1 << 4)) == 16`，不是 1

**对于 long long（64位）**：

```cpp
void printBinary64(long long num) {
    for (int i = 63; i >= 0; i--) {
        cout << ((num & (1LL << i)) == 0 ? "0" : "1");
    }
    cout << endl;
}
```

---

## 2. 二进制/十六进制字面量（C++14+）

```cpp
// 十进制
int a = 78;

// 二进制（0b 开头，C++14+）
int c = 0b1001110;  // 78

// 十六进制（0x 开头）
// 0100 -> 4
// 1110 -> e
// 0x4e -> 01001110
int d = 0x4e;  // 78
```

---

## 3. 按位取反 ~ 和相反数

```cpp
int a = 78;
printBinary2(a);        // 打印 a 的二进制
printBinary2(~a);      // 按位取反

// ~a + 1 = -a（相反数）
int e = ~a + 1;
printBinary2(e);
```

**重要结论**：`~a + 1 = -a`

---

## 4. int 和 long long 的最小值

```cpp
#include <climits>

int f = INT_MIN;           // -2147483648
long long g = LLONG_MIN;  // -9223372036854775808

cout << f << endl;         // -2147483648
printBinary2(f);          // 10000000000000000000000000000000
cout << -f << endl;       // 还是 -2147483648（最小值取负还是自己）
```

**特点**：最小值取相反数、绝对值都是自己

---

## 5. 位运算符 | & ^

```cpp
int g = 0b0001010;  // 10
int h = 0b0001100;  // 12

printBinary2(g | h);   // 按位或   00000000000000000000000000001110 = 14
printBinary2(g & h);   // 按位与   00000000000000000000000000001000 = 8
printBinary2(g ^ h);   // 按位异或 00000000000000000000000000000110 = 6
```

| 运算符 | 名称 | 规则      |
| ------ | ---- | --------- |
| `\|`  | 或   | 有 1 则 1 |
| `&`  | 与   | 有 0 则 0 |
| `^`  | 异或 | 不同为 1  |

---

## 6. 短路特性 || && vs 位运算符 | &

```cpp
bool returnTrue() {
    cout << "进入了returnTrue函数" << endl;
    return true;
}

bool returnFalse() {
    cout << "进入了returnFalse函数" << endl;
    return false;
}

int main() {
    // | 和 & 会计算两侧表达式
    bool test1 = returnTrue() | returnFalse();  // 两个函数都会执行

    // || 和 && 有短路特性
    bool test2 = returnTrue() || returnFalse(); // 第一个为true，不执行第二个

    return 0;
}
```

**区别**：

- `|`、`&`：两侧都会计算
- `||`、`&&`：有短路特性，左侧决定结果就不计算右侧

---

## 7. 左移 <<

```cpp
int i = 0b0011010;  // 26
printBinary2(i << 1);  // 左移 1 位 -> 52（乘以 2）
printBinary2(i << 2);  // 左移 2 位 -> 104（乘以 4）
printBinary2(i << 3);  // 左移 3 位 -> 208（乘以 8）
```

**公式**：`n << k` = `n * 2^k`（仅对非负数有效）

---

## 8. 右移 >> 和无符号右移

```cpp
int i = 0b0011010;  // 26（非负数）
printBinary2(i >> 2);   // 右移 2 位 -> 6   26/4

// C++ 没有 >>>，用 unsigned 转换
unsigned int ui = i;
printBinary2(ui >> 2);  // 无符号右移 2 位 -> 6

int j = -1;  // 负数（全1）
printBinary2(j >> 2);   // 右移，高位补符号位（1）
unsigned int uj = j;
printBinary2(uj >> 2);  // 无符号右移，高位补 0
```

| 运算符      | 名称       | 特点                      |
| ----------- | ---------- | ------------------------- |
| `>>`      | 右移       | 高位补符号位（负数补 1）  |
| unsigned >> | 无符号右移 | 高位补 0（转成 unsigned） |

**C++ 中模拟无符号右移**：

```cpp
unsigned int num = your_int;
num >> k;  // 无符号右移
```

---

## 9. 移位运算总结

| 操作          | 非负数      | 负数     |
| ------------- | ----------- | -------- |
| `<< k`      | `n * 2^k` | 可能溢出 |
| `>> k`      | `n / 2^k` | 高位补 1 |
| unsigned >> k | 高位补 0    | 高位补 0 |

**注意**：只有非负数适合用移位做乘除运算！

---

## 10. 常见位运算技巧

```cpp
// 获取第 i 位
(num >> i) & 1

// 设置第 i 位为 1
num | (1 << i)

// 设置第 i 位为 0
num & ~(1 << i)

// 切换第 i 位
num ^ (1 << i)

// 获取最低位的 1
num & (-num)

// 清除最低位的 1
num & (num - 1)

// 判断是否为 2 的幂次
(num & (num - 1)) == 0

// 交换两个数（不用临时变量）
a = a ^ b;
b = a ^ b;
a = a ^ b;
```

---

## 11. C++ 中检查溢出

```cpp
#include <limits>
#include <cstdint>

// 检查左移是否溢出
int leftShift(int num, int k) {
    if (k < 0 || k >= 31) return 0;  // 非法移位
    if (num > (INT_MAX >> k)) {
        cout << "可能溢出" << endl;
    }
    return num << k;
}
```

---

## 12. 完整示例代码

```cpp
#include <iostream>
#include <bitset>
#include <climits>
using namespace std;

void printBinary(int num) {
    for (int i = 31; i >= 0; i--) {
        cout << ((num & (1 << i)) == 0 ? "0" : "1");
    }
    cout << endl;
}

bool returnTrue() {
    cout << "进入了returnTrue函数" << endl;
    return true;
}

bool returnFalse() {
    cout << "进入了returnFalse函数" << endl;
    return false;
}

int main() {
    // 非负数
    int a = 78;
    cout << a << endl;
    printBinary(a);
    cout << "===a===" << endl;

    // 负数
    int b = -6;
    cout << b << endl;
    printBinary(b);
    cout << "===b===" << endl;

    // 二进制字面量
    int c = 0b1001110;
    cout << c << endl;
    printBinary(c);
    cout << "===c===" << endl;

    // 十六进制字面量
    int d = 0x4e;
    cout << d << endl;
    printBinary(d);
    cout << "===d===" << endl;

    // ~ 和相反数
    cout << a << endl;
    printBinary(a);
    printBinary(~a);
    int e = ~a + 1;
    cout << e << endl;
    printBinary(e);
    cout << "===e===" << endl;

    // int 最小值
    int f = INT_MIN;
    cout << f << endl;
    printBinary(f);
    cout << "===f===" << endl;

    // | & ^
    int g = 0b0001010;
    int h = 0b0001100;
    printBinary(g | h);
    printBinary(g & h);
    printBinary(g ^ h);
    cout << "===g、h===" << endl;

    // 短路测试
    cout << "test1测试开始" << endl;
    bool test1 = returnTrue() | returnFalse();
    cout << "test1结果，" << test1 << endl;

    cout << "test2测试开始" << endl;
    bool test2 = returnTrue() || returnFalse();
    cout << "test2结果，" << test2 << endl;
    cout << "===|、&、||、&&===" << endl;

    // <<
    int i = 0b0011010;
    printBinary(i);
    printBinary(i << 1);
    printBinary(i << 2);
    printBinary(i << 3);
    cout << "===i << ===" << endl;

    // >>
    printBinary(i);
    printBinary(i >> 2);
    unsigned int ui = i;
    printBinary(ui >> 2);
    cout << "===i >> ===" << endl;

    // 负数 >>
    int j = -1;
    printBinary(j);
    printBinary(j >> 2);
    unsigned int uj = j;
    printBinary(uj >> 2);
    cout << "===j >> ===" << endl;

    // 乘除法
    int k = 10;
    cout << k << endl;
    cout << (k << 1) << endl;
    cout << (k << 2) << endl;
    cout << (k << 3) << endl;
    cout << (k >> 1) << endl;
    cout << (k >> 2) << endl;
    cout << (k >> 3) << endl;
    cout << "===k===" << endl;

    return 0;
}
```
