/*#include <stdio.h>
int main ()
{
    long long int number=0;
    printf("enter a number\n");
    scanf("%lld",&number);
    while (number!=123)
    {
        int odd=0;
    	int even=0;
    	int count=0;
        while (number!=0)
    	{
       		if((number%10)%2==0) even++;
       		else odd++;
       		number/=10;
       		count++;
    	}
    	//printf("have %d even,%d odd ,%d count\n",even,odd ,count);
     	//number=even*100+odd*10+count;
    	printf("number=%d\n",number);
    }
    printf("%d",number);
    return 0;
}
*/


#include <stdio.h>
#include <string.h>
#define Max 100
void turn(char*current,char*next);
int main()
{
    char next[Max]="";
    char number[Max]="";
    printf("enter a number");
    scanf("%s",number);
    while(1)
    {
        if(strcmp(number,"123")==0) return 0;
        else
        {
            turn (number,next);
        	printf("%s,%s\n",number,next);
        	strcpy(number,next);
        }
    }
    printf("%s,stop",number);
    return 0;
}
void turn (char *current ,char *next)
{
    int even=0;
    int odd=0;
    int len=strlen(current);
    for (int i=0;i<len;i++)
    {
        int number=current[i]-'0';
        if(number%2==0) even++;
        else odd++;
    }
    sprintf(next,"%d%d%d",even,odd,len);
}