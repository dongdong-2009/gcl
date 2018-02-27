
#include <stdio.h>

#include<stdio.h>
int main( void )
{
    int a[5]={1,2,3,4,5};
    int i=0;
    int *ptr=(int *)(&a+1);
    printf("%d,%d",*(a+1),*(ptr-1));

    scanf("%d",i);
    return 1;
}
