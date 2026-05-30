#include <stdio.h>
#include <math.h>
int main()
{
    int number;
    int num2=0;
    printf("enter a number");
    scanf("%d",&number);
    while (number%2!=1)
    {
        number/=2;
        num2++;
    }
    printf("have %d two",num2);
    for (int i=3;i<=sqrt(number);i=i+2)
    {
        while (number%i==0)
        {
            printf("%d",i);
            number/=i;
        }
        
    }
    if(number>1)
    {
        printf("%d",number);
    }
    return 0;
}