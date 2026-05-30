#include <stdio.h>
#include <string.h>
#define N 100
void getNext(int *next,char *Find);
int main ()
{
    char *Find="abcabcabc";
    int next[N];
    getNext(next,Find);
    for(int i=0;i<strlen(Find);i++)
    {
        printf("%d",next[i]);
    }
    return 0;
}
void getNext(int *next,char *Find)
{
    if(next==NULL||Find==NULL) return;
    int i=1;
    int j=0;
    next[0]=0;
    int lenFind=strlen(Find);
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
            if(j>0)
            {
                j=next[j-1];
            }
            else 
            {
                next[i]=0;
                i++;
            }
        }
    }
}