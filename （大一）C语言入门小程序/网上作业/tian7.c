#include<stdio.h> 
#include<math.h>
int Prime(int n); /*�ж�һ�������Ƿ�Ϊ�������Ƿ���1�񷵻�0*/
double fun(int n);
int main()
{
int n;
scanf("%d",&n);
printf("sum till %d is :%lf\n",n,fun(n));
return 0;
}

int Prime(int n)
{
int k,i;
k=(int)sqrt(n);
for(i=2;i<=k;i++)
if(n%i==0) return 0;
if(i>k) return 1;
}

double fun(int n) /*��3��n֮����������ƽ����֮��*/
{ 
int m;
double s;
for(m=3;m<=n;m=m+2)
{
if (Prime(m))
s+=sqrt(m);
}
return s;
}