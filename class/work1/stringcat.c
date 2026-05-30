// #include <stdio.h>
// #include <string.h>
// char *stringcat(char *str1,char *str2);
// int main()
// {
//     char str1[100]="";
//     char str2[100]="";
//     char str[100]="";
//     printf("enter two strings:");
//     scanf("%s",str1);
//     scanf("%s",str2);
//     strcpy(str,stringcat(str1,str2));
//     printf("%s",str);
// }
// char *stringcat(char *str1,char *str2)
// {
    
//     static char str[100]="";
//     int i1=0;
//     int i2=0;
//     if(str1==NULL&&str2==NULL) return NULL;
//     while(str1[i1]!='\0')
//     {
//         str[i1]=str1[i1];
//         i1++;
//     }
//     while(str2[i2]!='\0')
//     {
//         str[i1+i2]=str2[i2];
//         i2++;
//     }
//     str[i1+i2]='\0';
//     return str;
// }
// #include <stdio.h>
// #include <string.h>
// char *stringcat(char *str1,char *str2);
// int main()
// {
//     char str1[100]="";
//     char str2[100]="";
//     char str[100]="";
//     strcpy(str,stringcat(str1,str2));
//     printf("%s",str);
// }
#include <stdio.h>
#include <string.h>
char *stringcat(char *str1,char *str2);
int main()
{
    char str1[100]="";
    char str2[100]="";
    char str[100]="";
    printf("enter two strings:\n");
    scanf("%s",str1);
    scanf("%s",str2);
    strcpy(str,stringcat(str1,str2));
    printf("%s\n",str);
}char *stringcat(char *str1,char *str2)
{
    
    static char str[20]="";
    int i1=0;
    int i2=0;
    if(str1==NULL ||str2==NULL) return NULL;
    while(str1[i1]!='\0')
    {
        i1++;
    }
    while(str2[i2]!='\0')
    {
        i2++;
    }
    int i=i1;
    int k=0;
    strncpy(str,str1,i1);
    for (;i<i1+i2;i++)
    {
        str[i]=str2[k];
        k++;
    }
    str[i]='\0';
    return str;
}