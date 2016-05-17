/***

알고리즘 수업 당시 짠 코드입니다.

***/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ARRLEN 1000000

/******************************************/
/**********************function prototype*************************************/
void decisiveQuickSort(int arr[] , int l , int r );
int inPlaceDeciviePartition(int arr[] , int l , int r );
void random(int arr[]);
void swapArrElement(int *ptrArrX , int *ptrArrY);
/*********************main function**************************************/
int main()
{
	//int i;
	clock_t start , finish;
	int * arr = (int*)malloc(sizeof(int) * ARRLEN ) ;
	random(arr);
	//�Լ� ȣ��

	start = clock();
	decisiveQuickSort(arr , 0 , ARRLEN-1);
	finish = clock();
	printf(" %f \n" , ( (double)(finish - start) )/ CLOCKS_PER_SEC );
	//for( i = 0; i < ARRLEN; i++)
	//{
	//	printf("%d " , arr[i] );
	//}
	//printf("\n");

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
void decisiveQuickSort(int arr[] , int l , int r )
{
	int pivot;
	//1���� 0�̸� ���� �ǵ���.
	if( l >= r )
		return;

	pivot = inPlaceDeciviePartition(arr , l , r  );
	decisiveQuickSort(arr , l , pivot - 1);
	decisiveQuickSort(arr , pivot + 1, r );
}
/*******************inPlace Partitioning****************************************/
int inPlaceDeciviePartition(int arr[] , int l , int r )
{
	int p ;
	int i ,j;

	//���� ���� ����
	//������ �������� ������ ���Ҹ� pivot ������ ���´�.
	p = arr[r];

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
			//���� ��ȯ
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
