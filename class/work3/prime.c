#include <stdio.h>
#include <stdbool.h>
#include <math.h>
bool isPrime (int number);
int main ()
{
    int number;
    printf("enter a number");
    scanf("%d",&number);
    if(isPrime(number)) printf("is Prime ");
    else printf("is not prime");
    return 0;
}
bool isPrime(int number)
{
    if(number==2)
        return true;
    if(number<=1)
        return false;
    if(number%2==0)
        return false;
    for(int i=3;i<=sqrt(number);i=i+2)
    {
        if(number%i==0) return false;
    }
    return true;
}