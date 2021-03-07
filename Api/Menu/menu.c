#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Api_Menu.h"

static int numInt;
static float numFloat;
static char string[256];
static int toggle;

void TestFunc(void)
{
	printf("Excute\r\n");
	numInt++;	
	numFloat += 0.01;
	toggle++;
	toggle = toggle%2;
	sprintf(string,"TestMsgi num[%d],float[%.02f]",numInt, numFloat);
}

static void *testMenu[][4] = {
	{(void *)TestFunc,       "Int",       	(void*)&numInt,             (int *)MENU_DATA_TYPE_INT},
	{(void *)TestFunc,       "float",       (void*)&numFloat,             (int *)MENU_DATA_TYPE_FLOAT},
	{(void *)TestFunc,       "string",      (void*)&string,             (int *)MENU_DATA_TYPE_STR},
	{(void *)TestFunc,       "On:1 Off:0",      (void*)&toggle,             (int *)MENU_DATA_TYPE_OFFON},
	{(void *)TestFunc,       "On:0 Off:1",      (void*)&toggle,             (int *)MENU_DATA_TYPE_ONOFF},
	{(void *)TestFunc,       "High:1 Low:0",      (void*)&toggle,             (int *)MENU_DATA_TYPE_HIGHLOW},
	{(void *)TestFunc,       "High:1 Low:0",      (void*)&toggle,             (int *)MENU_DATA_TYPE_HIGHLOW},
	{(void *)TestFunc,       "High:1 Low:0",      (void*)&toggle,             (int *)MENU_DATA_TYPE_HIGHLOW},
	{(void *)TestFunc,       "High:1 Low:0",      (void*)&toggle,             (int *)MENU_DATA_TYPE_HIGHLOW},
	{0,0},
};

void DisplayMainMenu(void)
{
	static menuInfo_t menuMain;
	memset(&menuMain, 0, sizeof(menuInfo_t));
	char help[]="1\n2\n\
		3\n\
		help menual";

	char title[] = "menu api example";
	menuMain.help = help;
	menuMain.name = title;
	//memcpy(menuMain.name,"Menu Api Test",sizeof("UHD TEST PROGRAM") );
	menuMain.mode = MENU_MODE_MAIN;

	DisplayMenu(testMenu, &menuMain);

}

int main(void)
{
	DisplayMainMenu();
}
