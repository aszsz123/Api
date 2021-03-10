#include <stdio.h>


#include "Api_NetClient.h"
#include "Client_Menu.h"


int main(void)
{
    StartNetClientProcess();

    StartClientMenu();
	
	while(1);

    return 1;
}
