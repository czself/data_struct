#include <iostream>
#include <string>

using namespace std;
struct binarytree {
    
    int data;
    char op;
    binarytree* left;
    binarytree* right;
};


binarytree* expressionToBinaryTree(const string& expression) {
   binarytree* root = nullptr;
   for (char c : expression) {
    binarytree* node = new binarytree();
    if(c=='+' || c=='-' || c=='*' || c=='/') {
        node->op=c;
        node->data=-1;
    }else{
        node->op='#';
        node->data=c-'0';
    }

    if(root==nullptr) {
        root=node;
        continue;
    }
    if(node->op=='+' || node->op=='-'){

        node->left=root;
        root=node;
        continue;
    }
    if(node->op=='*' || node->op=='/'){
        if(root->op=='+' || root->op=='-') 
        {
        node->left=root->right;
        root->right=node;
        continue;
        }
        node->left=root;
        root=node;
        continue;
    }
    if(node->op=='#'){
        binarytree* temp = root;
        while(temp->right != nullptr) {
            temp = temp->right;
        }
        temp->right = node;
        continue;
        }
    }
return root;
}


int evaluateExpression(binarytree* root) {
    if(root == nullptr) {
        return 0;
    }
    if(root->op == '#') {
        return root->data;
    }
    if(root->op == '+') {
        root->data = evaluateExpression(root->left) + evaluateExpression(root->right);
    }
    if(root->op == '-') {
        root->data = evaluateExpression(root->left) - evaluateExpression(root->right);
    }
    if(root->op == '*') {
        root->data = evaluateExpression(root->left) * evaluateExpression(root->right);
    }
    if(root->op == '/') {
        root->data = evaluateExpression(root->left) / evaluateExpression(root->right);
    }
    return root->data;
}

int main() {
    string ex="1+2*3-4";
    binarytree* root = expressionToBinaryTree(ex);
    cout<<evaluateExpression(root)<<endl;
    return 0;
}