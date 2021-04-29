#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define Q_MAX_NUM 256

int InitQueue(char *mqName, long size, int type)
{
    struct mq_attr attr;

    char name[100];

    memset(name, 0, 100);
    sprintf(name, "/%s", mqName);

    attr.mq_maxmsg = Q_MAX_NUM;
    attr.mq_msgsize = size;

    mqd_t mfd;

    if(type == 1)
    {
        //기존 큐 파일 시스템이 있다면 삭제
        mq_unlink(name);

        mfd = mq_open(name, O_RDWR | O_CREAT | O_EXCL,  0666, &attr);
        if (mfd == -1)
        {
            perror("open error tpye1");
            return -1;
        }
    } 
    else
    {
        mfd = mq_open(name, O_WRONLY, 0666, &attr);
        if(mfd == -1)
        {
            perror("open error type2");
            return -1;
        }
    }
    return mfd;
}

int RcvQueue(int fd, char *ptr, long size)
{
    if((mq_receive(fd, ptr, size,NULL)) == -1)
    {
        return -1;
    }

     return 1;
}

int SendQueue(int fd, char *ptr, long size)
{
    if((mq_send(fd, ptr, size, 1)) == -1)
    {
        perror("send error ");
        return -1;
    }

    return 1;
}

