//#pragma once

#ifndef API_MSGQUEUE_H
#define API_MSGQUEUE_H

#include <stdio.h>
#include <stdint.h>
#include <semaphore.h>

typedef struct _stQUEUE{
	void *queue;		// queue로 사용되는 배열
	int qSize;			// Queue Get Max Cnt
	size_t qMemSize;		// Queue Mem Size

	int front;			// 읽을 위치
	int rear; 			// 쓸 위치
//-----------------
	sem_t lock;
	int semInitResult;
}msgQueue_t;


void InitQueue(msgQueue_t *qPtr, size_t qSize, size_t dataSize,char *qName);
void ClearQueue(msgQueue_t *qPtr);

int PutQueue(msgQueue_t *qPtr, void *putData);
int GetQueue(msgQueue_t *qPtr, void *getData);

void PrintQueue(msgQueue_t *qPtr);
void DestroyQueue(msgQueue_t *qPtr);

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

#endif
