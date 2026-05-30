#include <bits/stdc++.h>
using namespace std;
int main ()
{
    vector<int>arr={1,21,4,234,3,4,5654,76,543,73,4224,534524,3244,2};
    for (int i=0;i<arr.size();i++)
    {
        int minindex=i;
        for (int j=i+1;j<arr.size();j++)
        {
            if (arr[j]<arr[minindex])
            {
                minindex=j;
            }
        }
        swap(arr[i],arr[minindex]);
    }
    for (int i=0;i<arr.size();i++)
    {
        cout<<arr[i]<<" ";
    } 
}