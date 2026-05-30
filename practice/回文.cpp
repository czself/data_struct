#include <bits/stdc++.h>
using namespace std;
bool isPalindrom (string str);
int main (){
    string str ="adsa";
    if(isPalindrom(str))
    printf("is palindeom\n");
    else
    printf ("is not \n");
}
bool isPalindrom (string str)
{
    int i=0;
    for (i;i<(str.size()>>1);i++)
    {
        if (str[i]!=str[str.size()-1-i])
        return false;
    }
    return true;
}