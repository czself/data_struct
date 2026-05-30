#include <stdio.h>
#include <math.h>
#include <string.h>
#define MAX 1000
int main ()
{
    int n;
    printf("Enter a number: ");
    scanf("%d", &n);
    int isprime[MAX];
    memset(isprime, 1, sizeof(isprime));
    isprime[0] = 0;
    isprime[1] = 0;
    for (int i=2;i<sqrt(n);i++)
    {
        if(isprime[i])
        {
            for (int j=i*i;j<=n;j+=i)
            {
                isprime[j]=0;
            }
        }
    }
    for(int io=0;io<=n;io++)
    {
        if(isprime[io])
        {
            printf("%d ", io);
        }
    }

    return 0;
}