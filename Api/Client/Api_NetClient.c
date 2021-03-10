#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <pthread.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>

#include <fcntl.h>
#include <unistd.h>

#include <poll.h>
#include <assert.h>

#include "Api_MsgQueue.h"
#include "Api_NetClient.h"

// ----------------------------------------------------------------------------------------------------- //
// Debugging Option...																					 //
// ----------------------------------------------------------------------------------------------------- //
#define xxx_DEBUG

#ifdef xxx_DEBUG
#define dp(fmt, args...) printf(fmt,## args)
#define dlp(fmt,args...) printf("[%s,%s(),%d]"fmt,__FILE__,__FUNCTION__,__LINE__,## args)
#else
#define dp(fmt, args...)
#define dlp(fmt,args...)
#endif
// ----------------------------------------------------------------------------------------------------- //
#ifndef FLAG_ON
    #define FLAG_ON 1
#endif

#ifndef FLAG_OFF
    #define FLAG_OFF 0
#endif

typedef struct st_netClient {
	unsigned int servIp;
	int servPort;

    char ipv6[16];
    char dev[16];

    char ipVersion;
}st_netClient_t;

typedef union uni_netClient {
	st_netClient_t mem;
	char buff[sizeof(st_netClient_t)];
}netClient_t;

static msgQueue_t queueTcpClient[SOCK_NO_MAX];

static int PutQueueNetClientData(int qNum, netClient_t *ptr)
{
	return PutQueue(&queueTcpClient[qNum],ptr);
}

static int GetQueueNetClientData(int qNum, netClient_t *ptr)
{
	return GetQueue(&queueTcpClient[qNum],ptr);
}
// ----------------------------------------------------------------------------------------------------- //
static msgQueue_t queueTcpRcvData;
static msgQueue_t queueTcpTxData;

int PutQueueTcpRcvData(netData_t *ptr)
{
	return PutQueue(&queueTcpRcvData,ptr);
}

int GetQueueTcpRcvData(netData_t *ptr)
{
	return GetQueue(&queueTcpRcvData,ptr);
}

int PutQueueTcpTxData(netData_t *ptr)
{
	return PutQueue(&queueTcpTxData,ptr);
}

int GetQueueTcpTxData(netData_t *ptr)
{
	return GetQueue(&queueTcpTxData,ptr);
}

// ----------------------------------------------------------------------------------------------------- //



/* State Client */
typedef struct netInfo_st{
	int connection;
	int sockFd;
}netInfo_t;

void PrintDataAll(char *data, int len, char *name)
{
    int num=0;
    
    printf("\r\n===========================================\r\n");
    printf("   %s", name);
    printf("\r\n-------------------------------------------\r\n");
    for(num = 0; num<len; num++)
    {
        printf("[%02X]", (unsigned char)data[num]);
    }

    printf("\r\n-------------------------------------------\r\n");
    for(num = 0; num<len; num++)
    {
        printf("%c", data[num]);
    }
    printf("\r\n===========================================\r\n");
}

static netInfo_t netClient[SOCK_NO_MAX];

static struct pollfd pollClient[SOCK_NO_MAX];

int SendTcpData(int sockNo, char *data, unsigned int length)
{
	int num=0;

	if( data == NULL )
		return -1;

	if(sockNo == 0xFF)
	{
		for(num=0; num<SOCK_NO_MAX; num++)
		{
			if( netClient[num].sockFd > 0)
			{
				if(write(netClient[num].sockFd, data, length) > 0)
					printf("[OK] Send tcp server[%d]\n",num);
				else
					printf("[FAIL] Send tcp server[%d]\n",num);
			}			
		}
	}
	else
	{
		if( netClient[sockNo].sockFd > 0 )
		{
			if(write(netClient[sockNo].sockFd, data, length) > 0)
				printf("[OK] Send tcp server[%d]\n",sockNo);
			else
				printf("[FAIL] Send tcp server[%d]\n",sockNo);
		}
	}
	return 1;
}

int DisconnectServer(int sockNo)
{
    printf("[OK] Disconnection (%d)(%d) \r\n", sockNo, netClient[sockNo].sockFd);
    memset(&pollClient[sockNo], -1, sizeof(struct pollfd) );
    
    netClient[sockNo].connection = FLAG_OFF;
    
    shutdown(netClient[sockNo].sockFd, SHUT_RDWR);
    return close(netClient[sockNo].sockFd);
}

static int CheckIpVersion(char *ip, int len)
{
	int num=0;
	
	int dotCnt=0;
	int colCnt=0;

	for(num=0; num<len; num++)
	{
		switch(ip[num])
		{
			case '.':
				dotCnt++;
				break;
			case ':':
				colCnt++;
				break;
			default: 
				break;
		}
	}

	if(dotCnt == 3)
		return 1; //IPv4

	if(colCnt >= 2 && colCnt <=7)
		return 2; //IPv6
	
	//다 아니면

	return -1;
}

int SetServerInfo(int sockNo, char *ip, int port, char *dev)
{ 
    netClient_t setInfo; 
	
    char ipv6_str[40];
    char ipv6_dev[40];

    char ret=0;

	ret = CheckIpVersion(ip, strlen(ip) );


	if(ret == 2)// IPv6
	{
        printf("[IP CHECK] IPv6\n");
        printf("[IP CHECK] %d\n", ret);
		memset(ipv6_str,0,40);
		memset(ipv6_dev,0,40);


		memset(&setInfo, 0x00, sizeof(netClient_t) );
		memcpy(setInfo.mem.dev, dev, strlen(dev) );
		printf("[OK] Set IPv6 Dev: [%s]\r\n", setInfo.mem.dev);

		//IP 변환 -> str to bin
		ret = inet_pton(AF_INET6, ip, setInfo.mem.ipv6);
		if(ret == -1)
		{
			printf("[FAIL] Set Server IP: [%s] \r\n", ip);
            return -1;
		}


		//IP 변환 -> bin to str
		inet_ntop(AF_INET6, setInfo.mem.ipv6, ipv6_str, 40);

		setInfo.mem.servPort = port;
        setInfo.mem.ipVersion = 2;
		printf("[OK] Set Server IP: [%s]\n", ipv6_str); 
	}
	else if(ret == 1)// IPv4
	{
        printf("[IP CHECK] IPv4\n");
        printf("[IP CHECK] %d\n", ret);
        setInfo.mem.servIp = inet_addr(ip);

        if((int)setInfo.mem.servIp == -1)
        {
			printf("[FAIL] Set Server IP: [%s] \r\n", ip);
            return -1;
        }
        else
            printf("[OK] Set Server IP: [%s] \r\n", ip);

        setInfo.mem.servPort = port;
        setInfo.mem.ipVersion = 1;
    }
	else
	{
        printf("[FAIL] Set Server IP: [%s] \r\n", ip);
		return -1;
	}

    if( PutQueueNetClientData(sockNo,&setInfo) < 0 )
    {
        printf("[FAIL] Put Msg Queue Tcp Set Info Overflow\r\n");
    }

    return 1;
}

int ConnectServer(int sockNo, netClient_t *servInfo)
{
    struct linger solinger;
    struct sockaddr_in6 addr_Ipv6;
    struct sockaddr_in addr_Ipv4;
    struct in_addr in;

    char ipv6_str[40];

    memset(ipv6_str,0,40);
    memset(&addr_Ipv6, 0, sizeof(struct sockaddr_in6));
    memset(&addr_Ipv4, 0, sizeof(struct sockaddr_in));
    printf("[  ] ConnectServer Proc \n");
    /*서버 연결 되어 있다면 끊고 시작*/
    if(netClient[sockNo].connection == 1)
        DisconnectServer(sockNo);

    printf("[  ] %d\r\n", servInfo->mem.ipVersion);
    if (servInfo->mem.ipVersion == 1) //IPv4
    {
        printf("[IP CHECK] IPv4\r\n");
        netClient[sockNo].sockFd = socket(AF_INET,SOCK_STREAM, 0);

        if (netClient[sockNo].sockFd < 0)
        {
            printf("[FAIL] Socket Create[%d] :", sockNo);
            perror(" ");
            return -1;
        }
        printf("[OK] Socket Create[%d] (%d) \r\n", sockNo, netClient[sockNo].sockFd);

        solinger.l_onoff = 1;
        solinger.l_linger = 0;
        if( setsockopt(netClient[sockNo].sockFd,SOL_SOCKET,SO_LINGER,&solinger,sizeof(struct linger)) == -1 )
        {
            perror("[FAIL] Set Sockopt :");
            close(netClient[sockNo].sockFd);
            return -1;
        }

        printf("[OK] Set Sockopt\r\n");

        addr_Ipv4.sin_family = AF_INET;
        addr_Ipv4.sin_addr.s_addr = servInfo->mem.servIp;
        addr_Ipv4.sin_port = htons(servInfo->mem.servPort);

        in.s_addr = servInfo->mem.servIp;

        int len = sizeof(addr_Ipv4);

        /*서버 소켓 연결*/
        printf("[START] Connecting Server\r\n");
        printf("        IP      : [%s] \r\n", inet_ntoa(in));
        printf("        PORT    : [%d] \r\n", servInfo->mem.servPort );
        if(connect(netClient[sockNo].sockFd, (struct sockaddr *)&addr_Ipv4, len) < 0)
        {
            perror("[FAIL] Connect Error :");
            DisconnectServer(sockNo);

            return -1;
        }

        netClient[sockNo].connection = FLAG_ON;

        printf("[OK] Connect Success\n");
        usleep(300*1000);
    }
    else if(servInfo->mem.ipVersion == 2) //IPv6
    {
        printf("[IP CHECK] IPv6\r\n");
        //IP 변환 -> bin to str
        inet_ntop(AF_INET6, servInfo->mem.ipv6, ipv6_str, 40);
        printf("[OK] Connet IP: %s\r\n", ipv6_str);
        printf("[OK] Use Dev: %s\r\n", servInfo->mem.dev);

        /*소켓 생성*/
        netClient[sockNo].sockFd = socket(AF_INET6, SOCK_STREAM, 0);
        if (netClient[sockNo].sockFd < 0)
        {
            printf("[FAIL] Socket Create[%d] :", sockNo);
            perror(" ");
            return -1;
        }
        printf("[OK] Socket Create[%d] (%d) \r\n", sockNo, netClient[sockNo].sockFd);

        /*소켓 옵션 설정*/
        /*
           [l_onoff = 1, l_linger = 0의 경우]
           close 함수를 호출하면, 이후 해당 소켓으로부터 자료의 수신이나 송신이 추가적으로 이루어지지 않는다. 
           상대방에게 RST 패킷을 전송하고, 자신의 소켓은 CLOSED 상태가 된다. 
           커널의 송수신 버퍼에 보관 중인 내용은 즉시 모두 폐기된다.

           [l_onoff = 1, l_linger != 0 의 경우]
           close 함수를 호출하면, 이후 해당 소켓으로부터 자료의 수신이나 송신이 추가적으로 이루어지지 않는다. 
           커널의 송신 버퍼에 아직 전송되지 않고 남아 있던 자료를 상대방에세 전송하고 FIN 패킷을 전송한다.
           close 함수를 호출한 응용 프로그램은 close 함수를 호출한 후에 커널에서 바로 반환되지 않고, 
           상대방으로부터 FIN_ACK 패킷을 받을 때까지 커널에서 대기한다. 
           옵션 l_linger의 값으로 설정한 시간 내에 FIN_ACK 패킷을 받지 못하면 EWOULDBLOCK를 반환하면서 응용 프로그램으로 복귀한다.
           */
        solinger.l_onoff = 1;
        solinger.l_linger = 0;
        if( setsockopt(netClient[sockNo].sockFd,SOL_SOCKET,SO_LINGER,&solinger,sizeof(struct linger)) == -1 )
        {
            perror("[FAIL] Set Sockopt :");
            close(netClient[sockNo].sockFd);
            return -1;
        }

        printf("[OK] Set Sockopt\r\n");


        /*서버 정보 입력*/
        addr_Ipv6.sin6_family   = AF_INET6;
        addr_Ipv6.sin6_flowinfo = 0;
        addr_Ipv6.sin6_port     = htons(servInfo->mem.servPort);
        addr_Ipv6.sin6_scope_id = if_nametoindex(servInfo->mem.dev);
        memcpy(&addr_Ipv6.sin6_addr, servInfo->mem.ipv6, 16);

        int len = sizeof(addr_Ipv6);

        /*서버 소켓 연결*/
        printf("[START] Connecting Server\r\n");
        printf("        IP      : [%s] \r\n", ipv6_str);
        printf("        PORT    : [%d] \r\n", servInfo->mem.servPort );
        printf("        USE DEV : [%s] \r\n", servInfo->mem.dev);
        if(connect(netClient[sockNo].sockFd, (struct sockaddr *)&addr_Ipv6, len) < 0)
        {
            perror("[FAIL] Connect Error :");
            DisconnectServer(sockNo);

            return -1;
        }

        netClient[sockNo].connection = 1;

        printf("[OK] Connect Success\n");
        usleep(300*1000);
    }
    else
    { 
        printf("[  ] ConnectServer Pasdfasdfasdfasdfadfa \n");
    }

    return 1;
}

int RcvProc(int sockNo)
{
    netData_t rcvData;
    netData_t test;

    int retRcv=0;

    memset(&rcvData, 0x00, sizeof(netData_t) );
    memset(&test, 0x00, sizeof(netData_t) );

    if( pollClient[sockNo].revents & POLLIN )
    {
        retRcv = recv(pollClient[sockNo].fd,rcvData.data, NETBUF_MAXSIZE, 0);

        if( retRcv < 0 )  /* Incorrecting shutdown */
        {
            printf("[TCP CLIENT_IPv6] Disconnect - Incorrecting Shutdown\r\n");
            DisconnectServer(sockNo);
        }
        else if( retRcv == 0 )    /* Graceful shutdown */
        {
            printf("[TCP CLIENT_IPv6] Disconnect - Graceful Shutdown\r\n");
            DisconnectServer(sockNo);
        }
        else    /* receive data */
        {
            rcvData.length = strlen(rcvData.data);
            rcvData.port = sockNo;
          
            printf("\r\n====================================================\r\n");
            printf("->[%s]\r\n", rcvData.data);
            printf("\r\n====================================================\r\n");
#if 1 //Echo
            PutQueueTcpRcvData(&rcvData);

            if(GetQueueTcpRcvData(&test) == FLAG_ON)
            {
                test.respTarget=sockNo;
                printf("\r\n====================================================\r\n");
                printf("Num[%d], Length[%d] target[%d]\r\n", test.port, test.length, test.respTarget);
                printf("[%s]\r\n", test.data);
                printf("\r\n====================================================\r\n");
                PutQueueTcpTxData(&test);
            }
#endif
        }
    }
    else
    {
        //미처리
    }
    return 1;
}

int *ThreadTcpRxClient(int sockNo)
{
    sleep(2);
    netClient_t getServInfo;
    netClient_t curServInfo;
    netClient_t tmpServInfo;

    int retPoll=0;
    memset(&getServInfo, 0x00, sizeof(netClient_t) );
    memset(&curServInfo, 0x00, sizeof(netClient_t) );
    memset(&tmpServInfo, 0x00, sizeof(netClient_t) );

    memset(&pollClient[sockNo], -1, sizeof(struct  pollfd) );
   
    netClient[sockNo].connection = FLAG_OFF;
    
    printf("[OK] Thread Create : TCP RX [%d]\r\n", sockNo);
    InitQueue(&queueTcpRcvData,NETCLIENT_MAXSIZE,sizeof(netData_t),"TCP RCV DATA");

    InitQueue(&queueTcpClient[sockNo],3,sizeof(netClient_t),"NetServer Info");
    while(1)
    {
        /** 서버 정보 입력 시 **/
        if( GetQueueNetClientData(sockNo,&getServInfo) == FLAG_ON ) /* receive new server info */
        {
            printf("[OK] Get Set Info \r\n");
            memcpy(&tmpServInfo, &getServInfo, sizeof(netClient_t) );

            /*서버 연결*/
            if(ConnectServer(sockNo, &getServInfo) == 1)
            {
                //성공 시 NetInfo 복사
                memset(&curServInfo, 0, sizeof(netClient_t) );
                memcpy(&curServInfo, &getServInfo, sizeof(netClient_t) );

                //Poll 등록
                pollClient[sockNo].fd = netClient[sockNo].sockFd; 
                pollClient[sockNo].events = POLLIN;
                
            }
            
            memset( &getServInfo, 0x00, sizeof(netClient_t) );
        }

        if(netClient[sockNo].connection == FLAG_ON)
        {
            /** 서버 연결 후 POLL 동작 **/
            retPoll = poll(&pollClient[sockNo], 1, 1000);

            switch(retPoll)
            {
                case -1: //POLL ERR
                    printf("[FAIL] POLL ERR\r\n");
                    break;
                case 0:  //POLL 타임아웃
                    //printf("# POLL TIMEOUT\r\n");
                    break;
                default: //POLL 이벤트 
                    RcvProc(sockNo);
                    break;
            }
        }
        else
        {
            /** 서버 연결 안되어 있을 때 **/
           
            
            /*재 연결 시도 */
            if(tmpServInfo.mem.ipv6[0] != 0 && tmpServInfo.mem.servIp != 0 &&tmpServInfo.mem.servPort != 0)
            {
                printf("=========================================\r\n");
                printf("[TCP RX] Retry Connection\r\n");
            
                if(ConnectServer(sockNo, &tmpServInfo) == 1)
                {
                    //성공 시 NetInfo 복사
                    memset(&curServInfo, 0, sizeof(netClient_t) );
                    memcpy(&curServInfo, &tmpServInfo, sizeof(netClient_t) );

                    //Poll 등록
                    pollClient[sockNo].fd = netClient[sockNo].sockFd; 
                    pollClient[sockNo].events = POLLIN;

                    memset( &tmpServInfo, 0x00, sizeof(netClient_t) );

                }
            }
 
            usleep(3000*1000);
        }
    }
    close(netClient[sockNo].sockFd);
}

int* ThreadTcpClientTx(void)
{
    int* ret;
    netData_t txData;

    InitQueue(&queueTcpTxData,NETCLIENT_MAXSIZE,sizeof(netData_t),"TCP CLIENT");

    sleep(1);
    printf("[OK] Thread Create Tcp Tx\r\n");

    while(1)
    {
        if( GetQueueTcpTxData(&txData) == FLAG_ON )
        {
            printf("port[%d]->terget[%d] len[%d] data[%s]\r\n",txData.port, txData.respTarget, txData.length, txData.data );
            PrintDataAll(txData.data, txData.length, "Send Tcp Data"); 
            SendTcpData(txData.respTarget, txData.data, txData.length);

            memset(&txData, 0x00, sizeof(netData_t));
        }
        usleep(100*1000);
    }


    return ret;
}

int UdpSendData(char *ip, char *port, char *data, int len,int cnt)
{
    int client_socket;
    struct sockaddr_in serverAddress;
    unsigned int server_addr_size;
    char readBuff[256];
    
    struct pollfd udpPoll;

    ssize_t receivedBytes;
    int retPoll=0;
    
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    inet_aton(ip, (struct in_addr*) &serverAddress.sin_addr.s_addr);
    serverAddress.sin_port = htons(atoi(port));

    // 소켓 생성
    if ((client_socket = socket(PF_INET, SOCK_DGRAM, 0)) == -1)
    {
        printf("[FAIL]socket Create UDP\n");
        return -1;
    }


    printf("[OK]socket Create UDP\n");


    udpPoll.fd = client_socket;
    udpPoll.events = POLLIN;
    
    server_addr_size = sizeof(serverAddress);
    sendto(client_socket, data, len, 0, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
        
    PrintDataAll(data, len, "UDP SEND");
    while(1)
    {
        retPoll = poll(&udpPoll, 1, 1000);
        
        switch(retPoll)
        {
            case -1:
            case 0:
                cnt--;
                PrintDataAll(data, len, "UDP SEND");
                sendto(client_socket, data, len, 0, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
                if(cnt <= 0)
                {
                    printf("[UDP] Timeout Exit\r\n");
                    close(client_socket);
                    return -1;
                }
                break;
            default :
               if( udpPoll.revents & POLLIN)
               {
                 receivedBytes = recvfrom(client_socket, readBuff, 256, 0, (struct sockaddr*)&serverAddress, &server_addr_size);

                 PrintDataAll(readBuff, receivedBytes, "UDP RCV");
                 close(client_socket);
                 return 1;
               }
               break;
        }
    }
    // 소켓 close
}




static pthread_t thdNetClientTx;
static pthread_t thdNetClientRx[SOCK_NO_MAX];
static int thdNetClientRx_Init[SOCK_NO_MAX];

int StartNetClientProcess(void)
{
	if( pthread_create(&thdNetClientTx,NULL,(void *)ThreadTcpClientTx,NULL) < 0 )
	{
		printf("#### thread create error : ThreadTcpClientTx");
		exit(1);
	}
    else
        pthread_detach(thdNetClientTx);

    if( pthread_create(&thdNetClientRx[SOCK_NO3],NULL,(void *)ThreadTcpRxClient,(void*)SOCK_NO3) < 0 )
	{
		printf("#### thread create error : ThreadTcpClient");
		exit(1);
	}
    else
    {
        pthread_detach(thdNetClientRx[SOCK_NO3]);
        thdNetClientRx_Init[SOCK_NO3] = FLAG_ON;
    }



	return 1;
}

int StopNetClientProcess(void)
{
    int num = 0;

	if( pthread_cancel(thdNetClientTx) < 0 )
		printf("#### thread exit fail\n");

    for(num = 0; num< SOCK_NO_MAX; num++)
    {
        if(thdNetClientRx_Init[SOCK_NO3] == FLAG_ON)
        {
            if( pthread_cancel(thdNetClientRx[num]) < 0 )
                printf("#### thread exit fail\n");
            else
                thdNetClientRx_Init[SOCK_NO3] = FLAG_OFF;
        }
    }
	return 1;
}

