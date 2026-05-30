#include <bits/stdc++.h>
using namespace std;
int main ()
{
    vector <int>arr ={123,4234,34214,214,64652,4131,2123,134,44,214,2,345,23,4,24,12,4123,124};
    sort(arr.begin(),arr.end());
    int r=arr.size()-1;int l=0;
    while(l<=r)
    {
        int mid=(l+r)>>1;
        if(arr[mid]==123)
        {
            cout<<"find it "<<mid<<endl;
            return 0;
        }
        else if(arr[mid]>123)
        {
            r=mid-1;
        }
        else
        {
            l=mid+1;
        }
    }  
    cout<<"not find"<<endl;
}






class Solution {
public:
    int search_max_position(const vector<int>& arr)
    {
        int ans=-1;
        int l=0;
        int r=arr.size()-1;
        while(l<=r)
        {
            int mid=(l+r)>>1;
            //int mid=l+(r-l)/2;这样防止溢出
            if(arr[mid]>=123)
            {
                ans=mid;
                r=mid-1;
            }
            else
            {
                l=mid+1;
            }
        }
        return ans;
    }
};



