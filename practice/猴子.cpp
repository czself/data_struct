#include <bits/stdc++.h>
using namespace std;
int main ()
{
    for (int i=6;i<4000;i++)
    {            
        int n=i;
        bool flag=true;
        for (int j=0;j<5;j++)
        {
            if(n%5!=1)
            {
                
                flag=false;
                break;  
            }
            else{
            n=n-1;
            n=n/5*4;
            }
        }
        if(flag)
            {
               cout <<i<<endl;
            }
    }
  return 0;
}