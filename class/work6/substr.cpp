#include <iostream>
#include <string>

using namespace std;
string sub(string domain,string model);
int main()
{
    string domain,model;
    cout<<"Enter domain name: ";
    cin>>domain;
    cout<<"Enter model name: ";
    cin>>model;
    cout<<"Substring: "<<sub(domain,model);
    return 0;
} 
string sub(string domain,string model)
{
    string result="";
    if(domain.size()==0||model.size()==0) return "";
    while(1)
    {
        int pos=domain.find(model);
        if(pos==string::npos) return result;
        result+=domain.substr(0,pos);
        domain.erase(0,pos+model.size());
    }
    return result;
}