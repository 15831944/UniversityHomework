#include <stdio.h>
int main()
{
	double a,b,c,x,y,z;
	printf("Please input a,b,c:\n");
	scanf("%lf%lf%lf",&a,&b,&c);
	if (a<=0 || b<=0 || c<=0)
	{
		printf("Error input!\n");
	}
	else
	{
		if (a>b)
		{
			x=a;
			z=b;
		}
		else
		{
			x=b;
			z=a;
		}
		if (c>x)
		{
			y=x;
			x=c;
		}
		else
		{
			if (c<z)
			{
				y=z;
				z=c;
			}
			else
			{
				y=c;
			}
		}
	}/*��abc�Ӵ�С����*/
	if (y+z<a)
	{
		printf("%f %f %f ����������\n",a,b,c);
	}
	else
	{
		if (y*y+z*z==x*x)
		{
			if (y==z)
			{
				printf("%f %f %f �ǵ���ֱ��������\n",a,b,c);
			}
			else
			{
			printf("%f %f %f ��ֱ��������\n",a,b,c);
			}
		}
		else
		{
			if (y=z)
			{
				if (x=y)
				{
					printf("%f %f %f �ǵȱ�������\n",a,b,c);
				}
				else
				{
					printf("%f %f %f �ǵ���������\n",a,b,c);
				}
			}
			else
			{
				printf("%f %f %f ��һ��������\n",a,b,c);
			}
		}
	}
	return 0;
}

