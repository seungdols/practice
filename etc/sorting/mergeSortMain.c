/***

알고리즘 수업 당시 짠 코드입니다.

***/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ARRLEN 10000000

void mergeSort(int arr[] , int tmp[]);
void rMergeSort(int arr[] ,int tmp[], int left, int right);
void merge(int arr[] ,int tmp[], int left , int mid , int right );
void iniArr(int arr[]);
void randomNumber(int arr[]);
void printArr(int arr[]);

int main()
{
	clock_t start , finish;

	int * arr = (int *)malloc(sizeof(int) * ARRLEN );
	int * tmp = (int *)malloc(sizeof(int) * ARRLEN );
	randomNumber(arr);

	start = clock();
	mergeSort(arr , tmp);
	finish = clock();
	printf("----------------\n");

	printf(" %f \n" , ( (double)(finish - start) )/ CLOCKS_PER_SEC );
	//printArr(arr);

	free(arr);
	free(tmp);
	arr = NULL;
	tmp = NULL;

	return 0;
}

void mergeSort(int arr[] , int tmp[])
{
	rMergeSort(arr ,tmp, 0 , ARRLEN-1);
	return;
}
void rMergeSort(int arr[] ,int tmp[], int left, int right)
{
	int mid;
	if(left < right)
	{
		mid = (left + right)/2;
		rMergeSort(arr ,tmp, left , mid);
		rMergeSort(arr ,tmp, mid+1 , right);
		merge(arr , tmp,left , mid , right);
	}
	return;
}
void merge(int arr[] , int tmp[] ,int left , int mid , int right )
{
	int i , k ,j;

	i = left;
	k = left;

	j = mid + 1;
	while( i <= mid && j <= right )
	{
		if(arr[i] <= arr[j])
		{
			tmp[k++] = arr[i++];
		}
		else
		{
			tmp[k++] = arr[j++];
		}
	}
	while( i <= mid )
	{
		tmp[k++] = arr[i++];
	}
	while( j <= right)
	{
		tmp[k++] = arr[j++];
	}
	for( k = left; k <= right; k++ )
	{
		arr[k] = tmp[k];
	}
	return;
}
void iniArr(int arr[])
{
	int i = 0;

	for( i = 0 ; i < ARRLEN; i++)
	{
		arr[i] = 0;
	}
}
void randomNumber(int arr[])
{
	int i;

	srand((unsigned)time(NULL));

	for(i = 0; i < ARRLEN; i++)
	{
		arr[i] = rand() % ARRLEN;
	}
}
void printArr(int arr[])
{
	int i ;
	for( i = 0; i < ARRLEN; i++)
	{
		printf(" %5d번째 key : %5d  \n", i ,arr[i] );
	}
	printf("\n");
}
