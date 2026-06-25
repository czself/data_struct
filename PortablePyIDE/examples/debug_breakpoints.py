def make_star_line(count):
    line = "*" * count
    return line


def count_scores(scores):
    total = 0
    best = 0
    for score in scores:
        total = total + score
        if score > best:
            best = score
    return total, best


print("断点调试小练习开始")

numbers = [1, 2, 3, 4]
stars = []

for number in numbers:
    star_line = make_star_line(number)
    stars.append(star_line)
    print(star_line)

scores = [90, 75, 88]
total_score, best_score = count_scores(scores)
average = total_score / len(scores)

print("总分：", total_score)
print("最高分：", best_score)
print("平均分：", average)
print("断点调试小练习结束")
