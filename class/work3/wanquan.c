#include <stdio.h>
int main()
{
    int n=10000;

    for (int o=1;o<n;o++)
    {
     	int idex=0;
        int sum=1;
        int arr[20]={};
        for (int i=2;i<o;i++)
    	{
        	if(o%i==0)
        	{
            	arr[idex]=i;
        		idex++;
        	}
    	}
    	for (int k=0;arr[k]!=0;k++)
    	{
    	     sum=arr[k]+sum;
    	}
    	if(sum==o)
    	{
    	     printf("is %d",o);
   	 	}    
    }
    return 0;
}