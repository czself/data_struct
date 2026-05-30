#include <bits/stdc++.h>
using namespace std;
int main()
{
    int i=-1;
    for (i=1;i<=3;i++)

    {
        if((i==1)+(i!=2)+(i!=1)==1)
        {
            if(i==1)
            cout <<"big";
            else if(i==2)
            cout <<"medium";
            else if(i==3)
            cout <<"small";
        }
    }
}