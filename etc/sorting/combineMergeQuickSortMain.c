/***

알고리즘 수업 당시 짠 코드입니다.
해당 코드는 세가지 정렬 메소드 기법을 합쳐서 속도를 구하는 코드입니다.
***/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ARRLEN 1000000

/*******************MergeSort function prototype ******************************/

void mergeSort(int arr[] , int tmp[]);
void rMergeSort(int arr[] ,int tmp[], int left, int right);
void merge(int arr[] ,int tmp[], int left , int mid , int right );

/******************Decisive QuickSort function prototype*******************************/

void decisiveQuickSort(int arr[] , int l , int r );
int inPlaceDeciviePartition(int arr[] , int l , int r );
void random(int arr[]);

/******************InPlace QuickSort function prototype*******************************/

void inPlaceQuickSort(int arr[] , int l , int r );
int inPlacePartition(int arr[] , int l , int r , int k);

/*******************common function prototype******************************/
void swapArrElement(int *ptrArrX , int *ptrArrY);
void randomNumber(int arr[]);
void printArr(int arr[]);

int main()
{
	//시간 측정을 위한 변수
	clock_t start , finish;

	//Data 입력을 위해 배열을 상수 사이즈 만큼 할당한다.
	int * arr = (int *)malloc(sizeof(int) * ARRLEN );
	//MergeSort에서 쓰일 임시 버퍼
	int * tmp = (int *)malloc(sizeof(int) * ARRLEN );

	printf("정렬을 시작합니다. merge , decisiveQuick , inplaceQuick Sort \n");

	//난수 발생 후에 배열에 저장.
	randomNumber(arr);
	start = clock();
	mergeSort(arr , tmp);
	finish = clock();
	printf("----------------\n");
	printf("MergeSort		: \t%f \n" , ( (double)(finish - start) )/ CLOCKS_PER_SEC );

	//난수 발생 후에 배열에 저장.
	randomNumber(arr);
	start = clock();
	decisiveQuickSort(arr , 0 , ARRLEN-1);
	finish = clock();
	printf("----------------\n");
	printf("decisiveQuickSort	 : \t%f \n" , ( (double)(finish - start) )/ CLOCKS_PER_SEC );

	//난수 발생 후에 배열에 저장.
	randomNumber(arr);
	start = clock();
	inPlaceQuickSort(arr , 0 , ARRLEN-1);
	finish = clock();
	printf("----------------\n");
	printf("inPlaceQuickSort	 : \t%f \n" , ( (double)(finish - start) )/ CLOCKS_PER_SEC );

	//동적할당한 메모리를 반환한다.
	free(arr);
	free(tmp);
	//포인터는 아무것도 가리키지 않도록 NULL pointer로 변경
	arr = NULL;
	tmp = NULL;

	printf("자동 종료 방지 Enter 입력 \n");
	getchar();

	return 0;
}
/******************MergeSort*******************************/
void mergeSort(int arr[] , int tmp[])
{
	rMergeSort(arr ,tmp, 0 , ARRLEN-1);
	return;
}
/*****************recursion MergeSort********************************/
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
/*****************Merge function********************************/
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

/********************decisive Quick Sorting function***************************************/
void decisiveQuickSort(int arr[] , int l , int r )
{
	int pivot;
	//1개나 0이면 종료 되도록.
	if( l >= r )
		return;

	pivot = inPlaceDeciviePartition(arr , l , r  );
	decisiveQuickSort(arr , l , pivot - 1);
	decisiveQuickSort(arr , pivot + 1, r );
}
/*******************decisive Partitioning****************************************/
int inPlaceDeciviePartition(int arr[] , int l , int r )
{
	int p ;
	int i ,j;

	//기준 원소 저장
	//결정적 퀵정렬은 마지막 원소를 pivot 값으로 잡는다.
	p = arr[r];

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
			//원소 교환
			swapArrElement(&arr[i] , &arr[j] );
		}
	}
	//원소를 다시 바꾼다.
	swapArrElement(&arr[i] , &arr[r] );

	return i;
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
/**********************swaping value in Array***************************/
void swapArrElement(int *ptrArrX , int *ptrArrY)
{
	int tmp;
	tmp = *ptrArrX;
	*ptrArrX = *ptrArrY;
	*ptrArrY = tmp;
}
/*********************pseudo random function****************************/
void randomNumber(int arr[])
{
	int i;

	srand((unsigned)time(NULL));

	for(i = 0; i < ARRLEN; i++)
	{
		arr[i] = rand() % ARRLEN;
	}
}
/***************printing in Array**********************************/
void printArr(int arr[])
{
	int i ;
	for( i = 0; i < ARRLEN; i++)
	{
		printf(" %7d번째 key : %5d  \n", i ,arr[i] );
	}
	printf("\n");
}
