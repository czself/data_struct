#include <bits/stdc++.h>
using namespace std;


int main ()
{
    vector<int>arr ={21,31,4124,12,32,4,142,414,24,4,14,12};
    int i=arr.size()-1;
    int j=0;
    int ans=0;
    int m=0;
    while (j<=i)
    {
        if(arr[i]>arr[i-1])
        {
            ans=i;
            cout<<ans<<" is top";
            return 0;
        }
        if(arr[j]>arr[j+1])
            {
                ans=j;
                cout<<ans<<" is top";
                return 0;
            }
        i--;
        j++;
        int m=(i+j)>>1;
        if(arr[m]<arr[m+1])
        {
            m=(i+m)>>1;
        }
        else if(arr[m]<arr[m-1])
        {
            m=(j+m)>>1;
        }
        else 
        {
            ans=m;
            cout<<ans<<" is top";
            return 0;
        }
    }
}
