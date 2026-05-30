#include <iostream>
#include <string>
#include <stack>
using namespace std;
bool judge (string s);
int main()
{
    string s="";
    if(judge(s))
        cout<<"the string is match"<<endl;
    else 
        cout <<"the string is not match"<<endl;
    return 0;
}
bool judge (string s )
{
     if(s.empty()) 
    {
        cout<<"the string is empty"<<"   "<<endl;
        return false;
    }
    stack<char> str;
    for (int i=0;i<s.size();i++)
    {
       
        if(s[i]=='(')
            str.push(s[i]);
        if(s[i]==')')
        {
            if(str.empty())
            {
                cout<<"error"<<endl;
                return false;
            }
            str.pop();
        }
    }
    if(str.empty())
    {
        return true ;
    }
    else 
    {
        return false;
    }
}