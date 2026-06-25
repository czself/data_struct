# 第 5 课：列表小背包
# list 像一个小背包，可以装很多东西。

backpack = ["铅笔", "橡皮", "练习本"]

print("我的背包里有：")

for item in backpack:
    print("-", item)

backpack.append("水杯")

print()
print("放入水杯以后：")

for item in backpack:
    print("-", item)

print()
print("一共有", len(backpack), "样东西。")

# 试一试：
# 1. 在 backpack 里加一个你喜欢的东西。
# 2. 把 append("水杯") 改成 append("课外书")。
