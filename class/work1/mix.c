#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#define Max 20
float stringNumber(char *str);
int main()
{
    char cstr[Max];
    int dotnum=0;
    printf("please enter a number \n");
    scanf("%s",cstr);
    // 检查是否包含小数点
    bool hasDot = false;
    for(int i=0;cstr[i]!='\0';i++)
    {
        if(cstr[i]=='.')
        {
            hasDot = true;
            break;  // 找到小数点就可以退出了
        }
    }
    
    float result = stringNumber(cstr);
    if(!hasDot)
    {
        printf("%d\n", (int)result);  // 整数格式，不显示小数
    }
    else
    {
        printf("%.6f\n", result);  // 浮点格式，显示小数
    }
    return 0;
}
float stringNumber(char *str)
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