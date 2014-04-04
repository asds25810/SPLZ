#include <stdio.h>
#include <stdlib.h>

void KMP_CalculateFail(unsigned char *string,int *fail,int length)
{
	int i=0;
	fail[0]=-1;
	int j=-1;
	while(i<length)
	{
		if(j==-1||string[i]==string[j])
		{
			i++;
			j++;
			fail[i]=j;
		}
		else
			j=fail[j];
	}
	//for(int i=0;i<length;i++)
	//	printf("%2c ",string[i]);
	//printf("\n");
	//for(int i=1;i<length;i++)
	//	printf("%2d ",fail[i]);
}
