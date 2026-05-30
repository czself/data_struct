#include <stdio.h>
int main ()
{
    for (int big=0;big<=1;big++)
    for (int second=0;second<=1;second++)
    for (int third=0;third<=1;third++)
    {
        if(big+second+third==1&&((big==1)+(second==0)+(big==0)==1))
           {
               printf("%s",big?"big":second?"second":"third");
           }
    }
           return 0;
}