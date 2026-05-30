#include <stdio.h>
#include <time.h>
int main()
{
    struct tm dayfrist={0};
    int count=0;
    for (int year=49;year<125;year++)
    {
        dayfrist.tm_year=year;
        dayfrist.tm_mon=9;
        dayfrist.tm_mday=1;
        mktime(&dayfrist);//用来更新用的
        if(dayfrist.tm_wday==0)
        {
            count++;
        }
    }
     printf("All in %d",count);
    return 0;
}