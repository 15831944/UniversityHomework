#include<stdio.h>
int input(char *s)    
{
	int i=0;
	char c;
	do
	{
		c=getchar();
		if (c>='0'&&c<='9'||c>='A'&&c<='F'||c>='a'&&c<='f')
			s[i++]=c;
		else 			
			return 0;
		
	}while (i<4);
	s[i]='\0';
	return 1;
}

int change(char *s)   
{
	int n=0,i=0;
	while (s[i])
	{
		if (s[i]>='0'&&s[i]<='9')
			n=n*16+(s[i]-48);
		else if(s[i]>='A'&&s[i]<='F')
			n=n*16+(s[i]-55);
		else if(s[i]>='a'&&s[i]<='f')
			n=n*16+(s[i]-87);
		i++;
	}
	return n;
}
int main( )
{	
	char s[5];          //�����ַ�����
	if (input(s))        //�����ȷ��������λʮ���������Ž���ת�������
		printf("The dec data is:%d\n",change(s));
	else              //���������ʾ��Ϣ
		printf("Original data error\n");				
	return 0;
}
