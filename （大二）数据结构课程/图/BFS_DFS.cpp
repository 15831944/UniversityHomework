#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>
#define list Node*
//ͼ ��ADT 
typedef int T;

typedef struct enode{
	int num;//һ���ڽӵ� 
	T data;//Ȩֵ 
	struct enode *next;//��һ���߽�� 
}ENode;
typedef struct graph{
	int vertices;
	ENode ** a;
}Graph; 

//�������� 
typedef struct Node
{
	int data;
    struct Node *next;
}Node;
typedef struct queue
{
	Node *front,*rear;
 }queue;
 
 void init(queue *q){
 	q->front=q->rear=NULL;
 }
 
//���� 
void append(queue *q,int x)
{
	Node *p=(Node*)malloc(sizeof(Node));
	p->data=x;
	p->next =NULL;
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

void pop(queue *q)
{
	Node *p;
	if(q->front!=q->rear)
	{
		printf("empty\n");
	}
	else
	{
		p=q->front ;
		q->front =p->next ;
		free(p);
	}
}

void queuefront(queue *q,int *x)
{
	Node *p=q->front ;
	if(q->front!=q->rear)
	{
		printf("empty\n");
	}
	else
	{
		*x=p->data;
	}
}

//ͼ 
 
 //�����ڽӱ� 
void CreateGraph(Graph *g,int n){
	int i;
	g->vertices=n;
	g->a=(ENode**)malloc(n*sizeof(ENode*));
	for(i=0;i<n;i++){
		g->a[i]=NULL;
	}
}

//�����½��
ENode* NewENode(int num, T data ,ENode *next ){
	ENode *p;
	p=(ENode*)malloc(sizeof(ENode));
	p->data=data;
	p->next=next;
	p->num=num;
	return p;
} 

//����
int search(Graph g,int u,int v) {
	int n;
	ENode *p;
	n=g.vertices;
	if(u<0||u>n-1){//���� 
		return 0;
	}
	for(p=g.a[u];p&&p->num!=v;p=p->next){
	}
	if(!p)
	{
		return 0;
	}
	return 1;
}

//���� 
int add(Graph *g,int u,int v,T data){
	int n;
	ENode *p;
	n=g->vertices;
	if(u<0||u>n-1||v>n-1||u==v||search(*g,u,v)){
		return 0;
	}
	p=NewENode(v,0,g->a[u]);
	g->a[u]=p;
	return 1;
}

//ɾ��
int deleteEnode(Graph *g,int u,int v){
	int n=g->vertices;
	ENode *p,*q;
	if(u>-1&&u<n){
		p=g->a[u];
		q=NULL;
		while((p->num!=v)&&p){
			
			q=p;
			p=p->next;
		}
		if(p){
			if(q){//p���ǵ�һ����� 
				q->next=p->next;
			}else{
				g->a[u]=p->next;
			}
			free(p);
			return 1;
		}
	}
	return 0;
}

//��ȱ����ݹ��㷨 
void DFS(Graph g,int v,int *visit){
	ENode *p;
	visit[v]=1;
	printf("%d ",v);
	for(p=g.a[v];p;p=p->next){
		if(!visit[p->num]){
			DFS(g,p->num,visit);
		}
	}
}
void TranDFS(Graph g){
	int visit[100];
	int i,n=g.vertices;
	for(i=0;i<n;i++){
		visit[i]=0;
	}
	for(i=0;i<n;i++){
		if(!visit[i]){
			DFS(g,i,visit);	
		}
	
	}
}

//��ȱ����㷨
void BFS(Graph g,int v,int *visit){
	ENode *n;
	int u;
	queue q;
	init(&q);//��ʼ������ 
	visit[v]=1;
	printf("%d ",v);
	append(&q,v);
	while(q.front!=q.rear){
		queuefront(&q,&u);
		pop(&q);
		for(n=g.a[u];n;n=n->next){
			if(!visit[n->num]){
				printf("%d ",n->num);
				visit[n->num]=1;
				append(&q,v);
			}
		}
	}
}
void TranBFS(Graph g){
	int visit[10];
	int i,n=g.vertices;
	for(i=0;i<n;i++){
		visit[i]=0;
	}
	for(i=0;i<n;i++){
		if(!visit[i]){
			BFS(g,i,visit);
		}
	}
}

int main()
{
	Graph g;
	CreateGraph(&g,4);
	add(&g,1,0,0);
	add(&g,1,2,0);
	add(&g,2,3,0);
	add(&g,3,0,0);
	add(&g,3,1,0);
	TranDFS(g);
	printf("\n");
	TranBFS(g);
	printf("\n");
	
	add(&g,0,2,0);
	TranDFS(g);
	printf("\n");
	TranBFS(g);
	printf("\n");
	
	if(deleteEnode(&g,2,3)){
		TranDFS(g);
		printf("\n");
		TranBFS(g);
	}
	return 0;
}
