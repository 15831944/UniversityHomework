/* Note:Your choice is C IDE */
//����ֵС��10-5(������λС��)��s=x-x2/2!+x3/3!-x4/4!+��
#include "stdio.h"
#include"math.h"
void main()
{
   double x,item,s=0.0;
   int i,j,t=1,judge=1;
   scanf("%lf",&x);
   for(i=1;fabs(item)>=(10e-5);i++)
   {
   	   for(j=1;j<=i;j++)
   	   {
   	   	   t*=j;
   	   }
   	   for(j=0;j<=i-1;j++)
       {
       	   x*=x;
       }
   	   item=judge*x/t;
   	   s+=item;
   	   judge=-judge;
   	   
   }
   printf("%.2f",s);
}