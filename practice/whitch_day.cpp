#include<bits/stdc++.h>
using namespace std;
void whichday(int year,int day);
int main ()
{
    int year=0;
    int day=0;
    scanf("%d",&year);
    scanf("%d",&day);
    whichday(year,day);
    return 0;
}
void whichday(int year,int day)
{
    int arr[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
 
    if(year%4==0)
    {
        arr[1]=29;
        if(day<=0||day>366) 
        {
            printf ("day is errror\n");
            return ;
        }
    } 
    else if(day<=0||day>=366) 
    {
        printf ("day is errror");
        return ;
    }
    for(int i=1;i<13;i++)
    {
        if(day<=0)
        {
            printf("%d-%d\n",i-1,day+arr[i-1]);
            return ;
        }
        day=day-arr[i];
    }
}