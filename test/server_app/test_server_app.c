#include <stdio.h>
#include <string.h>
#include "server_app.h"

#define PRINT_TEST(a_testResult, a_testName) if (!a_testResult) { printf("%s\n", a_testName); }

static int Test_Create(char* a_testName);
static int Test_Create_Neg(char* a_testName);
static int Test_Double_Destroy(char* a_testName);
static int Test_Double_Neg(char* a_testName);

int main() {
	char testName[100];
	PRINT_TEST(Test_Create(testName), testName);
	PRINT_TEST(Test_Create_Neg(testName), testName);
	PRINT_TEST(Test_Double_Destroy(testName), testName);
	PRINT_TEST(Test_Double_Neg(testName), testName);
	return 0;
}


static int Test_Create(char* a_testName) {
	ServerApp *server = ServerApp_Create("test_users_passwords.txt", 1000);
	strcpy(a_testName, "Test_Create");
	if (NULL == server) {
		return 0;
	}
	ServerApp_Destroy(&server);
	return 1;
}


static int Test_Create_Neg(char* a_testName) {
	ServerApp *server = ServerApp_Create(NULL, 1000);
	strcpy(a_testName, "Test_Create_Neg");
	if (NULL != server) {
		ServerApp_Destroy(&server);
		return 0;
	}
	server = ServerApp_Create("file_not_exists.txt", 1000);
	if (NULL != server) {
		ServerApp_Destroy(&server);
		return 0;
	}
	server = ServerApp_Create("test_users_passwords.txt", 0);
	if (NULL != server) {
		ServerApp_Destroy(&server);
		return 0;
	}
	return 1;
}


static int Test_Double_Destroy(char* a_testName) {
	ServerApp *server = ServerApp_Create("test_users_passwords.txt", 1000);
	strcpy(a_testName, "Test_Double_Destroy");
	if (NULL == server) {
		return 0;
	}
	ServerApp_Destroy(&server);
	ServerApp_Destroy(&server);
	return 1;
}


static int Test_Double_Neg(char* a_testName) {
	ServerApp *server = NULL;
	strcpy(a_testName, "Test_Double_Neg");
	ServerApp_Destroy(NULL);
	ServerApp_Destroy(&server);
	return 1;
}