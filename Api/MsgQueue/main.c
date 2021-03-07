#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "Api_MsgQueue.h"

msgQueue_t queue;

typedef struct msgData{
	char str[100];
	int num;
}msgData_t;

int main (void)
{
	msgData_t tx;
	msgData_t rx;

	int try=0;
	//void InitQueue(msgQueue_t *qPtr, size_t qSize, size_t dataSize,char *qName);
	InitQueue(&queue, 10, sizeof(msgData_t), "MsgQueue_Test");

	while(1)
	{
		tx.num = try;
		sprintf(tx.str, "test[%d]", try++);
		
 		//int PutQueue(msgQueue_t *qPtr, void *putData);                                                                                         
		PutQueue(&queue, &tx);
		
		memset(&tx, 0, sizeof(msgData_t));
		sleep(1);
		
		//int GetQueue(msgQueue_t *qPtr, void *getData);
		GetQueue(&queue, &rx);
		printf("%d : {%s}\n", rx.num, rx.str);

		memset(&rx, 0,sizeof(msgData_t));

		if(try == 10)
			break;
	}

	//void DestroyQueue(msgQueue_t *qPtr);
	DestroyQueue(&queue);
		
}
