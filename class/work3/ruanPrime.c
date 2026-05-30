#include <stdio.h>
#include <stdbool.h>
#include <math.h>
bool isPrime (int number);
int main ()
{
    int number1;
    int number2;
    for (number1=1;number1<98;number1++)
    {
        number2=number1+2;
        if((isPrime(number1)&&isPrime(number2))&&(number2-number1==2))
            printf("%d %d is ruan Prime\n",number1,number2);
    	// else printf("%d %d is not ruan Prime\n",number1,number2);
    }
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