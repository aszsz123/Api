#include <termios.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

#include <pthread.h>

#include <arpa/inet.h>

#include "Api_NetClient.h"
#include "Api_Menu.h"

#include "Client_Menu.h"

void printSpace(int num)
{
    int i = 0;
    printf("\r");
    for(i=0; i<num; i++)
    {
        printf(" ");
    }
}

int InputNetInfo_Port(char *netInfo)
{
    char buf[100];
    int  k;
    int stack=0;
    int dotCnt=0;
    system("clear");
    printf("Input NetInfo Port \r\n");
    printf("Only Use '0'~'9'\r\n");
    memset(buf,0,100);
    while(1)
    {
        k = getch();
        switch(k)
        {
            case 8: // BackSpace
                if(stack > 0 )
                {
                    buf[stack-1] = 0;

                    stack--;
                    printSpace(stack + sizeof("  > Input Info[%d] :") );
                    printf("\r > Input Info[%d] : %s",stack, buf);
                }
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                if(stack < 5)
                {
                    buf[stack++] = k;
                    printSpace(stack + sizeof("  > Input Info[%d] :") );
                    printf("\r > Input Info[%d] : %s",stack, buf);
                }
                break;
            case 'q':
                return -1;
                break;
            case 13: //\r, \n
            case 10:
                printf("\r\n"); //
                if( stack < 1)
                {
                    printf("Wrong Network Info[%s] [%d][%d]\r\n", buf, dotCnt, stack);
                    memset(buf,0,100);
                    stack = 0;
                }
                else
                {
                    printf("\r\nSet Info[%s]\r\n", buf);
                    memcpy(netInfo, buf, stack);

                    return 1;
                }
                break;
            default :
                printf("Wrong Input : Only Use '0'~'9' , Exit : 'q'\r\n");
                break;
        }
    }

}
int InputNetInfo_IPv4(char *netInfo)
{

    char buf[20];
    int  k;
    int stack=0;
    int dotCnt=0;
    system("clear");
    printf("Input NetInfo \r\n");
    printf("Only Use '0'~'9' or '.'\r\n");
    memset(buf,0,16);
    while(1)
    {
        k = getch();
        switch(k)
        {
            case 8: // BackSpace
                if(stack > 0 )
                {
                    if(buf[stack-1]== '.')
                    {
                        dotCnt--;
                    }
                    buf[stack-1] = 0;

                    stack--;
                    printf("\r                                        ");
                    printf("\r > Input Info[%d] : %s",stack, buf);
                }
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                if(stack < 15)
                {
                    buf[stack++] = k;
                    printf("\r                                      ");
                    printf("\r > Input Info[%d] : %s",stack, buf);
                }
                break;
            case '.':
                if(dotCnt < 3)
                {
                    dotCnt++;
                    if(stack < 15)
                    {
                        buf[stack++] = k;
                        printf("\r                                        ");
                        printf("\r > Input Info[%d] : %s",stack, buf);
                    }
                }
                break;
            case 'q':
                return -1;
                break;
            case 13:
            case 10:
                printf("\r\n");
                if(dotCnt != 3 || stack < 7)
                {
                    printf("Wrong Network Info[%s] [%d][%d]\r\n", buf, dotCnt, stack);
                    memset(buf,0,20);
                    stack = 0;
                }
                else
                {
                    printf("\r\nSet Info[%s]\r\n", buf);
                    memcpy(netInfo, buf, stack);

                    usleep(100*1000);
                    return 1;
                }
                break;
            default :
                printf("Wrong Input : Only Use '0'~'9' or '.' , Exit : 'q'\r\n");
                break;
        }
    }

}
int InputNetInfo_IPv6(char *netInfo)
{
    char buf[100];
    int  k;
    int stack=0;
    int dotCnt=0;
    system("clear");
    printf("Input NetInfo IPv6 \r\n");
    printf("Only Use '0'~'9', 'a'~'f' 'A'~'F' or ':'\r\n");
    memset(buf,0,100);
    while(1)
    {
        k = getch();
        switch(k)
        {
            case 8: // BackSpace
                if(stack > 0 )
                {
                    if(buf[stack-1]== ':')
                    {
                        dotCnt--;
                    }
                    buf[stack-1] = 0;

                    stack--;

                    printSpace(stack + sizeof("  > Input Info[%d] :") );
                    printf("\r > Input Info[%d] : %s",stack, buf);
                }
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
                if(stack < 40)
                {
                    buf[stack++] = k;
                    printSpace(stack + sizeof("  > Input Info[%d] :") );
                    printf("\r > Input Info[%d] : %s",stack, buf);
                }
                break;
            case ':':
                if(dotCnt < 7)
                {
                    dotCnt++;
                    if(stack < 40)
                    {
                        buf[stack++] = k;
                        printf("\r > Input Info[%d] : %s",stack, buf);
                    }
                }
                break;
            case 'q':
                return -1;
                break;
            case 13: //\r, \n
            case 10:
                printf("\r\n"); // IPv6 최소 입력: ::1
                if(dotCnt < 2 || stack < 3)
                {
                    printf("Wrong Network Info[%s] [%d][%d]\r\n", buf, dotCnt, stack);
                    memset(buf,0,100);
                    stack = 0;
                }
                else
                {
                    printf("\r\nSet Info[%s]\r\n", buf);
                    memcpy(netInfo, buf, stack);

                    usleep(100*1000);
                    return 1;
                }
                break;
            default :
                printf("Wrong Input : Only Use '0'~'9' 'a'~'f', 'A'~'F'or '.' , Exit : 'q'\r\n");
                break;
        }
    }

}



int InputNetInfo_dev(char *netInfo)
{
    char buf[100];
    int  k;
    int stack=0;
    system("clear");
    printf("Input NetInfo Dev\r\n");
    memset(buf,0,100);
    while(1)
    {
        k = getch();
        switch(k)
        {
            case 8: // BackSpace
                if(stack > 0 )
                {
                    buf[stack-1] = 0;

                    stack--;

                    printSpace(stack + sizeof("  > Input Info[%d] :") );
                    printf("\r > Input Info[%d] : %s",stack, buf);
                }
                break;
            case 'q':
                return -1;
                break;
            case 13: //\r, \n
            case 10:
                printf("\r\n"); // IPv6 최소 입력: ::1
                printf("\r\nSet Info[%s]\r\n", buf);
                memcpy(netInfo, buf, stack);

                usleep(100*1000);
                return 1;
            default :
                if(stack < 16)
                {
                    buf[stack++] = k;
                    printSpace(stack + sizeof("  > Input Info[%d] :") );
                    printf("\r > Input Info[%d] : %s",stack, buf);
                }
                break;
        }
    }

}
int SetServerIPv4(void)
{
    char ip[100];
    char port[10];
    char dev[40];
    memset(ip, 0, 100);
    memset(port, 0, 10);
    memset(dev, 0, 40);

    if(InputNetInfo_IPv4(ip) == -1)
        return -1;

    if(InputNetInfo_Port(port) == -1)
        return -1;

    SetServerInfo(SOCK_NO3, ip, atoi(port), NULL);
    return 1;
}


int SetServerIPv6(void)
{
    char ip[100];
    char port[10];
    char dev[40];
    memset(ip, 0, 100);
    memset(port, 0, 10);
    memset(dev, 0, 40);

    if(InputNetInfo_IPv6(ip) == -1)
        return -1;

    if(InputNetInfo_Port(port) == -1)
        return -1;

    if(InputNetInfo_dev(dev) == -1)
        return -1;

    SetServerInfo(SOCK_NO3, ip, atoi(port), dev);
    return 1;
}

int TermCmd(void)
{

    char buf[256];
    int  k;
    int stack=0;
    system("clear");
    printf("Input cmd\r\n");
    memset(buf,0,256);

    while(1)
    {
        k = getch();
        switch(k)
        {
            case 8: // BackSpace
                if(stack > 0 )
                {
                    buf[stack-1] = 0;

                    stack--;

                    system("clear");
                    printSpace(stack + sizeof("  > Input cmd[%d] :") );
                    printf("\r > Input cmd[%d] : %s",stack, buf);
                }
                break;
            case '+':
                return 1;
                break;
            case 13: //\r, \n
            case 10:
                printf("\r\n");
                system(buf);
                memset(buf,0,256);
                stack =0;
                printf("\r > Input cmd[%d] : %s",stack, buf);
                break;
            default :
                if(stack < 256)
                {
                    system("clear");
                    buf[stack++] = k;
                    printSpace(stack + sizeof("  > Input cmd[%d] :") );
                    printf("\r > Input cmd[%d] : %s",stack, buf);
                }
                break;
        }
    }
}
static void *clientMenu [][4] = {
    {(void *)SetServerIPv6,     "SET IPv6 SERVER", NULL, NULL  },
    {(void *)SetServerIPv4,     "SET IPv4 SERVER", NULL, NULL  },
    {(void *)TermCmd,           "BASH CMD MODE"  , NULL, NULL  },

        {0,0}
};

static int* ThreadTestMenu(void)
{
    int *ret=0;
    static menuInfo_t menuMain;
    
    memset(&menuMain, 0, sizeof(menuInfo_t));

    char help[]=".\n";
    char title[] = "Net Client Menu Ver 0.1";
    
    menuMain.name = title;
    menuMain.help = help;

    menuMain.mode = MENU_MODE_MAIN;

    DisplayMenu(clientMenu, &menuMain);

    
    return ret;
}


int StartClientMenu(void)
{

    sleep(4);
    pthread_t testMenu;


    if( pthread_create(&testMenu, NULL, (void*)ThreadTestMenu, NULL) < 0 )
    {
        printf("#### thread create error : ThreadTestMenu");
        exit(1);
    }
    else
        pthread_detach(testMenu);


    return 1;
}
