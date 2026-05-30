#include <stdio.h>
#include <string.h>
#define MAX 40
int main()
{
    char Source [MAX]="data structure ,please find data";
    char Find [MAX]="data";
    int i=0;
    for (;i<=strlen(Source)-strlen(Find);i++)
    {
        int j=0;
        for (;j<strlen(Find);j++)
        {
            if(Find[j]!=Source[i+j])
            {
                break;
            }
        }
             if(j==strlen(Find))
            {
            	printf("index is%d",i);
            }
    }
    printf("weizhapdao\n");
}