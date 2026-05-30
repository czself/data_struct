//~用法



#include <bits/stdc++.h>
using namespace std;

int main ()
{
    int arr[14]={100,100,100,100,200,100,300,300,300,300,400,400,200,200};
    int num=0;
    for(int i=0;i<14;i++)
    {
        num=arr[i]^num;
    }
    int number=(~num+1)&num;//获取从右往左第一个非0的数
    int onlylumber=0;
    for (int i=0;i<14;i++)
    {
        if(arr[i]&number)
        {
            onlylumber^=arr[i];
        }
    }
    cout << onlylumber << " " << (onlylumber ^ num) << endl;
    return 0;
}





class binary_system
{
public:
    public : void print(int num)
    {
        for (int i=31;i>=0;i--)
        {
            cout <<((num>>i)&1);
            cout << bitset<32>(num)<<endl;
        }
    }
};
