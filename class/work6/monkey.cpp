//五只猴子分别带了8个桃子参加聚会。它们围成一个圆，第一只猴子将自己的桃子分
//成3分，其中一份和
//余下的留个自己，另外两份分给相邻的两个猴子（第二个和第五个）。分完后，其他
//猴子也做了和第一
//只猴子同样的事，当第五只猴子分完后，每只猴子各有多少个桃子？


#include <iostream>
int push(int arr[],int head,int data,int tail);
int pop(int arr[],int head,int tail);
const int MAX= 6;
int arr[MAX];
int main()
{
    int head=0,tail=0;
    tail=push(arr,head,0,tail);
    tail=push(arr,head,1,tail);
    tail=push(arr,head,2,tail);
    tail=push(arr,head,3,tail);
    tail=push(arr,head,4,tail);
    int peach[5]={8,8,8,8,8};
    for(int i=0;i<5;i++)
    {
        int idex=arr[head%MAX];
        int fen =peach[idex]/3;
        int yu=peach[idex]%3;
        peach[idex]=yu+fen;
        head=pop(arr,head,tail);
        int left=(idex+4)%5;
        int right=(idex+1)%5;
        peach[left]+=fen;
        peach[right]+=fen;
        tail=push(arr,head,idex,tail);
    }
    for(int i=0;i<5;i++)
    {
        std::cout<<peach[i]<<std::endl;
    }
    return 0;
}
int push(int arr[],int head,int data,int tail)
{
    if(tail-head>=MAX)   return -1;
    else 
    {
        arr[tail%MAX]=data;
        return tail+1;
    }
}
int pop(int arr[],int head,int tail)
{
    if(head==tail) return -1;
    else return head+1;
}
