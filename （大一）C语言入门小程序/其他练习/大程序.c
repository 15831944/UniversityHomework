/* Note:Your choice is C IDE */
#include "stdio.h"
#include"string.h"
typedef struct student
{
	int  num;
	char name[20];
	int score;
	int rank;
}student;
int readin(student *stu)
{
	int i=0;
    int n;
    printf("ѧ�ţ�");
	scanf("%d\n",&stu[i].num);
	printf("������");
    scanf("%c\n",&stu[i].name);
    printf("�ɼ���");
    scanf("%d",&stu[i].score);
    printf("        1.����        ");
    printf("        0.����        ");
    scanf("%d",&n);
    switch(n)
    {
    	case 0:printf("���أ�");break;
    	case 1:
    	{
    	if(i<19)
    	{
    		i++;
    		readin(stu);
    	}
    	else printf("���أ�");
    	}
    }
	
}
void print(student *stu)
{
	int i;
	FILE *fp;
	char ch;
	fp=fopen("d:\\ѧ������.txt","r");
	if(fp==0)
	{
		printf("�ļ���ʧ�ܣ�");
	}
	ch=fgetc(fp);
	printhead();
	while(ch!=EOF)
	{
		putchar(ch);
		ch=fgetc(fp);
	}
	putchar('\n');
    fclose(fp);
    
}
int sort(student *stu,int n)
{
	
}
int cul(student *stu,int n)
{
	
}
int insert(student *stu,int n)
{
	
}
int del(student *stu,int n)
{
	
}
void printhead()
{
	printf("ѧ��    ����    �ɼ�    ����\n");
}
void menu()
{
	printf("********1.��ʾ������Ϣ********\n");
	printf("********2.������Ϣ����********\n");
	printf("********3.ѧ���ɼ�����********\n");
	printf("********4.���Գɼ�ͳ��********\n");
	printf("********5.����������ѯ********\n");
	printf("********0.�˳�        ********\n");
}
void manubase()
{
	printf("%%%%%%%%1.����ѧ����¼%%%%%%%%\n");
	printf("%%%%%%%%2.ɾ��ѧ����¼%%%%%%%%\n");
	printf("%%%%%%%%3.�޸�ѧ����¼%%%%%%%%\n");
	printf("%%%%%%%%0.�˳�        %%%%%%%%\n");
}
void menuscore()
{
	printf("@@@@@@@@1.����ѧ���ܷ�@@@@@@@@");
	printf("@@@@@@@@2.�����ܷ�����@@@@@@@@");
	printf("@@@@@@@@0.�����ϲ�˵�@@@@@@@@");
}
void menucount()
{
	printf("&&&&&&&&1.��γ���߷�&&&&&&&&");
	printf("&&&&&&&&2.��γ���ͷ�&&&&&&&&");
	printf("&&&&&&&&3.��γ�ƽ����&&&&&&&&");
	printf("&&&&&&&&0.�����ϲ�˵�&&&&&&&&");
}
void menuscarch()
{
	printf("########1.��ѧ�Ų�ѯ  ########");
	printf("########2.��������ѯ  ########");
	printf("########3.�����β�ѯ  ########");
	printf("########0.�����ϲ�˵�########");
}
int menu2(student *stu,int n)
{
	int choice,t,find[20];
	student s;
	do
	{
		menubase();
		printf("ѡ��\n");
		scanf("%d",&choice);
		switch(choice)
		{
			case 1:
		}
	};
}

int main()
{
    stedunt stu[20];
    int choice,n;
    FILE *fp;
    fp=fopen("d:\\ѧ������.txt","w+");
    if(fp==0)
    {
    	printf("�ļ���ʧ�ܣ�");
    }
    menu();
    printf("ѡ��");
    scanf("%d",&n);
    switch(n)
    {
    	case 1:
    	case 2:
    	case 3:
    	case 4:
    	case 5:
    	case 0:printf("�ټ���");
    }
}