#include <bits/stdc++.h>
using namespace std;
vector<int> memoryInt(int mun);
int main()
{
    int num;
    scanf("%d",&num);

    for (auto bit :memoryInt(num))
    {
        printf ("%d ",bit);
    }
}
vector<int>memoryInt(int num)
{
    vector<int> arr ={};
    int i=31;
    for (i;i>=0;i--)
    {
        (num&(1<<i))==0?arr.push_back(0):arr.push_back(1);
    }
    return arr;
}