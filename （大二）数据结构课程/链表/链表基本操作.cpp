#include <stdio.h>
#include <stdlib.h>
#include<malloc.h>
#define list Node*
typedef struct Node
{
	int data;
    struct Node *next;
}Node;

Node *create()
{
    Node *p=NULL;
    int num;
    printf("���������ݣ�\n");
    scanf("%d",&num);
	p=(Node*)malloc(sizeof(Node));
    p->data=num;
    p->next=NULL;
    return p;
}

Node* buildlist()
{
	Node *head=NULL,*p=NULL,*r=NULL;
    int num;
    printf("����0��ʼ�ͼ�������0����\n");
    scanf("%d",&num);
    while(num==0)
    {
		p=create();
		if(head==NULL)
    	{
		head=p;
        r=head;
    	}
   		else 
    	{
		r->next=p;
        r=p;
    	}
    	scanf("%d",&num);
    }
    
    return head;
}
void print(Node *head)
{
	Node *p=NULL;
    p=head;
    while(p!=NULL)
    {
		printf("%d ",p->data);
        p=p->next;
    }
    printf("\n");
}

void release(list *head)
{
	Node *p=*head;
	while(*head)
	{
		p=(*head)->next;
		free(*head);
		*head=p;
	}
}

Node *insert(Node *head,int num)
{
	Node *p1=NULL,*p2=NULL,*p=NULL;
	int x;
    p=(Node *)malloc(sizeof(Node));
    printf("��������ǣ�\n"); 
    scanf("%d",&x);
    p->data=x;
    p->next=NULL;
    p1=head;
    while(p1->data<num)
    {
		p2=p1;
        p1=p1->next;
    }
    if(p1==head)
    {
		p->next=p1;
        head=p;
    }
    else
	{
		p2->next=p;
        p->next=p1;
    }
    return head;
}

Node *delete1(Node *head,int m)
{
	Node *p1=NULL,*p2=NULL;
	int count=1;
    p1=head;
    while(count++<m)
    {
    	p2=p1;
    	p1=p1->next ;
	}
	if(p1!=NULL)
	{
		if(p1==head)
		{
			head=p1->next;
		}
		else
		{
			p2->next =p1->next;
		}
		free(p1);
		printf("ok\n");
	}
	else
		printf(" no  data\n");
    return head;
}

Node *change(Node *head,int num)
{
	Node *p=head;
	int count=1,x;
	while(count<num)
	{
		count++;
		p=p->next;
	}
	printf("����%d���޸ĳɣ�\n",num);
	scanf("%d",&x);
	p->data=x;
	return head;
 } 

void find(Node *head,int m)
{
	Node *p=head;
	int count=0;
	while(p!=NULL)
	{
		if(p->data==m)
		{
			count++;
		}
		p=p->next;
	}
	printf("%d�������г�����%d��",m,count);
 } 
int main()
{
	Node *a;
	int x,y,z,t;	
    a=buildlist();
    print(a);
    printf("�����ĸ�����ǰ��\n");
    scanf("%d",&x);
    a=insert(a,x);
    print(a);
    printf("ɾ���ڼ������ݣ�\n");
    scanf("%d",&y);
    a=delete1(a,y);
    print(a);
    printf(" �޸ĵڼ������ݣ�\n");
    scanf("%d",&z);
    a=change(a,z);
    print(a);
    printf("��Ҫ�����ĸ�Ԫ�أ�\n");
	scanf("%d",&t);
	find(a,t);
    release(&a);
    return 0;
    
}

