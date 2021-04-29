#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "Api_PmsgQueue.h"

pthread_t rcvth;
pthread_t sendth;

typedef struct st_buf_t{
    char name[30];
    int kind;
    char data[1024];
}buf_st;

void *threadrcv(void)
{
    int fd;
    fd = InitQueue("mq", sizeof(buf_st), 1);

    buf_st rcv;
    while(1)
    {
        RcvQueue(fd, (char *)&rcv, sizeof(buf_st) );    
        printf("=========================\r\n");
        printf("[NAME] %s\n", rcv.name);
        printf("[KIND] %d\n", rcv.kind);
        printf("[DATA] %s\n", rcv.data);
        printf("=========================\r\n");
        usleep(10*1000);
    }
}

#define TEST_DATA "Posix Msg Queue 시험"
void *threadsend(void)
{
    buf_st send;

    int fd;
    fd = InitQueue("mq", sizeof(buf_st), 0);

    memcpy(send.name,"send0", 5);
    send.kind=1;
    memcpy(send.data,TEST_DATA, strlen(TEST_DATA));

    while(1)
    {
       
        SendQueue(fd, (char *)&send, sizeof(buf_st));
        usleep(50*1000);
      
    }
}



void StartRcv(void)
{
    if(pthread_create(&rcvth, NULL, (void *)threadrcv, NULL)<0)
    {
        perror("rcv err : ");
        exit(0);
    }
    pthread_detach(rcvth);
    printf("rcv Ok \r\n");
}

void StartSend(void)
{
    if(pthread_create(&sendth, NULL, (void*)threadsend, NULL)<0)
    {
        perror("send err : ");
        exit(0);
    }
    pthread_detach(sendth);
    printf("send Ok \r\n");
}


int  main(void)
{
    StartRcv();
    StartSend();

    while(1)
    {

    }
    return 1;
}
