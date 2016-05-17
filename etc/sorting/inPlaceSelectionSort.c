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
	printf("���� ��\n");
	printList(queue);//�迭 ����
	inPlaceSelectionSort(queue);//���� �Լ� ȣ��
	printf("���� ��\n");
	printList(queue);//�迭 ����
	printf("�ڵ� ���� ���� Enter �Է�\n");
	getchar();
	return 0;
}
/*********************�ʱ�ȭ �Լ�*******************************/
void iniList(PRIORITY_QUEUE * queue)
{//�̹� ���뿡���� ������ ������ ���������ϴ�.
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
		//�ּڰ� ã�� �Լ� ȣ��. ã�� ������ ������ �ּҰ��� ���� �ǹǷ� ���� �������� �Ѱ��ش�.
		spot = smellest(queue , sorted);
		//ã�� �ּڰ��� �ε����� �������� ���� �տ� �ִ� �ڸ��� ��ȯ�Ѵ�.
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
	int smellest = QUEUE_LEN-1;//�ʱ� ���ذ�
	for ( i = QUEUE_LEN-1; i >= sorted; i--)//�ڿ��� ���� sorted���� ������ �پ����� loop ��ȯ
	{
		//sorted�� �Ķ������̸鼭 queue ������ ���ĵ� ���谪�̴�.
		if(queue[i].key < queue[smellest].key)
		{
			//�����ؼ� �� ���� �迭 �ε����� ����.
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
