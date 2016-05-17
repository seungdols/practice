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
	//Heap �� ù ��° ���� ���� �ʴ� ���̱⿡ 0���� �ʱ�ȭ �Ͽ����ϴ�.
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
	printf("============���� ��===========\n");
	printList(heap);
	heapSort(heap);
	printf("============���� ��===========\n");
	printList(heap);
	printf("�ڵ� ���� ���� Enter �Է�\n");
	getchar();
	return 0;
}
/*******************build heap function**************************/
void buildHeap( HEAP* heap )
{
	int i ;
	//���γ��带 ���� �θ����� ������ �����̹Ƿ� ��ü/2�� �� �κ� ���� heap�� ������.
	for( i = HEAP_LEN/2; i>=1; i-- )
	{
		//downHeap�� ȣ���Ѵ�. downHeap�� ���������� �����ȴ�.
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
	//left ���� �Ķ����ͷ� �־��� last ���� ũ�ٸ� �׳� ���� (�ƴϸ� �迭 ������ �Ѿ�� ���ܰ� �߻�)
	if( left > last )
		return ;
	//greater�� left �� ����
	greater = left;
	//right�� last���� �۰ų� ���� ���� �б�����
	if( right <= last )
		if( heap[right].key > heap[greater].key )
			//right�� �ε����� �ϴ� key�� greater�� �ε����� �ϴ� key���� ũ�ٸ�,
			greater = right;//greater�� right���� ����
	if( heap[i].key >= heap[greater].key )
		return ;
	//���� ��ȯ�Ѵ�.
	tmpKey = heap[i].key;
	tmpElement = heap[i].element;

	heap[i].key = heap[greater].key;
	heap[i].element = heap[greater].element;

	heap[greater].key = tmpKey;
	heap[greater].element = tmpElement;
	//downHeap�Լ��� ���������� ȣ��
	downHeap(heap , greater , last );
}
/***************heapSort function******************************/
void heapSort( HEAP* heap )
{
	//local variable
	int i;
	int tmpKey = 0 , tmpElement = 0;
	//heap�� �����Ѵ�.
	buildHeap(heap);
	//���������� ���� �ö󰡸� heap sort
	for( i = HEAP_LEN-1; i >=2 ; i-- )
	{
		tmpKey = heap[1].key;
		tmpElement = heap[1].element;

		heap[1].key = heap[i].key;
		heap[1].element = heap[i].element;

		heap[i].key = tmpKey;
		heap[i].element = tmpElement;
		downHeap(heap,1,i-1);//��ȯ �� downHeap ȣ��
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
