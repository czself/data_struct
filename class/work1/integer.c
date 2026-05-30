#include <stdio.h>
#include <string.h>
#define Max 20 
int integer(char *str);
int main()
{
    char cha[Max];
    printf("tell a character\n");
    scanf("%s",cha);
    int Number=integer(cha);
    printf("%d",Number);
    return 0;
}
int integer (char *str)
{
    int i=0;
    int number=0;
    int sign=1;
    if(str==NULL||strlen(str)==0) return 0;
    if(str[i]=='+')
    {
        sign=1;
    	i++;
    }
    if(str[i]=='-')
    {
        sign=-1;
        i++;
    }
    for (;str[i]!='\0';i++)
    {
        if(str[i]<'0'||str[i]>'9') return 0;
        number=number*10+(str[i]-'0');
    }
    number=number*sign;
    return number;
}