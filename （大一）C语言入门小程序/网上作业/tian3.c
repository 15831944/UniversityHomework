#include<stdio.h>
int main()
{
int n,m,r,count=0;
printf("please input a integer:\n");
scanf("%d",&m);
if (m<0) m=-m; /*��֤���յ�m��С��0*/
n=m; /*�˴�n�������������mֵ,�Ա����������*/
do
{
r=m%10;
printf("%d",r);
count++;
m=m/10;
}while (m); 
printf("\n%d has %d digits\n",n,count);
return 0;
}