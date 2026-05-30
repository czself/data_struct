#include<bits/stdc++.h>
using namespace std;
bool isBracketRight(string str);
int main ()
{
    string str="   ()()()()(  )";
    cout << isBracketRight(str) << endl;
}
bool isBracketRight(string str)
{
    if(str.empty()) return false;
    stack <char> small ={};
    for (int i=0;i<str.size();i++)

    {
        if(str[i]=='(')
        small.push('(');
        else if(str[i]==')')
        {
            if(!small.empty())
            {
                small.pop();
            }
            else return false;
        }
        else 
        continue;
    }
    if(small.empty())
    {return true;}
    else {return false;}
}