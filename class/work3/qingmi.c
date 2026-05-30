// #include <stdio.h>
// #define Max 1000
// int main()
// {
//     int o=10000;
//     for (int i=1;i<o;i++)
//     {
//             int midex=0;
//             int marr[Max]={0};
//         for (int k=1;k<i;k++)
//         {
//             if(i%k==0)
//             {
//                 marr[midex]=k;
//                 midex++;
//             }
//         }
//         for (int p=1;p<i;p++)
//         {
//                  int nidex=0;
//     			int narr[Max]={0};
//             for (int y=1;y<p;y++)
//             {
//                 if(p%y==0)
//                 {
//                     narr[nidex]=y;
//                     nidex++;
//                 }
//             }
//             int msun=0;
//             int nsum=0;
//              for (int u=0;narr[u]!=0||marr[u]!=0;u++)
//         	{
//                  msun+=marr[u];
//                  nsum+=narr[u];
//             }   
//             if(msun==p&&nsum==i)
//                  {
//                      printf("find it %d and %d\n",i,p);
//                      break;
//                  }
//         }
//     }
//     return 0;
// }

#include <stdio.h>
#define Max 10000
int main ()
{
    int o=10000;
    int sum[Max+1]={0};
    for (int i=1;i<=o;i++)
    {
        for (int j=1;j<i;j++)
        {
            if(i%j==0)
            {
                sum[i]+=j;
            }
        }
    }
    for (int k=1;k<=Max;k++)
    {
        int b=sum[k];
        if(b<Max&&b>k&&k==sum[b])//必须先声明b在一定范围，不然会越界，无法得出正确答案
        {
            printf("%d and %d\n",b,k);
        }
    }
    return 0;
}