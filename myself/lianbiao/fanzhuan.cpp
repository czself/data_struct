#include <bits/stdc++.h>
using namespace std;
int main()
{

}
struct node
{
    int data;
    struct node *next;
};
class solution
{
    node *head;
    public:
    solution(node *head)
    {
        node *pre = NULL;
        node *next = NULL;
        while(head != NULL)
        {
            next =head ->next;
            head ->next = pre;
            pre = head;
            head = next;
        }
        head = pre;
    }
//     // 双链表节点
// struct node
// {
//     int data;
//     node *prev; // 多了一个前驱
//     node *next;
// };
    solution(node *head)
    {
        node *pre = NULL;
        node *next = NULL;
        while(head != NULL)
        {
            next = head->next;   // 先存下一个，跟你完全一样

            // // 就多这一行，改前驱
            // head->prev = next;

            // 下面两行和你单链表一模一样
            head->next = pre;
            pre = head;

            head = next;
        }
        head = pre;
    }
};
// 双链表 同风格反转
