# 第 6 课：函数小机器
# 函数像一台小机器：放进去材料，得到一个结果。

def make_card(name, animal):
    card = "我是" + name + "，我喜欢" + animal + "。"
    return card


def add_points(old_points, new_points):
    return old_points + new_points


message = make_card("小明", "熊猫")
print(message)

points = 10
points = add_points(points, 5)
points = add_points(points, 8)

print("现在有", points, "分。")

# 试一试：
# 1. 改改 make_card 里的名字和动物。
# 2. 改改加分数字，看看 points 怎么变。
