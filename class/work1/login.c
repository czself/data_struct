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
			}
        	else
        	{
			if(strncmp(current_Word,pass_Word,Pcount)!=0||Pcount!=strlen(current_Word))
    		{
        		printf("密码错误,请重新输入\n");
    		}
            else
            {
                printf("登录成功！\n");
                return 0;
            }
        	}
        }
	}
}