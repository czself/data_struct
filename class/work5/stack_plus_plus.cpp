#include <iostream>
#include <stack>
using namespace std;
int became_number(char c);
bool limit (string str);
int main ()
{
    string str;
    cout<< "enter a string";
    cin >>str;
    if(limit(str))
    {
        cout <<"fuhe"<<endl;
    }
    else {cout <<"bufuhe";}
    return 0;
    
}
int became_number(char c)
{
    switch(c)
    {
        case '(' :return 3;
        case ')' :return 7;
        case '[' :return 2;
        case ']' :return 8;
        case '{' :return 1;
        case '}' :return 9;
    }
    return 0;
}
bool limit (string str)
{
    stack <int> s;
    for (int i=0;i<str.size();i++)
    {
        int n=became_number(str[i]);
        if(s.empty()&&n==1)
        {
            s.push(n);
            continue;
        }
        if(s.size()==1&&n==2)
        {
            s.push(n);
            continue;
        }
        if(s.size()==2&&n==3)
        {
            s.push(n);
            continue;
        }
        if(s.size()==3&&n==7)
        {
            if(s.top()+n==10)
            {
                 s.pop();
            	continue;
            }
        }
		if(s.size()==2&&n==8)
        {
            if(s.top()+n==10)
            {
                 s.pop();
            	continue;
            }
        }
        if(s.size()==1&&n==9)
        {
            if(s.top()+n==10)
            {
                 s.pop();
            	continue;
            }
    }
    if(s.empty())
    {
        return true ;
    }
    else return false;
}