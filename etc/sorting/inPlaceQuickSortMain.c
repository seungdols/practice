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
	//�Լ� ȣ��
	start = clock();
	inPlaceQuickSort(arr , 0 , ARRLEN-1);
	finish = clock();
	printf( " %f \n" , ((double)(finish - start))/CLOCKS_PER_SEC);

	printf("�ڵ� ���� ���� Enter �Է� \n");
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
	//1���� 0�̸� ���� �ǵ���.
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
	//���� ���� ����
	p = arr[k];
	//���� ���Ҹ� �������� ������.
	swapArrElement(&arr[k] , &arr[r] );

	i = l;
	j = r - 1;
	//���ǿ� ���� ��ġ ���� �ʴ� �͵��� ��ȯ
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
	//���Ҹ� �ٽ� �ٲ۴�.
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
