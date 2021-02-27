#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "client_app.h"

ClientApp *g_client;

static void handler_SigInt(int a_sinNum);

int main() {
	signal(SIGINT, handler_SigInt);
	g_client = Client_Create();
	if (NULL == g_client) {
		printf("client is NULL !!!!\n");
		return 1;
	}
	Client_Run(g_client);
	Client_Destroy(&g_client);
	return 0;	
}


static void handler_SigInt(int a_sinNum) {
	printf("\n\n*****     signal SIGINT was called     *****\n");
	printf("\n\n*****              Bye Bye             *****\n");
	Client_Destroy(&g_client);
	exit(0);
}