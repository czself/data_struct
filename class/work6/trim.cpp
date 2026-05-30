#include <iostream>
#include <string>
using namespace std;
string trim(string demon);
int main ()
{
    string str="    hello word    ";
    str=trim(str);
    cout <<str<<endl;
}
string trim(string demon)
{
    if(demon.empty())   return "";
    int end=-1,start=-1;
    for (int i=0;i<demon.size();i++)
    {
        if(demon[i]==' ')  continue;
        if(start==-1)
        {
            start=i;
        }
        else start=start;
        end=i;
    }
    return demon.substr(start, end - start + 1);;
}
