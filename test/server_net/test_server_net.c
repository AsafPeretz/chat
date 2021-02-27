#include <stdio.h>
#include <string.h>
#include "server_net.h"

#define PRINT_TEST(a_testResult, a_testName) if (!a_testResult) { printf("%s\n", a_testName); }

typedef struct Package {
	size_t m_signal;
	char m_name[30];
	char m_password[30];
	size_t m_clientNumber;
}Package;

typedef struct Context {
	size_t m_numOfClient;
	char m_string[50];
	int m_testFail;
	ServerNet *m_server;
}Context;

static int Test_Create(char *a_testName);
static int Test_Create_NULL(char *a_testName);
static int Test_Destroy(char *a_testName);
static int Test_Run(char *a_testName);

static void Server_NewClient(int a_sock, void *a_context);
static void Server_CloseClient(int a_sock, void *a_context);
static void Server_GotMessage(int a_client_sock, void *a_inMsg, void *a_context);
static void Server_Fail(int a_sock, void *a_context);

static int stringToSizeT(const char* a_str, size_t *a_retval);

int main() {
	char testName[100];
	PRINT_TEST(Test_Create(testName), testName);
	PRINT_TEST(Test_Create_NULL(testName), testName);
	PRINT_TEST(Test_Destroy(testName), testName);
	PRINT_TEST(Test_Run(testName), testName);
	return 0;
}


static int Test_Create(char *a_testName) {
	ServerNet *server = ServerNet_Create(Server_NewClient, Server_GotMessage, Server_CloseClient, Server_Fail);
	strcpy(a_testName, "Test_Create");
	if (NULL == server) {
		return 0;
	}
	ServerNet_Destroy(&server);
	return 1;
}


static int Test_Create_NULL(char *a_testName) {
	ServerNet *server = ServerNet_Create(NULL, NULL, NULL, NULL);
	strcpy(a_testName, "Test_Create_NULL");
	if (NULL == server) {
		return 0;
	}
	ServerNet_Destroy(&server);
	return 1;
}


static int Test_Destroy(char *a_testName) {
	ServerNet *server = ServerNet_Create(NULL, NULL, NULL, NULL);
	strcpy(a_testName, "Test_Destroy");
	ServerNet_Destroy(&server);
	ServerNet_Destroy(&server);
	if (NULL != server) {
		return 0;
	}
	return 1;
}

static int Test_Run(char *a_testName) {
	Context con;
	ServerNet *server = ServerNet_Create(Server_NewClient, Server_GotMessage, Server_CloseClient, Server_Fail);
	strcpy(a_testName, "Test_Run");
	con.m_numOfClient = 0;
	strcpy(con.m_string , "i am a context string");
	con.m_testFail = 0;
	con.m_server = server;
	ServerNet_Run(server, (void*)&con);
	ServerNet_Destroy(&server);
	return con.m_testFail ? 0 : 1;
}


/* ---------- Local Server Functions ---------- */

static void Server_NewClient(int a_sock, void *a_context) {
	Context *context = (Context*)a_context;
	context->m_testFail = 0;
	++(context->m_numOfClient);
	if (context->m_numOfClient != 1) {
		context->m_testFail = 1;
	}
}


static void Server_CloseClient(int a_sock, void *a_context) {
	Context *context = (Context*)a_context;
	context->m_testFail = 0;
	--(context->m_numOfClient);
	if (context->m_numOfClient != 0) {
		context->m_testFail = 1;
	}
}


static void Server_GotMessage (int a_client_sock, void *a_inMsg, void *a_context) {
	char send[] = "Hi Client !!\nTest Passed!!\n", clientNumber[10] = { '\0' }, clientSignal[10] = { '\0' };
	unsigned char currentIndex = 0;
	Package pack;
	Context *context = (Context*)a_context;

	strcpy(clientSignal, (char*)a_inMsg);
	currentIndex += stringToSizeT(clientSignal, &pack.m_signal) + 1;

	strcpy(pack.m_name, (char*)a_inMsg + currentIndex);
	currentIndex += strlen(pack.m_name) + 1;

	strcpy(pack.m_password, (char*)a_inMsg + currentIndex);
	currentIndex += strlen(pack.m_password) + 1;

	strcpy(clientNumber, (char*)a_inMsg + currentIndex);
	currentIndex += stringToSizeT(clientNumber, &pack.m_clientNumber) + 1;

	if (context->m_numOfClient != 1 || strcmp(context->m_string, "i am a context string") != 0) {
		context->m_testFail = 1;
		return;
	}
	if (pack.m_signal != 12 || strcmp(pack.m_name,"ziv") != 0 || strcmp(pack.m_password,"123") != 0 || pack.m_clientNumber != 51) {
		context->m_testFail = 1;
		return;
	}

	if (ServerNet_Success != ServerNet_Send(a_client_sock, send, strlen(send) + 1)) {
		context->m_testFail = 1;
	}
}


static void Server_Fail (int a_sock, void *a_context) {
	((Context*)a_context)->m_testFail = 1;
}


static int stringToSizeT(const char* a_str, size_t *a_retval) {
	size_t i = 0;
	if (NULL == a_retval) {
		return -1;
	}
	*a_retval = 0;
	do {
		*a_retval *= 10;
		*a_retval += a_str[i] - '0';
		++i;
	} while (a_str[i] >= '1' && a_str[i] <= '9');
	return i;
}







