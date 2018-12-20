#include <stdio.h>

void Find(int *a, int n, int *max, int *maxPos, int *min, int *minPos);

int main()
{
 int array[10] = {23, 45, 90, -9, 43, 90, 4, 2, -9};
 int max; //��������
 int maxPos; //��Ӧ�±�
 int min;
 int minPos;
 Find(array, 10, &max, &maxPos, &min, &minPos);
 printf("max=%d, maxPos=%d, min=%d, minPos=%d\n", max, maxPos, min, minPos); 
 return 0;
}

void Find(int *a, int n, int *max, int *maxPos, int *min, int *minPos)
{
 int i;
 int iTmp = 0; //ƽ����
 for (i=0;i<n;i++)
 {
  iTmp += a[i];
 }
 *max = *min = iTmp = iTmp / n;
 for (i=0;i<n;i++)
 {
  if (a[i] > iTmp)
  {
   if (*max < a[i])
   {
    *max = a[i];
    *maxPos = i;
   }
  }
  else
  {
   if (*min > a[i])
   {
    *min = a[i];
    *minPos = i;
   }
  }
 }
}