#ifndef __API_MENU_H__
#define __API_MENU_H__

/*옵션*/
#define SIZE_FUNCNAME   40	//라인 기능 이름 최대 사이즈
#define SIZE_DATA       30	//라인 데이터 최대 사이즈 
#define POS_DATA_OFFSET 0	//데이터 출력 포지션 오프셋
#define POS_SEL_OFFSET -1	//라인 포인터 출력 포지션 오프셋

#define MENU_KEY_EXIT 			'q'
#define MENU_KEY_LINE_UP 		'w' 
#define MENU_KEY_LINE_DOWN 		's'
#define MENU_KEY_MODE_CHANGE	'c'
#define MENU_KEY_HELP			'h'


typedef struct{
	/*사용자 지정*/
    int mode;			//메뉴 종료 모드
    char *name;			//메뉴 타이틀
	char *help;			//헬프 메뉴얼 출력 스트링


	/*설정 하지 말 것*/
	int line;			//메뉴 라인 모드 시 현재 라인
    int selMode;		//메뉴 입력 모드 Line(Default), Input
}menuInfo_t;

/*메뉴 종료 모드*/
#define MENU_MODE_MAIN  1 //종료 버튼 입력 시 Yes, yes, Y ,y 입력 시 return 
#define MENU_MODE_SUB   2 //종료 버튼 입력 시 바로 return 

/*라인 데이터의 출력 타입*/
#define MENU_DATA_TYPE_NONE 		0
#define MENU_DATA_TYPE_STR 			1
#define MENU_DATA_TYPE_ONOFF		2
#define MENU_DATA_TYPE_OFFON		3
#define MENU_DATA_TYPE_HIGHLOW		4
#define MENU_DATA_TYPE_INT			5
#define MENU_DATA_TYPE_FLOAT		6


int DisplayMenu(void *(*menu)[4], menuInfo_t *info);

int getch(void);
#endif
