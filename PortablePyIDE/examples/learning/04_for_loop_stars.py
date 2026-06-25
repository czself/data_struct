# 第 4 课：重复做事情
# for 可以让程序重复执行几次。

for number in range(1, 6):
    print("第", number, "次：你好 Python")

print()
print("下面画星星楼梯：")

for count in range(1, 7):
    line = "*" * count
    print(line)

# 试一试：
# 1. 把 range(1, 6) 改成 range(1, 11)。
# 2. 把 "*" 改成 "#" 或 "o"。
