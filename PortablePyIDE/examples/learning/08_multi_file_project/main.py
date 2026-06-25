# 第 8 课：多文件小项目
# main.py 是入口，helper.py 里放小工具函数。

from helper import add_badge, make_greeting


name = "小小程序员"

print(make_greeting(name))
print(add_badge(name, "认真学习徽章"))

# 试一试：
# 1. 打开 helper.py，看函数是怎么写的。
# 2. 在 make_greeting 上按 F12，跳到它的定义。
