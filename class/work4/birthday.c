#include <stdio.h>
#include <time.h>
int main ()
{
    struct tm birth={0};
    int month,day;
    printf("enter your friend birthday");
    scanf("%d %d",&month,&day);
    birth.tm_mon=month-1;
    birth.tm_mday=day;
    time_t now=time(NULL);
    struct tm *years =localtime (&now);
    birth.tm_year=years->tm_year;
    time_t birth_s=mktime(&birth);
    time_t s=now-birth_s;
    int days=(int)(s/(60*60*24));
    if(days <0)
    {
        days=days+365;
    }
    printf("%d",days);
    return 0;
}