# 第 7 课：猜数字游戏
# 这个版本没有随机数，方便你看懂每一步。

secret = 7
guess = int(input("猜一个 1 到 10 的数字："))

if guess == secret:
    print("猜对了。")
elif guess < secret:
    print("太小了。")
else:
    print("太大了。")

print("答案是", secret)

# 试一试：
# 1. 把 secret 改成别的数字。
# 2. 在 if 那一行左边点红点，再点“调试”。
