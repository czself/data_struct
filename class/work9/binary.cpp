#include <iostream>

using namespace std;
typedef struct binary_tree
{
    int data;
    char op;
    struct binary_tree *left;
    struct binary_tree *right;
} binary_tree;


binary_tree *xx(binary_tree *root ,binary_tree *node);
int main ()
{
    string ex="1+2*3-4";
    binary_tree*root =new binary_tree;
    root->data=0;
    root->op='#';
    root ->left=nullptr;
    root ->right=nullptr;
    for (int i=0;i<ex.size();i++)
    {
        if(ex[i]=='+'||ex[i]=='-'||ex[i]=='*'||ex[i]=='/')
        {
            root ->op=ex[i];
        }
        else
        {
            root ->data=ex[i]-'0';
        }
    }
    root =xx(root,root);
}