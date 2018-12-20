#include<stdio.h>
typedef struct stack
{
	int top,maxstack;
	int elements[50];
 }stack;
 
 void create(stack *s,int max)
 {
 	s->top=-1;
 	s->maxstack =max;
 }
 
 void push(stack *s,int x)
 {
 	s->elements [++s->top ]=x;
 }
 
 void pop(stack *s)
 {
 	if(s->top<0)
 	{
 		printf("empty\n");
	 }
	 else
	 {
	 	s->top--;
	 }
 }
 void print(stack *s)
 {
 	for(s->top =0;(s->top )<(2 );(s->top )++)
 	{
 		printf("%d ",s->elements[s->top ]);
	 }
 }
 
 //������������1��2��3��4��Ϊջ�����룬ʵ�����п�������㷨
 void process(int pos,int *path ,int curp)//����ǰpos ��λ��,�����ν�ջ������ 
 {
 	int m,i;
 	int total=4;//�����������е��ܸ��� 
 	if(pos<total)
 	{
 		push(&s,pos+1);
 		process(pos+1,path,curp);
 		pop();
	 }
	 if(s.top!=-1)//ջ��Ϊ�� 
	 {
	 	m=pop();
	 	path[curp]=m;
	 	curp++;
	 	process(pos+1,path,curp);
	 	push(m);
	 }
	 if(pos==total&&s.top!=-1)//���һ�ֿ��ܵķ��� 
	 {
	 	for(i=0;i<curp;i++)
	 	{
	 		printf("%d ",path[i]);
	 		printf("\n");
		 }
	 }
  } 
  
  int main()
  {
  	int path[10];
	stack s;
	create(&s,4);
	push(&s,1);
	printf("����������У�\n");
	process(1,path,0);
	return 0;
  }
