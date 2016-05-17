/***

알고리즘 수업 당시 짠 코드입니다.

***/
#include <stdio.h>
#include <string.h>
#define QUEUE_LEN 20

typedef struct list
{
	int key;
	int element;
}PRIORITY_QUEUE;//priority queue struct

/*********************protoptype*******************************/
void inPlaceSelectionSort(PRIORITY_QUEUE* queue);
int smellest(PRIORITY_QUEUE* queue , int sorted);
void iniList(PRIORITY_QUEUE * queue);
void printList(const PRIORITY_QUEUE* queue);
/*********************main function*******************************/
int main()
{
	PRIORITY_QUEUE queue[QUEUE_LEN] =
	{
		{45 , 1},
		{54  , 2},
		{676  , 3},
		{-634  , 4},
		{6  , 5},
		{56, 6},
		{344  , 7},
		{3  , 8},
		{341 , 9},
		{67  , 10},
		{64 , 11},
		{77  , 12},
		{60  , 13},
		{5  , 14},
		{67 , 15},
		{13 , 16},
		{11  , 17},
		{1  , 18},
		{4  , 19},
		{-7  , 20}
	};
	printf("Select Sorting\n");
	printf("정렬 전\n");
	printList(queue);//배열 출력
	inPlaceSelectionSort(queue);//정렬 함수 호출
	printf("정렬 후\n");
	printList(queue);//배열 출력
	printf("자동 종료 방지 Enter 입력\n");
	getchar();
	return 0;
}
/*********************초기화 함수*******************************/
void iniList(PRIORITY_QUEUE * queue)
{//이번 내용에서는 쓸모가 없지만 만들었습니다.
	int i = 0;

	for( i = 0 ; i < QUEUE_LEN; i++)
	{
		queue[i].key = 0;
		queue[i].element = 0;
	}
}
/*********************Selection Sorting Function*******************************/
void inPlaceSelectionSort(PRIORITY_QUEUE* queue)
{
	int sorted = 0;
	int i ,spot,tmpKey = 0 , tmpElement = 0;

	for( i = 0 ; i < QUEUE_LEN - 1 ; i++ , sorted++)
	{
		//최솟값 찾는 함수 호출. 찾을 때마다 앞으로 최소값이 정렬 되므로 값을 증가시켜 넘겨준다.
		spot = smellest(queue , sorted);
		//찾은 최솟값의 인덱스를 가지고서 원래 앞에 있던 자리와 교환한다.
		tmpKey = queue[spot].key;
		tmpElement = queue[spot].element;

		queue[spot].key = queue[i].key;
		queue[spot].element = queue[i].element;

		queue[i].key = tmpKey;
		queue[i].element = tmpElement;
	}
}
/*********************return smellest value Function*******************************/
int smellest(PRIORITY_QUEUE* queue , int sorted)
{
	int i ;
	int smellest = QUEUE_LEN-1;//초기 기준값
	for ( i = QUEUE_LEN-1; i >= sorted; i--)//뒤에서 부터 sorted변수 값까지 줄어들며 loop 순환
	{
		//sorted는 파라미터이면서 queue 내에서 정렬된 경계값이다.
		if(queue[i].key < queue[smellest].key)
		{
			//비교해서 더 작은 배열 인덱스를 저장.
			smellest = i;
		}
	}
	return smellest;
}
/*********************print Queue Function*******************************/
void printList(const PRIORITY_QUEUE* queue)
{
	int i ;
	for( i = 0; i < QUEUE_LEN; i++)
	{
		printf("key : %5d  Element : %5d \n", queue[i].key , queue[i].element );
	}
	printf("\n");
}
