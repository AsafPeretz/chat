#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "server_app.h"

ServerApp *g_server;

static void handler_SigInt(int a_sinNum);

int main() {
	signal(SIGINT, handler_SigInt);
	g_server = ServerApp_Create("../../users_and_passwords.txt", 1000);
	if (NULL == g_server) {
		printf("server is NULL !!!!\n");
		return 1;
	}
	ServerApp_Run(g_server);
	ServerApp_Destroy(&g_server);
	return 0;
}


static void handler_SigInt(int a_sinNum) {
	printf("\n\n*****     signal SIGINT was called     *****\n");
	printf("\n\n*****              Bye Bye             *****\n");
	ServerApp_Destroy(&g_server);
	exit(0);
}
