secret = 7

print("我心里有一个 1 到 10 的数字。")
answer = int(input("你猜是多少？"))

if answer == secret:
    print("猜对了！")
elif answer < secret:
    print("小了一点。")
else:
    print("大了一点。")
