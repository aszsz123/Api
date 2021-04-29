/*	 
		Menu Api 
		
		Version: 1.0
		
		Made By Jose		
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#include "Api_Menu.h"

int Pow(int x, int y)
{
	int i;
	int result;

	if( y == 0 )
	{
		result = 1;
		return result;
	}

	result = x;

	for( i = 1 ; i < y ; i++ )
	{
		result = result * x;
	}

	return result;
}

int StrToNum(char *string)
{
	int loop = 0, subLoop = 0;
	int buffer = 0;
	char temp = 0;
	int length = 0;

	while(string[length] >= '0' && string[length] <= '9')
	{
		length++;
	}

	if( length == 0 )
	{
		return -1;
	}

	for( subLoop = length-1 ; subLoop > 0 ; subLoop-- )
	{
		for( loop = 0 ; loop < subLoop ; loop++ )
		{
			temp = string[loop];
			string[loop] = string[loop+1];
			string[loop+1] = temp;
		}
	}

	for( loop = 0 ; loop < length ; loop++ )
	{
		if( string[loop] >= '0' && string[loop] <= '9' )
		{
			buffer += (string[loop] - '0') * (Pow(10,loop));
		}
		else
		{
			printf("not number loop - %d\n",loop);

			return -1;
		}
	}

	return buffer;
}



int getch(void)
{
	int ch;
	struct termios buf, save;

	tcgetattr(0,&save);

	buf = save;
	buf.c_lflag &= ~(ICANON|ECHO);
	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME] = 0;

	tcsetattr(0, TCSAFLUSH, &buf);
	ch = getchar();
	tcsetattr(0, TCSAFLUSH, &save);

	return ch;
}

#define KEY_ENTER_LF    10
#define KEY_ENTER_CR    13
#define KEY_BACK        8
#define KEY_TAP			9
#define KEY_ESC			27

/*함부로 변경하지 말것*/
#define SIZE_NUM            6 //!고정
#define SIZE_LINE_ARROW     1 //!고정
#define SIZE_SEL            3 //!고정

#define POS_NUM             1
#define POS_LINE_ARROW  POS_NUM+SIZE_NUM
#define POS_FUNCNAME    POS_LINE_ARROW+SIZE_LINE_ARROW              //
#define POS_DATA        POS_FUNCNAME+SIZE_FUNCNAME+POS_DATA_OFFSET  //데이타 출력 포지션
#define POS_SEL         MENU_LINE_SIZE+POS_SEL_OFFSET-SIZE_SEL      //라인 포인터 출력 포지션

#define MENU_LINE_SIZE  POS_DATA+SIZE_SEL+SIZE_NUM+SIZE_DATA  //메뉴 라인 틀 사이즈*



void MakeBaseStr(char *out )
{
	int loop;

	memset(out, 0, 1024);
	out[0]  = '|';
	for(loop=1;loop<MENU_LINE_SIZE;loop++)
	{
		out[loop]=' ';
	}
	out[loop] = '|';
}

void PrintLineStr(void)
{
	char out[1024];
	int loop;
	memset(out, 0, 1024);	
	out[0]  = '+';
	for(loop=1;loop<MENU_LINE_SIZE;loop++)
	{
		out[loop]='-';
	}
	out[loop] = '+';

	printf("%s\r\n",out);
}

void PrintEtcStr(char *input, int startPos)
{
	char out[1024];


	memset(out, 0, 1024);
	MakeBaseStr(out);

	memcpy(&out[startPos], input, strlen(input) );
	printf("%s\r\n",out);

}



int PrintHelpStr(char *input, int startPos)
{
	char out[1024];
	int cnt=0;
	char k=0;
	
	int stack=0;
	memset(out, 0, 1024);

	while(1)
	{
		k = input[cnt];
		switch(k)
		{
			/*데이터가 NULL 이면*/
			case 0:
				if(out[0] == 0)
				{
					//출력할 데이터가 없음 
					return 1;
				}
				
				//현재 버퍼 출력
				PrintEtcStr(out, startPos);
				return 1;				
				break;
			/*줄바꿈*/
			case KEY_ENTER_LF:
				//현재 버퍼 출력
				PrintEtcStr(out, startPos);
				memset(out, 0, 1024);
				stack=0;
				break;
			/*케리지 리턴*/
			case KEY_ENTER_CR:
				//아무것도 안함
				break;
			/* TAP 입력*/
			case KEY_TAP:
				//아무것도 안함
				break;	
			/*문자 일때*/
			default :
				if(stack > MENU_LINE_SIZE-2-startPos )
				{
					//메뉴 라인 사이즈가 넘어서 출력
					PrintEtcStr(out, startPos);
					memset(out, 0, 1024);
					stack=0;
				}
				//데이터를  버퍼에 저장
				out[stack++] = k;	
		//		printf("Get[%c]\r\n",out[stack-1]);
				break;
		}	

		cnt++;
	}
}
void PrintMenuStr(char *funcName, char *attu, char *sel, int line, int lastLine, int curLine)
{
	char out[1024];
	memset(out, 0, 1024);
	char lineStr[10];	

	MakeBaseStr(out);
	/* line Num */
	memset(lineStr,0,10);
	sprintf(lineStr," %3d: ",line);
	if(line != lastLine)
	{
		memcpy(&out[POS_NUM], lineStr, strlen(lineStr) );
	}
	/* 현재 라인 표시 */
	if(line ==  curLine)
	{
		memcpy(&out[POS_LINE_ARROW],">",1);
	}

	/* Function Name */
	if(funcName != NULL)
	{    
		memcpy(&out[POS_FUNCNAME], funcName, strlen(funcName));
	}
	/* Data */
	if(attu != NULL)
	{
		memcpy(&out[POS_DATA], attu, strlen(attu) );
	}
	/* Select */
	if(sel != NULL)
	{
		memcpy(&out[POS_SEL], sel, strlen(sel));
	}
	printf("%s\r\n", out);
}
int DisplayMenu(void *(*menu)[4], menuInfo_t *info)
{   
	system("clear");
	int i=0;
	int selFunc;
	int menu_num;

	char temp[256];
	char dummy;
	int k;
	char key_data[10];
	int stack=0;
	int type=0;
	int line=0;

	char funcStr[30];
	char data[100];
	char sel[4];


	PrintLineStr();
	while(1)
	{
		while(1)
		{
			if((int*)(menu[i][0]) == 0)
			{
				menu_num = i;
				i = 0;
		//		printf("%d\r\n",menu_num);
				break;
			}
			else
			{
				i++;
			}

		}

		PrintLineStr();
		PrintEtcStr(info->name, 10);
		PrintLineStr();
		line = info->line;

		while(1)
		{
			memset(funcStr,0,30);
			memset(data,0,100);
			memset(sel,0,4);



			//현재 라인값이 NULL 이면 루프 종료
			if((int*)(menu[i][0]) == 0)
			{

#if 0
				if(info->mode == MENU_MODE_SUB)
				{
					sprintf(funcStr, "%s['%c']", "<- Back", MENU_KEY_EXIT);
				}

				if(info->mode == MENU_MODE_MAIN)
				{
					sprintf(funcStr, "%s['%c']", "* Exit Progrem Input: ", MENU_KEY_EXIT);
				}


				MakeMenuStr(outStr, funcStr, data, sel, i, menu_num);

				printf("%s\r\n",outStr);
#endif
				PrintLineStr(); 
				break;
			}

			type = (int)menu[i][3];

			switch(type)
			{
				case 0: //none
					break;
				case 1: //str
					sprintf(data,"[%s]",((char*)menu[i][2]));
					break;
				case 2: 
					sprintf(data,"[%s]",*((unsigned char*)menu[i][2])? "ON":"OFF");
					break;
				case 3:
					sprintf(data,"[%s]",*((unsigned char*)menu[i][2])? "OFF":"ON");
					break;
				case 4:
					sprintf(data,"[%s]",*((unsigned char*)menu[i][2])? "HIGH":"LOW");
					break;
				case 5:
					sprintf(data,"[%d]", *((int*)menu[i][2]));
					break;
				case 6:
					sprintf(data,"[%.2f]", *((float*)menu[i][2]));
					break;
				case 7:
					if(*((unsigned char*)menu[i][2]) == 3)
					{
						sprintf(data,"[%s]","미감지");
					}
					else
					{
						sprintf(data,"[%s]",*((unsigned char*)menu[i][2])? "정상":"이상");
					}
					break;
				default:
					break;
			}

			sprintf(funcStr,"%s",(unsigned char *)menu[i][1]);


			//현재 선택 메뉴 라인 체크
			if(i == line)
			{
				strcat(sel, "(*)");
			}
			else
			{
				strcat(sel, "( )");
			}

			PrintMenuStr(funcStr, data, sel, i, menu_num, line);

		
			//다름 라인 수행
			i++;
		}

		PrintLineStr();
		if(info->selMode == 0)
		{
#if 0 //조작법 표출 X
			memset(temp,0, 256 );
			sprintf(temp,"* Use : Change Line Use Up Down and Enter");
			PrintEtcStr(temp, 2);

			memset(temp,0, 256 );
			sprintf(temp,"Line Up['%c']",MENU_KEY_LINE_UP);
			PrintEtcStr(temp, 2);

			memset(temp,0, 256 );
			sprintf(temp,"Line Down['%c']",MENU_KEY_LINE_DOWN);
			PrintEtcStr(temp, 2);

			memset(temp,0, 256 );
			sprintf(temp,"Exit['%c' or ESC]",MENU_KEY_EXIT);
			PrintEtcStr(temp, 2);

			memset(temp,0, 256 );
			sprintf(temp,"ModeChange['%c', TAB ]",MENU_KEY_MODE_CHANGE);
			PrintEtcStr(temp, 2);

			PrintLineStr();
#endif		
			memset(temp,0, 256 );	
			sprintf(temp,"Select Mode: LINE | * Select Line : %d",info->line);
			PrintEtcStr(temp, 2);

		}
		else
		{
#if 0
			memset(temp,0, 256 );	
			sprintf(temp,"* Use : Input Menu Num and Enter");
			PrintEtcStr(temp, 2);

			memset(temp,0, 256 );	
			sprintf(temp,"Del[BackSpace]");
			PrintEtcStr(temp, 2);

			memset(temp,0, 256 );	
			sprintf(temp,"Exit['%c'] or ESC",MENU_KEY_EXIT);
			PrintEtcStr(temp, 2);

			memset(temp,0, 256 );	
			sprintf(temp,"| ModeChange['%c' or TAB ]",MENU_KEY_MODE_CHANGE);
			PrintEtcStr(temp, 2);
			PrintLineStr();
#endif 
			memset(temp,0, 256 );	
			sprintf(temp,"Select Mode: INPUT  | * InputData : %s",key_data);
			PrintEtcStr(temp, 2);
		}
		PrintLineStr();
		k = getch();
		switch(k)
		{
			//case KEY_DOWN:
			case MENU_KEY_LINE_DOWN:
				/*라인 다운*/
				if(info->selMode != 0)
				{
					break;
				}

				if(line >= menu_num)
				{
					line = menu_num;
				}
				else
				{
					line++;
				}
				break;
				//case KEY_UP:
			case MENU_KEY_LINE_UP:
				if(info->selMode != 0)
				{
					break;
				}

				/*라인 업*/
				if(line <= 0 )
				{
					line=0;
				}
				else
				{
					line--;
				}
				break;
				//case KEY_LEFT:
			case MENU_KEY_MODE_CHANGE:
			case KEY_TAP:
				if(info->selMode == 0)
				{
					info->selMode=1;
				}
				else
				{
					info->selMode=0;
				}
				memset(key_data, 0, 10);
				stack = 0;
				break;
			case KEY_ENTER_LF:
			case KEY_ENTER_CR:
				if(info->selMode == 0)
				{
					/*현재 라인 함수 수행*/
					if(line != menu_num)
					{
						( (void (*)(void)) (menu[line][0]) )();
					}
					else
					{
					}
				}
				else
				{
					selFunc = StrToNum(key_data);


					memset(key_data,0,10); 
					stack =0; 
					if(selFunc < 0 || selFunc >= menu_num)
						break;

					if(selFunc != menu_num)
					{
						line =  selFunc;
						( (void (*)(void)) (menu[selFunc][0]) )();
					}
				}
				break;
			case KEY_BACK:
				if(stack > 0)
				{
					key_data[stack-1] = 0;
					stack--;
				}

				break;
				//case KEY_ESC:
			case MENU_KEY_EXIT:
			case KEY_ESC:
				if(info->mode == MENU_MODE_MAIN)
				{
					while(1)
					{
						printf("프로그램을 종료 하시겠습니까 ? [Yes/No] \r\n");
						memset(key_data, 0, 10);
						scanf("%s", key_data);
						scanf("%c", &dummy);
						if(strstr(key_data,"yes")!=NULL)
							return 1;
						else if(strstr(key_data,"Yes")!=NULL)
							return 1;
						else if(strstr(key_data,"Y")!=NULL)
							return 1;
						else if(strstr(key_data,"y")!=NULL)
							return 1;
						else
							// 메뉴 복귀
							break;
					}
				}
				else
				{
					return 1;
				}
				break;
			case MENU_KEY_HELP:
				system("clear");
				PrintLineStr();
				PrintHelpStr(info->help, 2);	
				PrintLineStr();
				PrintEtcStr("If you want to go back, press any key", 3);	
				PrintLineStr();
				getch();
				break;
			default :	
				if(info->selMode == 1)
				{
					if(stack <= 10)
					{
						key_data[stack++] = (char)k;
					}
				}
				break;
		}
		info->line = line;
		system("clear");

	}//while end...
	return 1;
}



