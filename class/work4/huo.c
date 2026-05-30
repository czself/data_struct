/*struct tm	存储年、月、日、时、分、秒的结构体
/*time(&)	获取当前时间戳（秒数）
/*localtime()	把时间戳转成 struct tm 格式
/*通常用time_t存放秒数
/*mktime()	把 struct tm 转成时间戳
/*difftime()	计算两个时间戳的差值（秒）
    \\struct tm {
    \\int tm_sec;   // 秒
    \\int tm_min;   // 分
    \\int tm_hour;  // 时
    \\int tm_mday;  // 日 ← 注意是 mday，不是 day
    \\int tm_mon;   // 月 ← 注意是 mon，不是 month
    \\int tm_year;  // 年
    \\int tm_wday;  // 星期几
    \\int tm_yday;  // 年内第几天
    \\int tm_isdst; // 夏令时
};
*/
#include <stdio.h>
#include <time.h>
int main ()
{
    struct tm birth={0};
    int year,month,day ;
    printf("enter your birthday");
    scanf ("%d %d %d",&year,&month,&day);
    birth.tm_year=year-1900;
    birth.tm_mon=month-1;
    birth.tm_mday=day;
    time_t now=time(NULL);
    time_t tmd =mktime(&birth);
    time_t s=now-tmd ;
    int days = (int)(s / (60 * 60 * 24));
    if(days<0) return 0;
    printf ("%d",days);
}