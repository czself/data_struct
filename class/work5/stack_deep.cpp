#include <iostream>
#include <string>
#include <stack>
#include <vector>
using namespace std;
int judge(string str);
int main ()
{
    string str="(())(((())))()";
    if (judge(str) == -1)
    {
        cout <<"it is not balanced "<<endl;
        return 0;
    }
    cout <<"deepth:"<<judge(str)<<endl;
    return 0;
}
int  judge (string str)
{
    stack<char> s;
    stack<int> deep;
     if(str.empty()) 
    {
        cout<<"the string is empty"<<"   "<<endl;
        return -1;
    }
    for (int i=0;i<str.size();i++)
    {
       
        if(str[i]=='(')
        {
            if(s.empty())
            {
                deep.push(0);
            }
               s.push(str[i]);
        }
        if(str[i]==')')
        {
            if(s.empty())
            {
                cout<<"error"<<endl;
                return -1;
            }
            s.pop();
            if(deep.empty())
                deep.push(1);
            else
				deep.top()++;
        }
    }
    if(s.empty())
    {
        int maxnumber=0;
        while(!deep.empty())
        {
            if(deep.top()>maxnumber)
            {
                maxnumber=deep.top();
            }
            deep.pop();
        }
        return  maxnumber;
    }
    else 
    {
        return -1;
    }
}