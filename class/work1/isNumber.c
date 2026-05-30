#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#define Max 20
bool isNumber (char *str);
int main ()
{
    char number[Max]="";
    printf("Please enter a number: ");
    scanf("%s",number);

    if(isNumber(number)==true)
    {
        printf("is number");
    }
    else 
    {
        printf("is not number");
    }
    return 0;
    
}
bool isNumber (char *str)
{
    int i=0;
    int dot =0;
    if(str == NULL || strlen(str) == 0) return false;
    if(str[i]=='-' || str[i]=='+') i++;
    if(str[i]=='\0')return false;
    while(str[i]!='\0')
    {
        if(str[i]=='.')
        {
            dot++;
            if(dot==1)
            {
                i++;
            	if(str[i]=='\0')return false;
            	for(int j=i;str[j]!='\0';j++)
                {
                    if(str[j]<'0'||str[j]>'9')return false;
                }
            }
            else return false;
            i++;
        }
        else 
        {
            if(str[i]<'0'||str[i]>'9')return false; 
            i++;
        }
    }
    return true;
} 