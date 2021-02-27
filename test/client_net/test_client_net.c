#include <stdio.h>
#include <string.h>
#include "client_net.h"

#define PRINT_TEST(a_testResult, a_testName) if (!a_testResult) { printf("%s\n", a_testName); }

#define PORT 9000
#define BUFFER_LENGTH 4096
#define LOCAL_HOST_ADDRESS "127.0.0.1"

int Test_ClientCreate(char *a_testName);
int Test_ClientCreate_Neg(char *a_testName);
int Test_ClientDoubleDestroy(char *a_testName);
int Test_ClientDestroy_Neg(char *a_testName);
int Test_ClientConnect(char *a_testName);
int Test_ClientConnect_Neg(char *a_testName);
int Test_ClientSend(char *a_testName);
int Test_ClientSend_Neg(char *a_testName);
int Test_ClientReceive(char *a_testName);
int Test_ClientReceive_Neg(char *a_testName);


int main() {
	char testName[100];
	PRINT_TEST(Test_ClientCreate(testName), testName);
	PRINT_TEST(Test_ClientCreate_Neg(testName), testName);
	PRINT_TEST(Test_ClientDoubleDestroy(testName), testName);
	PRINT_TEST(Test_ClientDestroy_Neg(testName), testName);
	PRINT_TEST(Test_ClientConnect(testName), testName);
	PRINT_TEST(Test_ClientConnect_Neg(testName), testName);
	PRINT_TEST(Test_ClientSend(testName), testName);
	PRINT_TEST(Test_ClientSend_Neg(testName), testName);
	PRINT_TEST(Test_ClientReceive(testName), testName);
	PRINT_TEST(Test_ClientReceive_Neg(testName), testName);
	return 0;
}


int Test_ClientCreate(char *a_testName) {
	ClientNet *client = NULL;
	int port = PORT;
	char address[] = LOCAL_HOST_ADDRESS;
	strcpy(a_testName, "Test_ClientCreate");
	client = ClientNet_Create(address, port);
	if (NULL == client) {
		return 0;
	}
	ClientNet_Destroy(&client);
	return 1;	
}

int Test_ClientCreate_Neg(char *a_testName) {
	ClientNet *client = NULL;
	int port = PORT;
	char address[] = LOCAL_HOST_ADDRESS;
	strcpy(a_testName, "Test_ClientCreate_Neg");
	client = ClientNet_Create(NULL, port);
	if (NULL != client) {
		return 0;
	}
	client = ClientNet_Create(address, -1);
	if (NULL != client) {
		return 0;
	}
	client = ClientNet_Create(address, 70000);
	if (NULL != client) {
		return 0;
	}
	ClientNet_Destroy(&client);
	return 1;	
}


int Test_ClientDoubleDestroy(char *a_testName) {
	ClientNet *client = NULL;
	int port = PORT;
	char address[] = LOCAL_HOST_ADDRESS;
	strcpy(a_testName, "Test_ClientDoubleDestroy");
	client = ClientNet_Create(address,port);
	ClientNet_Destroy(&client);
	ClientNet_Destroy(&client);
	return 1;
}


int Test_ClientDestroy_Neg(char *a_testName) {
	ClientNet *client = NULL;
	strcpy(a_testName, "Test_ClientDestroy_Neg");
	ClientNet_Destroy(&client);
	ClientNet_Destroy(NULL);
	return 1;
}


int Test_ClientConnect(char *a_testName) {
	ClientNet *client = NULL;
	int port = PORT;
	char address[] = LOCAL_HOST_ADDRESS;
	ClientNet_Result status;
	strcpy(a_testName, "Test_ClientConnect");
	client = ClientNet_Create(address,port);
	status = ClientNet_Connect(client) ? 1 : 0;
	ClientNet_Destroy(&client);
	return (ClientNet_Success == status ? 1 : 0);
}


int Test_ClientConnect_Neg(char *a_testName) {
	strcpy(a_testName, "Test_ClientConnect_Neg");
	return (ClientNet_UninitializedError == ClientNet_Connect(NULL) ? 1 : 0);
}	

	
int Test_ClientSend(char *a_testName) {
	ClientNet *client = NULL;
	int port = PORT;
	char data[] = "hello from client!!", address[] = LOCAL_HOST_ADDRESS;
	ClientNet_Result status;
	strcpy(a_testName, "Test_ClientSend");
	client = ClientNet_Create(address,port);
	ClientNet_Connect(client);
	status = ClientNet_Send(client, data, strlen(data) + 1);
	ClientNet_Destroy(&client);
	return (ClientNet_Success == status ? 1 : 0);
}


int Test_ClientSend_Neg(char *a_testName) {
	ClientNet *client = NULL;
	int port = PORT;
	char data[] = "hello from client!!", address[] = LOCAL_HOST_ADDRESS;
	strcpy(a_testName, "Test_ClientSend_Neg");
	client = ClientNet_Create(address,port);
	ClientNet_Connect(client);
	if (ClientNet_UninitializedError != ClientNet_Send(NULL, data, strlen(data) + 1)) {
		ClientNet_Destroy(&client);
		return 0;
	}
	if (ClientNet_UninitializedError != ClientNet_Send(client, NULL, strlen(data) + 1)) {
		ClientNet_Destroy(&client);
		return 0;
	}
	ClientNet_Destroy(&client);
	return 1;
}


int Test_ClientReceive(char *a_testName) {
	ClientNet *client = NULL;
	int port = PORT;
	char data[] = "send", address[] = LOCAL_HOST_ADDRESS, buffer[BUFFER_LENGTH] = { 0 };
	strcpy(a_testName, "Test_ClientReceive");
	client = ClientNet_Create(address, port);
	ClientNet_Connect(client);
	ClientNet_Send(client, data, strlen(data) + 1);
	if (ClientNet_Success != ClientNet_Receive(client, buffer, BUFFER_LENGTH)) {
		ClientNet_Destroy(&client);
		return 0;
	}
	ClientNet_Destroy(&client);
	return 1;
}		


int Test_ClientReceive_Neg(char *a_testName) {
	ClientNet *client = NULL;
	int port = PORT;
	char address[] = LOCAL_HOST_ADDRESS, buffer[BUFFER_LENGTH] = { 0 };
	strcpy(a_testName, "Test_ClientReceive_Neg");
	client = ClientNet_Create(address, port);
	ClientNet_Connect(client);
	if (ClientNet_UninitializedError != ClientNet_Receive(NULL, buffer, BUFFER_LENGTH)) {
		ClientNet_Destroy(&client);
		return 0;
	}
	if (ClientNet_UninitializedError != ClientNet_Receive(client, NULL, BUFFER_LENGTH)) {
		ClientNet_Destroy(&client);
		return 0;
	}
	ClientNet_Destroy(&client);
	return 1;
}
