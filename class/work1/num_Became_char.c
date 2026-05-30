#include <stdio.h>
#include <string.h>
#include <math.h>
#define Max 20
char *numberString(float num);
int main()
{
    float number=0;
    char str[Max];
    printf("please enter a number\n");
    scanf("%f",&number);
    strncpy(str,numberString(number),Max);
    printf("%s",str);
	return 0;
}
char *numberString(float num)
{
    static char str[Max];
    int i = 0;
    if (isnan(num)) {
        strncpy(str, "nan", Max-1);
        str[Max-1] = '\0';
        return str;
    }
    if (num < 0.0f) {
        if (i < Max-1) str[i++] = '-';
        num = -num;
    }
    if (num == 0.0f) {
        if (i < Max-1) {
            str[i++] = '0';
        }
        str[i] = '\0';
        return str;
    }
    long long intPart = (long long)num;
    double frac = (double)num - (double)intPart;
    char tmp[Max];
    int ti = 0;
    if (intPart == 0) {
        tmp[ti++] = '0';
    } else {
        while (intPart > 0 && ti < Max-1) {
            tmp[ti++] = (char)('0' + (int)(intPart % 10));
            intPart /= 10;
        }
    }
    for (int k = ti - 1; k >= 0; k--) {
        if (i < Max - 1) str[i++] = tmp[k];
        else break;
    }
    double eps = 1e-9;
    int maxFracDigits = 6;
    if (frac > eps) {
        if (i < Max - 1) str[i++] = '.';
        for (int d = 0; d < maxFracDigits && frac > eps; d++) {
            frac *= 10.0;
            int digit = (int)frac;
            if (i < Max - 1) {
                str[i++] = (char)('0' + digit);
            } else {
                break;
            }
            frac -= digit;
        }
    }
    str[i] = '\0';
    return str;
}