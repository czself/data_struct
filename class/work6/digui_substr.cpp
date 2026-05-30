#include <iostream>
#include <string>
using namespace std;
string sub (string demon,string model);
int main()
{
    string demon="***sss***ttt***uuu***";
    string result =sub(demon,"***");
    cout <<result<<endl;
}
string sub (string demon,string model)
{
    if(demon.size()==0||model.size()==0) return "";
        int pos=demon.find(model);
        if(pos==string::npos)  return demon;
        demon.erase(pos, model.size());
		demon=sub(demon,model);
 		return demon;
}