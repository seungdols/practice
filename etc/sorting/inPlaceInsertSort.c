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
	printf("���� ��\n");
	printList(queue);//�迭 ����
	inPlaceInsertSort(queue);
	printf("���� ��\n");
	printList(queue);
	printf("�ڵ� ���� ���� Enter �Է�\n");
	getchar();

	return 0;
}
/****************************�ʱ�ȭ �Լ�*************************************/
void iniList(PRIORITY_QUEUE * queue)
{//�̹� ���뿡���� ������ ������ ���������ϴ�.
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
	//key �� element�� �ӽ÷� ���� �� ����
	int tmpKey = 0 , tmpElement = 0;
	//loop control
	int i , j;
	//�ܺδ� 0 to QUEUE_LEN
	for( i = 0; i < QUEUE_LEN-1; i++)
	{
		//j�� i�� loop ��ȯ���� �ʱ�ȭ�� �Ѵ�.
		j = i;
		while(j >= 0)//j�� 0 ���� ũ�ų� ������ ������ loop ��ȯ
		{
			if(queue[j].key > queue[j+1].key)//���� �� ���� ���� ���� �� ũ�� ���� ���� ���� �� �ڸ� ��ȯ.
			{
				tmpKey = queue[j+1].key;
				tmpElement = queue[j+1].element;

				queue[j+1].key = queue[j].key;
				queue[j+1].element = queue[j].element;

				queue[j].key = tmpKey;
				queue[j].element = tmpElement;
				j--;//j�� -1�� ���־��� loop �� �����ȴ�.
			}
			else//���� ���� �� ������ ������ ������ �����Ƿ� loop Ż��
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
