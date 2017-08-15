/***

알고리즘 수업 당시 짠 코드입니다.

***/
#include <stdio.h>
#include <string.h>
#define true 1
#define false 0
#define HEAP_LEN 20

/*************define struct*****************/

typedef struct heap
{
	int key;
	int element;
}HEAP;

/**************function prototype****************/

void buildHeap(HEAP * heap);
void downHeap(HEAP* heap , int i , int last);
void heapSort(HEAP* heap);
void printList(const HEAP* heap);

/***************main function***************/

int main()
{
	//Heap 의 첫 번째 값은 쓰지 않는 값이기에 0으로 초기화 하였습니다.
	HEAP heap[HEAP_LEN] =
	{
		{0		, 0},
		{99		, 1},
		{7		, 2},
		{66		, 3},
		{5		, 4},
		{9		, 5},
		{56		, 6},
		{78		, 7},
		{38		, 8},
		{76		, 9},
		{74		, 10},
		{12		, 11},
		{11		, 12},
		{567	, 13},
		{5		, 14},
		{8		, 15},
		{45		, 16},
		{47		, 17},
		{37		, 18},
		{13		, 19}
	};
	printf("============정렬 전===========\n");
	printList(heap);
	heapSort(heap);
	printf("============정렬 후===========\n");
	printList(heap);
	printf("자동 종료 방지 Enter 입력\n");
	getchar();
	return 0;
}
/*******************build heap function**************************/
void buildHeap( HEAP* heap )
{
	int i ;
	//내부노드를 갖는 부모부터 말썽이 시작이므로 전체/2를 한 부분 부터 heap을 만든다.
	for( i = HEAP_LEN/2; i>=1; i-- )
	{
		//downHeap을 호출한다. downHeap은 재귀적으로 실행된다.
		downHeap(heap, i , HEAP_LEN);
	}
}
/******************downHeap function***************************/
void downHeap( HEAP* heap , int i , int last )
{
	//local variable
	int left = 2*i;
	int right = 2*i + 1;
	int greater = 0;
	int tmpKey = 0 , tmpElement = 0;
	//left 값이 파라미터로 주어진 last 보다 크다면 그냥 종료 (아니면 배열 범위를 넘어서는 예외가 발생)
	if( left > last )
		return ;
	//greater에 left 값 저장
	greater = left;
	//right가 last보다 작거나 같을 때만 분기실행
	if( right <= last )
		if( heap[right].key > heap[greater].key )
			//right를 인덱스로 하는 key가 greater를 인덱스로 하는 key보다 크다면,
			greater = right;//greater에 right값을 저장
	if( heap[i].key >= heap[greater].key )
		return ;
	//값을 교환한다.
	tmpKey = heap[i].key;
	tmpElement = heap[i].element;

	heap[i].key = heap[greater].key;
	heap[i].element = heap[greater].element;

	heap[greater].key = tmpKey;
	heap[greater].element = tmpElement;
	//downHeap함수를 재귀적으로 호출
	downHeap(heap , greater , last );
}
/***************heapSort function******************************/
void heapSort( HEAP* heap )
{
	//local variable
	int i;
	int tmpKey = 0 , tmpElement = 0;
	//heap을 생성한다.
	buildHeap(heap);
	//마지막부터 위로 올라가며 heap sort
	for( i = HEAP_LEN-1; i >=2 ; i-- )
	{
		tmpKey = heap[1].key;
		tmpElement = heap[1].element;

		heap[1].key = heap[i].key;
		heap[1].element = heap[i].element;

		heap[i].key = tmpKey;
		heap[i].element = tmpElement;
		downHeap(heap,1,i-1);//교환 후 downHeap 호출
	}
	return ;
}
/*********************print Function*******************************/
void printList(const HEAP* heap)
{
	int i ;

	for( i = 1; i < HEAP_LEN; i++)
	{
		printf("key : %5d Element : %5d \n", heap[i].key , heap[i].element);
	}
	printf("\n");
}
