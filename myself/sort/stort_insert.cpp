#include <bits/stdc++.h>
using namespace std;
int main ()
{
    vector <int>arr ={123,4234,34214,214,64652,4131,2123,134,44,214,2,345,23,4,24,12,4123,124};
    for (int i=1;i<arr.size();i++)
    {
        for (int j=i-1;j>=0;j--)
        {
            if(arr[j]>arr[j+1])
            {
                swap(arr[j],arr[j+1]);
            }
        }
    }
    for (int i=0;i<arr.size();i++)
    {
        cout << arr[i] << " ";
    }
    return 0;
}