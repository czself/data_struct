#include <stdio.h>
#include <stdlib.h>
typedef  struct people 
{
    int id;
    struct people* next;
}people;
int main ()
{
    people last = {5, NULL};    
    people node2 = {3, &last};  
    people node1 = {2, &node2};
    
    people frist= {1, &node1}; 
    people *head=&frist;
    people node3 = {4, NULL};   
    people *temp =&head;
    people *p=&head;
    while(1)
    {
        if(p->id>node3.id)
        {
            temp->next=&node3;
            node3.next=p;
            break;
        }
        else 
        {
            temp=p;
            p=p->next;
        }
    }
}
people *creat(int id)
{
    people *new=malloc(sizeof(people));
    new->id=id;
    new->next=NULL;
    return new;
}
people *read (people *head)
{
     while (head!=NULL)
     {
         printf("%d",head->id);
         head=head->next;
     }
}
void update(people *head,int oldid,int newid)
{
    while(head!=NULL)
    {
        if(head->id==oldid)
            {
                head->id=newid;
                return;
            }
            head=head->next;
    }
}
void delet(people **head,int id)
{
    if(*head=NULL)
    {
        printf("null");
        return ;
        
    }
    people *current=*head;
    people *pre=NULL;
	while(current != NULL && current->id != id) 
    {
        pre = current;
        current = current->next;
    }
    if(pre == NULL) {
        *head = current->next;  
    }
    else {
        pre     ->next = current->next; 
        
    }
    free(current);
    printf("成功删除 id = %d 的节点\n", id);
}