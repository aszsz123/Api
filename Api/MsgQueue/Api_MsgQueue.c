#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <memory.h>

#include <sys/ipc.h>
#include <sys/sem.h>

#include "Api_MsgQueue.h"
//-------------------------------------------------------------------------------//
//#define QUEUE_DEBUG

#ifdef QUEUE_DEBUG
#define dp(fmt,args...) printf(fmt,## args)
#define dlp(fmt,args...) printf("[%s,%s,%d]"fmt,__FILE__,__FUNCTION__,__LINE__,## args)
#else
#define dp(fmt,args...)
#define dlp(fmt,args...)
#endif
//-------------------------------------------------------------------------------//
#define SEM_INIT_OK		1

/**************************************************************************************/
/*--------------------------------------------------------------------------------------
  함수명및기능: initQueue() - 지정된 크기의 큐을 생성하는 함수
  전달인자: qPtr -큐의 주소
  qSize - 큐의 크기
  dataSize - 데이터 하나의 크기(구조체 하나의 크기)
  리턴값: 없음
  --------------------------------------------------------------------------------------*/
void InitQueue(msgQueue_t *qPtr, size_t qSize, size_t dataSize,char *qName)
{
	//**list 
	qPtr->queue = calloc(qSize, dataSize);
	assert(qPtr->queue != NULL);

	qPtr->front = qPtr->rear = 0;
	qPtr->qSize = qSize;

    qPtr->qMemSize = dataSize;
	sem_init(&qPtr->lock,0,0);
	sem_post(&qPtr->lock);
    
	qPtr->semInitResult = SEM_INIT_OK;

	if(qName != NULL)
	{
		dlp("Queue name [%s] - qPtr [0x%x]\n",qName,(int)qPtr);
	}
}

/*--------------------------------------------------------------------------------------
  함수명및기능: clearStack() - 큐 초기화 함수(모든 데이터 삭제)
  전달인자: qPtr - 큐의 주소
  dataSize - 데이터 하나의 크기
  memFree - 큐에 저장되는 데이터의 부가 메모리를 삭제하는 함수, 구조체 별로 생성해야함
  리턴값: 없음
  --------------------------------------------------------------------------------------*/
void ClearQueue(msgQueue_t *qPtr)
{
	int i;

	if(qPtr == NULL)
	{
		dlp("Queue is null\n");
		return;
	}
	if(qPtr->semInitResult != SEM_INIT_OK)
	{
		dlp("#### Queue has not been initialized ####\n");
		return;
	}

	sem_wait(&qPtr->lock);

	for(i = qPtr->front; i < qPtr->rear; i = (i+1)%qPtr->qSize)
	{
		
		memset( (char *)qPtr->queue + qPtr->qMemSize * i, 0, qPtr->qMemSize); // 데이터 영역 초기화
	}	
	qPtr->front = qPtr->rear = 0;  

	sem_post(&qPtr->lock);
}

/*--------------------------------------------------------------------------------------
  함수명및기능: putQueue() - 큐에 데이터 하나를 저장함
  전달인자: qPtr - 큐의 주소
  putData - 큐에 저장할 데이터
  dataSize - 데이터 하나의 크기
  memCpy - 큐에 저장되는 데이터를 복사하는 함수 
  리턴값: 성공적으로저장하면1, 실패하면-1 리턴, 큐 주소가 NULL이면 0을 리턴
  --------------------------------------------------------------------------------------*/
int PutQueue(msgQueue_t *qPtr, void *putData)
{
	if(qPtr->queue == NULL)
	{
		dlp("Queue is null\n");
		return 0;
	}
	if(qPtr->semInitResult != SEM_INIT_OK)
	{
		dlp("#### Queue has not been initialized ####\n");
		return 0;
	}

	sem_wait(&qPtr->lock);

	// 다음 공간이 Queue 완충지대이면 Overflow
	if( ((qPtr->rear + 1 ) % qPtr->qSize) == qPtr->front)
	{
		printf("Queue Overflow\n");
		sem_post(&qPtr->lock);
		return -1;
	}

	memcpy( (char *)(qPtr->queue) + qPtr->rear * qPtr->qMemSize, putData, qPtr->qMemSize  ) ;

	qPtr->rear = (qPtr->rear + 1) % qPtr->qSize;

	sem_post(&qPtr->lock);
	return 1;
}

/*--------------------------------------------------------------------------------------
  함수명및기능: getQueue() - 큐에서 데이터 하나를 꺼냄
  전달인자: qPtr - 큐의 주소
  getData - 꺼낸 데이터를 저장할 기억 공간의 주소
  dataSize - 데이터 하나의 크기
  memCpy - 큐에 저장되는 데이터를 복사하는 함수, 구제체 별로 생성해야 함
  memFree - 큐에 저장되는 데이터의 부가 메모리를 삭제하는 함수 
  리턴값: 성공적으로꺼내면1, 실패하면-1 리턴,큐 주소가 NULL이면 0을 리턴
  --------------------------------------------------------------------------------------*/
int GetQueue(msgQueue_t *qPtr, void *getData)
{
	if(qPtr->queue == NULL)
	{
		dlp("Queue is null\n");	
		return 0;
	}

	if(qPtr->semInitResult != SEM_INIT_OK)
	{
        dlp("Queue Sem Not Init\n");
		return 0;
	}

	sem_wait(&qPtr->lock);

	// 두개의 포인터가 같으면 Queue에 데이터가 없는것
	if(qPtr->front == qPtr->rear)
	{
		sem_post(&qPtr->lock);
		return -1;
	}

	memcpy( getData, (char *)qPtr->queue + qPtr->qMemSize * qPtr->front, qPtr->qMemSize);
	memset( (char *)qPtr->queue + qPtr->qMemSize * qPtr->front, 0x00, qPtr->qMemSize );// get한 영역을 0으로 초기화

	qPtr->front = (qPtr->front + 1) % qPtr->qSize;

	sem_post(&qPtr->lock);
	return 1;
}

/*--------------------------------------------------------------------------------------
  함수명및기능: printQueue() - 큐의 모든 데이터를 출력 함
  전달인자: qPtr - 큐의 주소
  dataSize - 데이터 하나의 크기
  memPrint - 큐에 저장되는 데이터 하나를 출력하는 함수
  리턴값: 없음
  --------------------------------------------------------------------------------------*/
void PrintQueue(msgQueue_t *qPtr)
{
	int i;

	//printf("Queue 내의 데이터 : Front ----> rear\n");

	sem_wait(&qPtr->lock);
	if(qPtr->front == qPtr->rear)
	{
		printf("Queue is empty\n\n");
		sem_post(&qPtr->lock);
		return;
	}

	for(i = qPtr->front; i != (qPtr->rear) % qPtr->qSize; i = (i + 1) % qPtr->qSize)
	{
		//memPrint( (char *)qPtr->queue+dataSize * i );
	}
	sem_post(&qPtr->lock);

	//printf("\n\n");
}

/*--------------------------------------------------------------------------------------
  함수명및기능: destroyQueue() - 큐 소멸함수
  전달인자: qPtr - 큐의 주소
  dataSize - 데이터 하나의 크기
  memFree - 큐에 저장되는 데이터에 부가 메모리를 삭제하는 함수 
  리턴값: 없음
  --------------------------------------------------------------------------------------*/
void DestroyQueue(msgQueue_t *qPtr)
{
	int i;

	sem_wait(&qPtr->lock);
	for(i=qPtr->front; i<qPtr->rear; i=(i+1)%qPtr->qSize)
	{
		memset( (char *)qPtr->queue + qPtr->qMemSize * i, 0x00, qPtr->qMemSize);
	}

	free(qPtr->queue); //  큐영역 해제

	qPtr->queue = NULL;
	qPtr->front = qPtr->rear = 0;  

	sem_destroy(&qPtr->lock);

	sem_post(&qPtr->lock);
}
/**************************************************************************************/
/**************************************************************************************/

