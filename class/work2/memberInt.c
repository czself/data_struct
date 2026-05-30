#include <stdio.h>
int memoryInt(int number);
int main()
{
    int number;
    printf("enter a number");
    scanf("%d",&number);
    memoryInt(number);
    return 0;
}
int memoryInt(int number)
{
    char *p=(char*)&number;
    printf("[");
    for(int i=0;i<4;i++)
    {
       printf("0x%02X",p[i]);
        if(i!=3)
        {
            printf(",");
        }
    }
    printf("]");
}