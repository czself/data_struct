#include <iostream>
using namespace std;
void quick_sort (int q[],int l,int r)
{
    if(l>=r)  return ;
    int x=q[l+r>>1];//等于(l+r)/2
    int i=l-1,j=r+1;
    while (i<j)
    {
        do i++; while (q[i]<x);
        do j--; while (q[j]>x);
        if(i<j)
        {
            int t=q[i];
            q[i]=q[j];
            q[j]=t;
        }
    }
    quick_sort(q,l,j);
    quick_sort(q,j+1,r);    

}
const int N = 1e6 + 10;
int q[N];
int main()
{
    
    int n=0;
    cin >> n;
    for (int i = 0; i < n; i ++ ) cin >> q[i];

    quick_sort(q, 0, n - 1);

    for (int i = 0; i < n; i ++ ) cout << q[i] << " ";
    return 0;
}