/***

알고리즘 수업 당시 짠 코드입니다.

***/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ARRLEN 1000000

/***********************************************************/
/**********************function prototype*************************************/
void inPlaceQuickSort(int arr[] , int l , int r );
int inPlacePartition(int arr[] , int l , int r , int k);
void random(int arr[]);
void swapArrElement(int *ptrArrX , int *ptrArrY);
/*********************main function**************************************/
int main()
{
	clock_t start , finish;
	int * arr = (int*) malloc ( sizeof(int) * ARRLEN );
	random(arr);
	//함수 호출
	start = clock();
	inPlaceQuickSort(arr , 0 , ARRLEN-1);
	finish = clock();
	printf( " %f \n" , ((double)(finish - start))/CLOCKS_PER_SEC);

	printf("자동 종료 방지 Enter 입력 \n");
	getchar();
	return 0;
}

void random(int arr[])
{
	int i;
	srand((unsigned)time(NULL));

	for( i = 0; i < ARRLEN; i++ )
	{
		arr[i] = rand() % ARRLEN;
	}
}
/********************inPlace Quick Sorting function***************************************/
void inPlaceQuickSort(int arr[] , int l , int r )
{
	int k = ( l + r )/2;
	int pivot;
	//1개나 0이면 종료 되도록.
	if( l >= r )
		return;

	pivot = inPlacePartition(arr , l , r , k );
	inPlaceQuickSort(arr , l , pivot - 1);
	inPlaceQuickSort(arr , pivot + 1, r );
}
/*******************inPlace Partitioning****************************************/
int inPlacePartition(int arr[] , int l , int r , int k)
{
	int p ;
	int i ,j;
	//기준 원소 저장
	p = arr[k];
	//기준 원소를 마지막에 숨긴다.
	swapArrElement(&arr[k] , &arr[r] );

	i = l;
	j = r - 1;
	//조건에 따라 일치 하지 않는 것들을 교환
	while( i <= j )
	{
		while( i<= j && arr[i] <= p )
		{
			i++;
		}
		while( j >= i && arr[j] >= p )
		{
			j--;
		}
		if( i < j )
		{
			swapArrElement(&arr[i] , &arr[j] );
		}
	}
	//원소를 다시 바꾼다.
	swapArrElement(&arr[i] , &arr[r] );

	return i;
}
void swapArrElement(int *ptrArrX , int *ptrArrY)
{
	int tmp;
	tmp = *ptrArrX;
	*ptrArrX = *ptrArrY;
	*ptrArrY = tmp;
}
