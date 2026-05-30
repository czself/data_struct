#include <bits/stdc++.h>
using namespace std;
string trim (string str);
int main()
{
    string str="   hello world   ";
    cout<<trim(str)<<endl;
}
string trim (string str)
{
    if(str.empty())
    {
        return str;
    }
    int j=0;
    int start=-1;
    int end=-1;
    for (int i=0;i<str.size();i++)
    {
        if(str[i]==' ')
        {
            j=i;
        }
        else 
        {
            if(start==-1)
            {
                start=j+1;
            }
            end=i;
        }
    }
    return str.substr(start,end-start+1);
}
