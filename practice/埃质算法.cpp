#include <bits/stdc++.h>
using namespace std ;
vector<int> isprime(vector<int> &arr);
int main()
{
    vector<int> arr={};
    for (auto bit:isprime(arr))
    {
        cout <<bit<<endl;
    }
}
vector<int> isprime(vector<int> &arr)
{
    for (int i=0;i<99;i++)
    {
        arr.push_back(i+2);
    }
    for (int j=0;j<sqrt(arr.size());j++)
    {
        for (int l=j+1;l<arr.size();l++)
        {
            if(arr[l]%arr[j]==0)
            {
                arr.erase(arr.begin()+l);
                l--;
            } 
        }
    }
    return arr;
}
