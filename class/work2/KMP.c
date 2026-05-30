#include <stdio.h>
#include <string.h>
#define N 100
#define MAX 20
void getNext(int *next,char *Find);
int main()
{
    char Source[MAX]="";
    char Find[MAX]="";
    int next[N];
    printf("please enter Source and Find\n");
    scanf("%s",Source);
    scanf("%s",Find);
    getNext(next,Find);
    int i=0;
    int j=0;
    while(i<strlen(Source))
    {
        if(Source[i]==Find[j])
        {
            i++;
            j++;
        }
        else
        {
            if(i==0)
            {
                i++;
            }
            else
            {
                j=next[j-1];
            }
        }
        if(j==strlen(Find))
        {
            printf("index is %d\n",i-j);
            j=next[j-1];
        }
    }
    return 0;
}
void getNext(int *next,char *Find)
{
    int lenFind=strlen(Find);
    int i=1;
    int j=0;
    next[0]=0;
    if(Find==NULL||next==NULL) return ;
    while(i<lenFind)
    {
        if(Find[i]==Find[j])
        {
            j++;
            next[i]=j;
            i++;
        }
        else
        {
            if(j==0)
            {
                next[i]=0;
                i++;
            }
            else
            {
                j=next[j-1];
            }
        }
    }
}