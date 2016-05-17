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
}PRIORITY_QUEUE;

/*********************protoptype**********************************/
void iniList(PRIORITY_QUEUE * queue);
void printList(const PRIORITY_QUEUE* queue);
void inPlaceInsertSort(PRIORITY_QUEUE * queue);
/*********************main function*******************************/
int main()
{
	PRIORITY_QUEUE queue[QUEUE_LEN] =
	{
		{3		, 1},
		{4		, 2},
		{67		, 3},
		{78		, 4},
		{0		, 5},
		{56		, 6},
		{-36	, 7},
		{38		, 8},
		{76		, 9},
		{453	, 10},
		{-89	, 11},
		{75		, 12},
		{8		, 13},
		{56		, 14},
		{357	, 15},
		{46		, 16},
		{-45	, 17},
		{9		, 18},
		{2		, 19},
		{1		, 20}
	};
	printf("Insert Sorting\n");
	printf("정렬 전\n");
	printList(queue);//배열 출력
	inPlaceInsertSort(queue);
	printf("정렬 후\n");
	printList(queue);
	printf("자동 종료 방지 Enter 입력\n");
	getchar();

	return 0;
}
/****************************초기화 함수*************************************/
void iniList(PRIORITY_QUEUE * queue)
{//이번 내용에서는 쓸모가 없지만 만들었습니다.
	int i = 0;

	for( i = 0 ; i < QUEUE_LEN; i++)
	{
		queue[i].key = 0;
		queue[i].element = 0;
	}
}
/*********************Insert Sorting Function*******************************/
void inPlaceInsertSort(PRIORITY_QUEUE * queue)
{
	//key 와 element를 임시로 저장 할 변수
	int tmpKey = 0 , tmpElement = 0;
	//loop control
	int i , j;
	//외부는 0 to QUEUE_LEN
	for( i = 0; i < QUEUE_LEN-1; i++)
	{
		//j는 i로 loop 순환마다 초기화를 한다.
		j = i;
		while(j >= 0)//j가 0 보다 크거나 같을때 까지만 loop 순환
		{
			if(queue[j].key > queue[j+1].key)//다음 값 보다 이전 값이 더 크면 다음 값과 이전 값 자리 교환.
			{
				tmpKey = queue[j+1].key;
				tmpElement = queue[j+1].element;

				queue[j+1].key = queue[j].key;
				queue[j+1].element = queue[j].element;

				queue[j].key = tmpKey;
				queue[j].element = tmpElement;
				j--;//j를 -1씩 해주어야 loop 가 종료된다.
			}
			else//현재 값이 더 작으면 비교할 이유가 없으므로 loop 탈출
			{
				break;
			}
		}
	}
}

/*********************print Queue Function*******************************/
void printList(const PRIORITY_QUEUE* queue)
{
	int i ;
	for( i = 0; i < QUEUE_LEN; i++)
	{
		printf("key : %5d Element : %5d\n", queue[i].key , queue[i].element );
	}
	printf("\n");
}
