# 第一次作业

## 第一大题

### 第一问

当我们编写一段C语言代码，在运行程序时，编译器先会生成一个exe文件，存储在硬盘中，然后再运行。

1. 编译器会生成一个obj文件，再链接成一个exe文件，存储在硬盘中。
2. 运行程序时，编译器会从硬盘中读取exe文件，加载到内存中。
   3.当cup要运行程序时，会先从缓存中寻找所要数据，如果缓存中没有找到就会从内存中寻找。

bus是总线，在电脑中所有的硬件都在总线上交流，总线分为四层：cpu内部总线，系统总线，主板拓展总线，外部低速总线
在程序运行时，cpu内部总线会从缓存中寻找数据，如果缓存中没有找到就会从系统总线中寻找。
系统总线会从内存中寻找数据，在系统总线中分为三类总线地址总线，控制总线，数据总线。地址总线会告诉内存我要哪一块地址，控制总线会告诉内存我要读写，数据总线会向内存发送数据。
在程序运行时，主板拓展总线用于从硬盘获取数据
外部低速总线用于连接键盘鼠标等usb硬件

### 第二问

### 第三问

## 第二大题

### 1.登录

```c
#include <stdio.h>
#include <string.h>
int main(int n,char**arr)
{
	char user_Name[] ="sz";
    char pass_Word []="12345678";
    char current_Name[20] ="";
    char current_Word [20]="";
    int Ncount = strlen(user_Name);
    int Pcount = strlen(pass_Word);
    if(n==2)
    {
        if(strcmp(arr[1], user_Name) != 0)
        {
            printf("用户名不存在，请重新输入\n");
		}
        else
        {
			printf("请输入用户密码\n");
    }
    }
    if(n==3)
    {
		if(strcmp(arr[1], user_Name) != 0)
        {
        	printf("用户名不存在，请重新输入\n");
    }
        else
        {
            if(strcmp(arr[2], pass_Word) != 0)
            {
                printf("密码错误，请重新输入\n");
			}
        }
	}
    if(n==1)
    {
        while(1)
        {
        	printf("请输入用户名和密码\n");
        	scanf("%s",current_Name);
        	scanf("%s",current_Word);
     		if(strncmp(current_Name,user_Name,Ncount)!=0||Ncount!=strlen(current_Name))
        	{
            	printf("用户名不存在，请重新输入\n");
                memset(current_Name,0,sizeof(current_Name));
			}
        	else
        	{
			if(strncmp(current_Word,pass_Word,Pcount)!=0||Pcount!=strlen(current_Word))
    		{
        		printf("密码错误,请重新输入\n");
                memset(current_Word,0,sizeof(current_Word));
    		}
            else
            {
                printf("登录成功！\n");
                return 0;
            }
        	}
        }

    }
    return 0;
}
```

### 2.编写一个函数，名字为isNumber`，参数为一个字符串，判断它是不是数字——不考虑科学计数法。

```c
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
```

### 3.编写一个函数，名字为 `integer`，参数为一个字符串，输出int型整数。

如输入“123”，输出 123

输入“-1123”，输出-1123

```c

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
```

### 4.编写一个函数，名字为 `Float`，参数为一个字符串，输出float型数。

```c
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
```

### 5.编写一个函数，名字为string2Number，参数为一个字符串，输出数字。

```c
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
```

### 6.编写一个函数，名字为 `number2String`，参数任意类型的数字，将其换成字符串。

如输入 1234.1，输出“1234.1”

```c
#include <stdio.h>
#include <string.h>
#include <math.h>
#define Max 20
char *numberString(float num);
int main()
{
    float number=0;
    char str[Max];
    printf("please enter a number\n");
    scanf("%f",&number);
    strncpy(str,numberString(number),Max);
    printf("%s",str);
	return 0;
}
char *numberString(float num)
{
    static char str[Max];
    int i = 0;
    if (isnan(num)) {
        strncpy(str, "nan", Max-1);
        str[Max-1] = '\0';
        return str;
    }
    if (num < 0.0f) {
        if (i < Max-1) str[i++] = '-';
        num = -num;
    }
    if (num == 0.0f) {
        if (i < Max-1) {
            str[i++] = '0';
        }
        str[i] = '\0';
        return str;
    }
    long long intPart = (long long)num;
    double frac = (double)num - (double)intPart;
    char tmp[Max];
    int ti = 0;
    if (intPart == 0) {
        tmp[ti++] = '0';
    } else {
        while (intPart > 0 && ti < Max-1) {
            tmp[ti++] = (char)('0' + (int)(intPart % 10));
            intPart /= 10;
        }
    }
    for (int k = ti - 1; k >= 0; k--) {
        if (i < Max - 1) str[i++] = tmp[k];
        else break;
    }
    double eps = 1e-9;
    int maxFracDigits = 6;
    if (frac > eps) {
        if (i < Max - 1) str[i++] = '.';
        for (int d = 0; d < maxFracDigits && frac > eps; d++) {
            frac *= 10.0;
            int digit = (int)frac;
            if (i < Max - 1) {
                str[i++] = (char)('0' + digit);
            } else {
                break;
            }
            frac -= digit;
        }
    }
    str[i] = '\0';
    return str;
}
```

### 7.编写一个函数，名字为stringCat,参数为两个字符串，返回的是这两个字符串相加后的新字符串。

如”123“，”ABC“，得到”123ABC“

```c
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
```
