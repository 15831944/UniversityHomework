/* Note:Your choice is C IDE */
#include "stdio.h"
int main()
{
int m,i,j,s;
for(m=6;m<999;m++) /*��m<=999*/
{ 
s=1; /*s���������������֮�ͣ�1�϶�������*/
for(i=2;i<m;i++)
if(m%i==0) s=s+i; /*����������֮��*/
if(m-s==0) /*���m������*/
{ 
printf("%5d its fastors are 1 ",m);
for(j=2; j<m;j++)
if(m%j==0)
printf("%d ",j);
printf("\n");
}
}
return 0;
}