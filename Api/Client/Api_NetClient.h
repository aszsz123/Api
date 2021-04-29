//#pragma once
// compiler(preprocessor) 가 해당 파일을 한번만 읽도록 하는 명령 
// 몇몇 preprocessor 에서는 이해할 수 없을 수도 있다고 명시되어 있음.
// #ifndef Style도 C preprocessor에서 기억하는 기능이 있어 한번만 읽도록 하는 기능이 가능하다.

#ifndef __API_NETCLIENT_H__
#define __API_NETCLIENT_H__
#include <sys/socket.h>

#define NETCLIENT_MAXSIZE	1000
#define NETBUF_MAXSIZE	 	8192

enum{
	SOCK_NO1=0,
	SOCK_NO2,
	SOCK_NO3,

	SOCK_NO_MAX,
};

typedef struct st_netData {
	unsigned int port;
	unsigned int length;
	unsigned int respTarget;
	char data[NETBUF_MAXSIZE];
}__attribute__((packed))netData_t;


int GetQueueTcpRcvData(netData_t *ptr);

int SetServerInfo(int sockNo, char *ip, int port, char *dev);

int StartNetClientProcess(void);
int StopNetClientProcess(void);

int UdpSendData(char *ip, char *port, char *data, int len,int cnt);
#endif
