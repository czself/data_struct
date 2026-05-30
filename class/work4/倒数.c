#include <stdio.h>
#include <stdlib.h>

int main() {
    int N;
    printf("请输入一个正整数N(>1): ");
    scanf("%d", &N);

    if (N <= 1) {
        printf("输入错误，N必须大于1\n");
        return 1;
    }

    // 使用数组存储小数部分的数字
    int digits[1000];
    int remainder = 1; // 初始余数为1
    int index = 0;
    int seen[1000] = {0}; // 记录每个余数出现的位置

    // 模拟除法过程
    while (remainder != 0) {
if (seen[remainder] != 0) {
            // 发现循环节
            int start = seen[remainder] - 1;
printf("[");
            for (int i = 0; i < start; i++) {
                printf("%d,", digits[i]);
            }
            printf("%d] %d\n", digits[start], start);
            return 0;
        }

seen[remainder] = index + 1;
        remainder *= 10;
        digits[index] = remainder / N;
        remainder = remainder % N;
        index++;
    }

    // 如果是有限小数
printf("[");
    for (int i = 0; i < index - 1; i++) {
        printf("%d,", digits[i]);
    }
    printf("%d],-1\n", digits[index - 1]);

    return 0;
}