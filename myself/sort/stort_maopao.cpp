#include <bits/stdc++.h>
using namespace std ;
int main()
{
    vector<int>arr={12,31,23,12,14,2,4141,24,1,241,12412};
    for (int i=arr.size()-1;i>0;i--)
    {
        for (int j=0;j<i;j++)
        {
            if(arr[j]>arr[j+1])
            {
                swap(arr[j],arr[j+1]);
            }
        }
    }
    for (int i=0;i<arr.size();i++)
    {
        cout<<arr[i]<<" ";
    }
}