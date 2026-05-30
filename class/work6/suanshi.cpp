#include <iostream>
#include <string>
using namespace std;
string trim (string demon);
bool judge_num(char c);
int main()
{
    string str ="1k+2f*7w-9d/d7-&*h";
    str=trim(str);
    cout<<str<<endl;
}
string trim (string demon)
{
    if(demon.empty())   return "";
   for (int i=0;i<demon.size();i++)
   {
       if(judge_num(demon[i]))
       {
           int pos=i+1;
           demon.erase(i,1);
           i--;//删除运算符后，i回退一位，以便检查下一个字符
       }
   }
    return demon;
}
bool judge_num(char c)
{
    return c== '+' || c == '-' || c == '*' || c == '/';
}