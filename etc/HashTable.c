/***

이 코드는 제가 짠 코드인지 아닌지 헷갈리네요.
알고리즘 수업을 벌써 4년전?5년전에 들어서 가물가물 합니다.

***/

#include <stdio.h>

#define true 1
#define false 0

#define NO_SUCH_KEY 0
#define AVAILAVLE -1
#define PRIME_NUMBER 37
#define ARRAY_LEN PRIME_NUMBER
/*******************struct *************/
typedef struct hash_table
{
	int key;
	int element;
	int tag;
}HASH_TABLE;
/******************Function prototype****************/
int findElement(HASH_TABLE* hashTable,int  key);
int insertItem(HASH_TABLE* hashTable ,int key , int element);
int getNextBucket(int hashValue , int investigate);
int initBucket(HASH_TABLE * hashTable);
int removeElement(HASH_TABLE * hashTable , int key );
void overFlowException();
int isEmpty(HASH_TABLE * hashTable, int spot);
int deactivate(HASH_TABLE * hashTable , int spot);
int isDeactivated(HASH_TABLE * hashTable , int spot);
int hash(int key);

/**********main Function************/
int main()
{
	HASH_TABLE hashTable[ARRAY_LEN] = {
		{0,0,0},
		{0,0,0},
		{0,0,0},
		{0,0,0},
	};
	char inputFunction = 'NULL';
	int key = 0 , element = 0;
	int check = 0;
	initBucket(hashTable);//�ʱ�ȭ
	insertItem(hashTable , 34 , 34);
	insertItem(hashTable , 67 , 67);
	insertItem(hashTable , 78 , 78);
	insertItem(hashTable , 31 , 31);
	insertItem(hashTable , 60 , 60);
	insertItem(hashTable , 23 , 23);
	insertItem(hashTable , 87 , 87);
	insertItem(hashTable , 88 , 88);
	insertItem(hashTable , 77 , 77);
	insertItem(hashTable , 99 , 99);
	insertItem(hashTable , 11 , 11);
	insertItem(hashTable , 65 , 65);
	insertItem(hashTable , 1 , 1);
	insertItem(hashTable , 2 , 2);
	insertItem(hashTable , 3 , 3);
	insertItem(hashTable , 4 , 4);
	insertItem(hashTable , 5 , 5);
	insertItem(hashTable , 7 , 7);
	insertItem(hashTable , 9 , 9);

	while( inputFunction != 'q' )
	{
		printf("�Է� ���� ex ) f 14 0 | i 56 5 | r 4 0 \n");
		scanf("%c %d %d" , &inputFunction , &key , &element);
		switch ( inputFunction )
		{
		case 'f' :
			if(findElement(hashTable , key) == NO_SUCH_KEY )
			{
				printf(" No Such Key \n" );
			}
			printf("ã�� key�� ���Ҵ� %d �Դϴ�. \n",findElement(hashTable , key));
			break;
		case 'i' :
			check = insertItem(hashTable , key , element);
			if( check == true)
			{
				printf(" Insert Success \n" );
			}
			else
			{
				printf(" insert Failure \n" );
			}
			break;
		case 'r' :
			printf("������ key �� element : %d \n" , removeElement( hashTable , key) );
			break;
		case 'q' :
			break;
		}
	}

return 0;
}

int findElement(HASH_TABLE* hashTable,int  key)
{
	int hashKey = 0;
	int investigate = 0;

	hashKey = hash(key);//Ű�� �ؽ�

	while(investigate < ARRAY_LEN)
	{
		int findBucket = getNextBucket(hashKey, investigate);//�ڸ� ����
		if( hashTable[findBucket].tag == (isEmpty(hashTable , findBucket) ))
		{
			return NO_SUCH_KEY;// ��ã�Ҵ�.
		}
		else if( key == hashTable[findBucket].key )
		{
			return hashTable[findBucket].element;//ã���� element ����
		}
		else
			investigate += 1;//�ڸ� ã�� ��ȸ
	}
	return NO_SUCH_KEY;
}
int insertItem(HASH_TABLE* hashTable ,int key , int element)
{
	int hashKey = 0;
	int investigate = 0;
	hashKey = hash(key);//Ű�� �ؽ�

	while( investigate < ARRAY_LEN )
	{
		int findBucket = getNextBucket(hashKey , investigate);//�ڸ� ����
		if( (isEmpty(hashTable , findBucket) ) )//�����ٸ� - �����ٴ� ���� tag�� 0 �̰ų� available �̸�
		{
			hashTable[findBucket].key = key;
			hashTable[findBucket].element = element;
			hashTable[findBucket].tag = 1;
			return true;
		}
		else
		{
			investigate += 1;
		}
	}
	overFlowException();
	return false;
}
int removeElement(HASH_TABLE * hashTable , int key )
{
	int del_element = 0;
	int investigate = 0;

	while ( investigate < ARRAY_LEN )
	{
		int findBucket = getNextBucket(key , investigate);//�ڸ� ����
		if( hashTable[findBucket].key == key )//�ش� �ڸ� ã����
		{
			deactivate(hashTable, findBucket);//��ũ ǥ��
			del_element = hashTable[findBucket].element;//���� �� ����
			hashTable[investigate].element = 0;//���Ҹ� 0���� �ʱ�ȭ
			return del_element;//��ȯ
		}
		else
			investigate += 1;
	}
	return false;
}
int getNextBucket(int hashValue , int investigate)//collision resolution ���� ������
{
	return (hashValue + investigate) % ARRAY_LEN;
}
int initBucket(HASH_TABLE * hashTable)//�ʱ�ȭ �۾�
{
	int i ;
	for( i = 0; i < ARRAY_LEN; i++)
	{
		hashTable[i].key = 0;
		hashTable[i].element = 0;
		hashTable[i].tag = 0;
	}
	return true;
}

void overFlowException()
{
	printf(" ���� Hash-Table ������ �����մϴ�. \n");
}
int isEmpty(HASH_TABLE * hashTable, int spot)
{
	if( hashTable[spot].tag == 0 || (isDeactivated(hashTable , spot) == true) )
	{
		return true;
	}//�����ų� ������ ��
	else
		//������ ����
		return false;
}
int deactivate(HASH_TABLE * hashTable , int spot)//�����ɶ� ��ũ ǥ��
{
	hashTable[spot].key = AVAILAVLE;
	return true;
}
int isDeactivated(HASH_TABLE * hashTable , int spot)//�����Ǿ��� �ڸ����� Ȯ��
{
	if(hashTable[spot].key == AVAILAVLE)
		return true;
	else
		return false;
}
int hash(int key)//�Է� �� Ű���� ����
{
	return key/2;
}
