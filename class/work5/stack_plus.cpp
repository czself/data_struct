#include <string>
#include <iostream>
#include <stack>
using namespace std;
int became_number (char c);
bool judge (string str);
int main ()
{
    string str ="(({{[[]))";
    if(judge(str))
    cout <<"balanced"<<endl;
    
    else cout <<"not balanced"<<endl;
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
bool judge (string str)
{
    stack <int> s;
    for (int i=0;i<str.size();i++)
    {
        int n=became_number(str[i]);
      	if(n==0)
        {
            continue ;
        }
        if(n<5)
        {
            s.push(n);
            continue;
        }
        if(n>5)
        {
            if(s.top()+n==10)
            {
                s.pop();
                cout <<"    balanced"<<endl;
                continue;
            }
            else 
            {
                cout <<"    not balanced"<<endl;
                return false;
            }
        }
    }
    if(s.empty())
	return true;
    else return false;
}