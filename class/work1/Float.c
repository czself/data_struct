#include <stdio.h>
#include <string.h>
#define Max 20
float Float (char *str);
int main()
{
    char cha[Max];
    printf("please enter a float number\n");
    scanf("%s",cha);
    float number=Float(cha);
    printf("%f",number) ;
    return 0;
}
float  Float(char *str)
{
    int n=0;
    int sign=1;
    int zhenshu=0;
    float xiaoshu=0;
    int dot=0;
    float dotplace=0.1;
    if(str==NULL||strlen(str)==0)  return 0;
    if(str[n]=='+')
    {
        n++;
        sign=1;
    }
    if(str[n]=='-')
    {
        n++;
        sign=-1;
    }
    for (;str[n]!='\0';n++)
    {
        
        if(str[n]!='.')
        {
            if(str[n]<'0'||str[n]>'9') return 0;
            zhenshu=zhenshu*10+(str[n]-'0');
        }
        else
        {
            n++;            
            dot++;
            if(dot==1)
            {
                if(str[n]=='\0') return 0;
                for (;str[n]!='\0';n++)
                     {
                        if(str[n]<'0'||str[n]>'9') return 0;
                         xiaoshu=xiaoshu+(str[n]-'0')*dotplace;
                         dotplace=dotplace*0.1;
                     }
                return sign*(zhenshu+xiaoshu);
            }
            if(dot>=2)
            {
                return 0;
            }
        }
    }
    return sign*zhenshu;
}