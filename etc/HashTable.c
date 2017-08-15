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
	initBucket(hashTable);//초기화
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
		printf("입력 방식 ex ) f 14 0 | i 56 5 | r 4 0 \n");
		scanf("%c %d %d" , &inputFunction , &key , &element);
		switch ( inputFunction )
		{
		case 'f' :
			if(findElement(hashTable , key) == NO_SUCH_KEY )
			{
				printf(" No Such Key \n" );
			}
			printf("찾은 key의 원소는 %d 입니다. \n",findElement(hashTable , key));
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
			printf("삭제한 key 의 element : %d \n" , removeElement( hashTable , key) );
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

	hashKey = hash(key);//키를 해싱

	while(investigate < ARRAY_LEN)
	{
		int findBucket = getNextBucket(hashKey, investigate);//자리 선정
		if( hashTable[findBucket].tag == (isEmpty(hashTable , findBucket) ))
		{
			return NO_SUCH_KEY;// 못찾았다.
		}
		else if( key == hashTable[findBucket].key )
		{
			return hashTable[findBucket].element;//찾으면 element 리턴
		}
		else
			investigate += 1;//자리 찾아 순회
	}
	return NO_SUCH_KEY;
}
int insertItem(HASH_TABLE* hashTable ,int key , int element)
{
	int hashKey = 0;
	int investigate = 0;
	hashKey = hash(key);//키를 해싱

	while( investigate < ARRAY_LEN )
	{
		int findBucket = getNextBucket(hashKey , investigate);//자리 선정
		if( (isEmpty(hashTable , findBucket) ) )//비었다면 - 비었다는 것은 tag가 0 이거나 available 이면
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
		int findBucket = getNextBucket(key , investigate);//자리 선정
		if( hashTable[findBucket].key == key )//해당 자릴 찾으면
		{
			deactivate(hashTable, findBucket);//마크 표시
			del_element = hashTable[findBucket].element;//요소 값 저장
			hashTable[investigate].element = 0;//요소를 0으로 초기화
			return del_element;//반환
		}
		else
			investigate += 1;
	}
	return false;
}
int getNextBucket(int hashValue , int investigate)//collision resolution 선형 조사법
{
	return (hashValue + investigate) % ARRAY_LEN;
}
int initBucket(HASH_TABLE * hashTable)//초기화 작업
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
	printf(" 모든 Hash-Table 공간이 부족합니다. \n");
}
int isEmpty(HASH_TABLE * hashTable, int spot)
{
	if( hashTable[spot].tag == 0 || (isDeactivated(hashTable , spot) == true) )
	{
		return true;
	}//비었거나 삭제된 셀
	else
		//데이터 존재
		return false;
}
int deactivate(HASH_TABLE * hashTable , int spot)//삭제될때 마크 표시
{
	hashTable[spot].key = AVAILAVLE;
	return true;
}
int isDeactivated(HASH_TABLE * hashTable , int spot)//삭제되었던 자리인지 확인
{
	if(hashTable[spot].key == AVAILAVLE)
		return true;
	else
		return false;
}
int hash(int key)//입력 된 키값을 나눔
{
	return key/2;
}
