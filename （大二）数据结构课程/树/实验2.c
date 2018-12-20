#include<stdio.h>
#include<stdlib.h> 
#define list Node*

//���������� 

typedef struct btnode//���������
{
	char data;
	struct btnode *lchild,*rchild;
}btnode;
typedef struct btree
{
	struct btnode *root;
}btree;

void createbt(btree *a) //create a empty tree
{
	a->root = NULL;
}

btnode *newnode()
{
	btnode *p=(btnode*)malloc(sizeof(btnode));
	return p;
}

void breakbt(btree *a,btree *l,btree *r)//ɾ�����ڵ�
{
	btnode *p=a->root ;
	if(p!=NULL)
	{
		l->root =p->lchild ;
		r->root =p->rchild ;
		free(p);
		a->root =NULL;
	}
}

void visit(btnode *p)//visit 
{
	printf("%c",p->data );
}

void preorder(btnode *p)//�������
{
	if(p!=NULL)
	{
		visit(p);
		preorder(p->lchild );
		preorder(p->rchild );
	}
}
void pre(btree *a)
{
	preorder(a->root );
}

void inorder(btnode *p)//���� 
{
	if(p!=NULL)
	{
		inorder(p->lchild );	
		visit(p);
		inorder(p->rchild );	
	}
}
void in(btree *a)
{
	inorder(a->root );
}

void postorder(btnode *p)//���� 
{
	if(p!=NULL)
	{
		postorder(p->lchild );	
		postorder(p->rchild );
		visit(p);	
	}
}
void post(btree *a)
{
	postorder(a->root );
}

//���ж��� 
typedef struct Node
{
	btnode *data;
    struct Node *next;
}Node;

typedef struct queue
{
	Node *front,*rear;
 } queue;
 
 Node *create(btnode *num)
{
    Node *p=NULL;
	p=(Node*)malloc(sizeof(Node));
    p->data=num;
    p->next=NULL;
    return p;
}

int isempty(queue *q)
{
	if(q->front==NULL)
		return 1;
	else 
		return 0;
}

void append(queue *q,btnode *num)
{
	Node *p;
	p=create(num);
	if(q->front ==NULL)
	{
		q->front =p;
		q->rear =p; 
	}
	else
	{
		q->rear->next=p;
		q->rear=p;
	}
}

btnode* pop(queue *q)
{
	Node *p;
	btnode *c;
	if(isempty(q))
	{
		printf("empty\n");
	}
	else
	{
		p=q->front ;
		c=p->data ;
		q->front =p->next ;
		free(p);
	}
	return c;
}

void queuefront(queue *q,btnode *x)
{
	Node *p=q->front ;
	if(isempty(q))
	{
		printf("empty\n");
	}
	else
	{
		x=p->data;
	}
}


//������Ӧ�� 

btnode * precreate1()
{
	char c;
	btnode *p;
	printf("������ǰ������,��#����:\n");
	scanf("%c",&c);
	if(c!='#')
	{
		p=newnode();
		p->data =c;
		p->lchild=p->rchild =NULL;
		p->lchild=precreate1();	
		p->rchild=precreate1();	
	}
	else
	{
		p=NULL;
	}
	return p;
}
void precreate(btree *a)//�ݹ齨��������
{
	a->root =precreate1();
}

void arrcreate(btree *a)//�����Ŷ�����
{
	char c;
	int i,j;
	btnode *arr[32];
	btnode *p;
	printf("����һ���ַ���λ��,��A,1����#,0������\n");
	scanf("%c,%d",&c,&i);
	while(c!='#'&&i!=0)
	{
		if(i<32)
		{
			p=newnode();
			p->data=c;
			p->lchild =p->rchild =NULL;
			arr[i]=p;
			if(i!=1)
			{
				j=i/2;
				if(j%2==0)
				{
					arr[j]->lchild =p;
				}
				else
				{
					arr[j]->rchild =p;
				}
			}
			printf("go on\n");
			scanf("%c,%d",&c,&i);
		}
		else
		{
			printf("full!\n");
			break;
		}
	}
	a->root =arr[1];
}

int max(int a,int b)
{
	if(a>b)
		return a;
	else 
		return b;
}
int depth(btnode *p)
{
	if(!p)
		return 0;
	else
		return 1+max(depth(p->lchild),depth(p->rchild ));
}
void clchight(btree *a)//height
{
	btnode *p=a->root ;
	printf("�߶�Ϊ��%d\n",depth(p));
}

int size(btnode *p) 
{
	if(!p)
		return 0;
	else
		return 1+size(p->lchild )+size(p->rchild );
}
void clcsize(btree *a)//������ 
{
	btnode *p=a->root ;
	printf("�����Ϊ��%d\n",size(p));
}

void change(btnode *p)
{
	btnode *t;
	if(p->lchild ||p->rchild )
	{
		if(p->lchild )
			change(p->lchild );
		if(p->rchild )
			change(p->rchild );
		t=p->rchild ;
		p->rchild =p->lchild ;
		p->lchild =t;
	}
}
void exchange(btree *a)//������������ 
{
	change(a->root );
}

void layerorder(btree *a)//��α��� 
{
	btnode *p=a->root ,*q;
	queue *t;
	append(t,p);
	while(!isempty(t))
	{
		q=pop(t);
		printf("%c ",q->data );
		append(t,q->lchild );
		append(t,q->rchild );
	}
	printf("\n");
}

int main()
{
	int i;
	btree *a;
	printf("==========���˵�==========\n");
	printf("����1���ݹ齨��������     \n");
	printf("����2�����齨��������     \n");
	printf("����3�������ӡ������     \n");
	printf("����4�������ӡ������     \n");
	printf("����5�������ӡ������     \n");
	printf("����6����δ�ӡ������     \n");
	printf("����7������������ĸ߶�   \n");
	printf("����8������������Ľ���� \n");
	printf("����9�������������������� \n");
	printf("����0���˳�               \n");
	printf("==========================\n");
	scanf("%d",&i);
	while(i!=0)
	{ 
	switch(i)
	{
	case(1):
		precreate(a);
		break;
    case(2):
    	arrcreate(a);
		break;
	case(3):
		if(a)
		{
			preorder(a->root );
		}
		else
		{
			printf("non exist!\n");
		}
		break;
	case(4):
		if(a)
		{
			inorder(a->root );
		}
		else
		{
			printf("non exist!\n");
		}break;
	case(5):
		if(a)
		{
			postorder(a->root );
		}
		else
		{
			printf("non exist!\n");
		}
		break;
	case(6):
		if(a)
		{
			layerorder(a);
		}
		else
		{
			printf("non exist!\n");
		}
		break;
	case(7):
		if(a)
		{
			clchight(a);
		}
		else
		{
			printf("non exist!\n");
		}
		break;
	case(8):
	    if(a)
		{
			clcsize(a);
		}
		else
		{
			printf("non exist!\n");
		}
		break;
	case(9):
	    if(a)
		{
			exchange(a);
		}
		else
		{
			printf("non exist!\n");
		}
		break;
    scanf("%d",&i);
	}
}
printf("������ллʹ��!\n");
	return 0;
}
