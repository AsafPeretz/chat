#include <stdio.h> 				/* printf */
#include <stdlib.h>				/* malloc,free */
#include <string.h>				/* strlen */
#include "server_app.h"
#include <assert.h>

struct ServerApp {
	UserManager *m_userManager;
	ServerNet *m_serverNet;
	GroupManager *m_groupManager;
};

#define MAX_STRING_SIZE 50
#define BUFFER_SIZE 4096
#define NUM_OF_GROUPS 100

void NewClientFunc(int a_clientSock, void *a_context);
void CloseClientFunc(int a_clientSock, void *a_context);
void FailFunc(int a_clientSock, void *a_context);
void GotMessageFunc(int a_clientSock, void *a_inMsg, void *a_context);
static void handleRegisterRequest(ServerApp *a_serverApp, void *a_inMsg, int a_clientSock);
static void handleLogInRequest(ServerApp *a_serverApp, void *_inMsg, int a_clientSock);
static void handleOpenGroupRequest(ServerApp *a_serverApp, void *a_inMsg, int a_clientSock);
static void handleLogOutRequest(ServerApp *a_serverApp, void *a_inMsg, int a_clientSock);
static void handleGroupNameRequest(ServerApp *a_serverApp, void *a_inMsg, int a_clientSock);
static void handleGroupAddRequest(ServerApp *a_serverApp, void *a_inMsg, int a_clientSock);
static void handleLeaveGroupRequest(ServerApp *a_serverApp, void *a_inMsg, int a_clientSock);
static void destroyAllGroups(GroupManager *a_groupManager, char *a_groupString);


ServerApp* ServerApp_Create(char *a_fileUsers, size_t a_capacity) {
	ServerApp *server = (ServerApp*)malloc(sizeof(ServerApp));
	if (NULL == server) {
		return NULL;
	}
	server->m_serverNet = ServerNet_Create(NewClientFunc, GotMessageFunc, CloseClientFunc, FailFunc);
	if (NULL == server->m_serverNet) {
		free(server);
		return NULL;
	}
	server->m_userManager = UserManager_Create(a_fileUsers, a_capacity);
	if (NULL == server->m_userManager) {
		ServerNet_Destroy(&(server->m_serverNet));
		free(server);
		return NULL;
	}
	server->m_groupManager = GroupManager_Create(NUM_OF_GROUPS);
	if (NULL == server->m_groupManager) {
		UserManager_Destroy(&(server->m_userManager));
		ServerNet_Destroy(&(server->m_serverNet));
		free(server);
		return NULL;
	}
	return server;
}


ServerApp_Result ServerApp_Run(ServerApp *a_server) {
	ServerNet_Result status;
	status = ServerNet_Run(a_server->m_serverNet, a_server);
	switch (status) {
		case ServerNet_Success:
			return ServerApp_Success;
		case ServerNet_Allocation_Error:
			return ServerApp_Allocation_Error;
		case ServerNet_Select_Error:
			return ServerApp_Network_Error;
		default:
			return ServerApp_Unknown_Error;

	}
}


void ServerApp_Destroy(ServerApp **a_server) {
	if (NULL == a_server || NULL == *a_server) {
		return;
	}
	GroupManager_Destroy(&((*a_server)->m_groupManager));
	UserManager_Destroy(&((*a_server)->m_userManager));
	ServerNet_Destroy (&((*a_server)->m_serverNet));
	free(*a_server);
	*a_server = NULL;
}


void NewClientFunc(int a_clientSock, void *a_context) {
	char message[] = "welcome to the Chat!\n";
	int msgSize = strlen(message);
	printf("one of the client has been connected\n");
	ServerNet_Send(a_clientSock, message, msgSize + 1);
}


void CloseClientFunc(int a_clientSock, void *a_context) {
	printf("one of the client has been disconnected\n");
}


void FailFunc(int a_clientSock, void *a_context) {
	char message[] = "There was a problem with the server\n";
	int msgSize = strlen(message);
	ServerNet_Send (a_clientSock, message, msgSize);
}


void GotMessageFunc(int a_clientSock, void *a_inMsg, void *a_context) {
	ServerApp* server = (ServerApp*)a_context;
	char tag;	
	Protocol_CheckTag((char*)a_inMsg, &tag);
	switch (tag) {
		case Protocol_Request_Register:
			handleRegisterRequest(server, a_inMsg, a_clientSock);
			break;

		case Protocol_Request_LogIn:
			handleLogInRequest(server, a_inMsg, a_clientSock);
			break;
	
		case Protocol_Request_LogOut:
			handleLogOutRequest(server, a_inMsg, a_clientSock);
			break;

		case Protocol_Request_GroupOpen:
			handleOpenGroupRequest(server, a_inMsg, a_clientSock);
			break;

		case Protocol_Request_GroupJoin:
			handleGroupAddRequest(server, a_inMsg, a_clientSock);
			break;

		case Protocol_Request_GroupLeave:
			handleLeaveGroupRequest(server, a_inMsg, a_clientSock);
			break;

		case Protocol_Request_GroupsNames:
			handleGroupNameRequest(server, a_inMsg, a_clientSock);
			break;
	}
}


static void handleLogOutRequest(ServerApp *a_server, void *a_inMsg, int a_clientSock) {
	Request_Logout packRecived;
	Response_LogInOut packToSend;
	char buffer[BUFFER_SIZE];
	size_t msgSize;
	Protocol_Unpack((char*)a_inMsg, &packRecived);
	if (UserManager_Success == UserManager_Logout(a_server->m_userManager, packRecived.m_name)) {
		packToSend.m_status = 1;
		destroyAllGroups(a_server->m_groupManager, packRecived.m_groupsNames);
	}
	else {
		packToSend.m_status = 0;
	}
	packToSend.m_tag = Protocol_Response_LogOut;
	Protocol_Pack(&packToSend, buffer, &msgSize);
	ServerNet_Send(a_clientSock, buffer, msgSize);
}


static void destroyAllGroups(GroupManager *a_groupManager, char *a_groupString) {
	int i = 0, numOfStrings = a_groupString[0];
	int startIndex = 2;
	char str[MAX_STRING_SIZE];
	GroupManager_Result status;
	while (i < numOfStrings) {
		strcpy(str, &a_groupString[startIndex]);
		status = GroupManager_RemoveUser(a_groupManager, str);
		assert(status == GroupManager_Success);
		startIndex += strlen(str) + 1;
		++i;
	}
}


static void handleLeaveGroupRequest(ServerApp *a_server,void *a_inMsg, int a_clientSock) {
	Request_Group packRecived;
	Response_LeaveGroup packToSend;
	char buffer[BUFFER_SIZE];
	size_t msgSize;

	Protocol_Unpack((char*)a_inMsg, &packRecived);

	if(GroupManager_Success == GroupManager_RemoveUser(a_server->m_groupManager, packRecived.m_groupName)) {
		packToSend.m_status = 1;	
	}
	else {
		packToSend.m_status = 0;
	}
	packToSend.m_tag = Protocol_Response_LeaveGroup;
	strcpy(packToSend.m_groupName, packRecived.m_groupName);
	Protocol_Pack(&packToSend, buffer, &msgSize);
	ServerNet_Send(a_clientSock, buffer, msgSize);
}


static void handleGroupAddRequest(ServerApp *a_server, void *a_inMsg, int a_clientSock) {
	Request_Group packRecived;
	Response_Group packToSend;
	char buffer[BUFFER_SIZE];
	size_t msgSize;
	Protocol_Unpack((char*)a_inMsg, &packRecived);
	if(GroupManager_Success == GroupManager_AddUser(a_server->m_groupManager, packRecived.m_groupName, packToSend.m_ipAddress, packToSend.m_port)) {
		packToSend.m_status = 1;	
	}
	else {
		packToSend.m_status = 0;
	}
	strcpy(packToSend.m_groupName, packRecived.m_groupName);
	packToSend.m_tag = Protocol_Response_GroupJoin;
	Protocol_Pack(&packToSend, buffer, &msgSize);
	ServerNet_Send(a_clientSock, buffer, msgSize);
}


static void handleGroupNameRequest(ServerApp *a_server, void *a_inMsg, int a_clientSock) {
	Request_GroupsNames packRecived;
	Response_GroupNames packToSend;
	char buffer[BUFFER_SIZE];
	size_t msgSize;
	Protocol_Unpack((char*)a_inMsg, &packRecived);
	GroupManager_GetAllGroups(a_server->m_groupManager,packToSend.m_allGroups);
	packToSend.m_tag = Protocol_Response_GroupsNames;
	Protocol_Pack(&packToSend, buffer, &msgSize);
	ServerNet_Send(a_clientSock, buffer, msgSize);
}


static void handleOpenGroupRequest(ServerApp *a_server, void *a_inMsg, int a_clientSock) {
	Request_Group packRecived;
	Response_Group packToSend;
	char buffer[BUFFER_SIZE];
	size_t msgSize;
	GroupManager_Result status;
	Protocol_Unpack((char*)a_inMsg, &packRecived);
	status = GroupManager_OpenGroup(a_server->m_groupManager, packRecived.m_groupName, packToSend.m_ipAddress, packToSend.m_port);
	packToSend.m_status = (status == GroupManager_Success ? 1 : 0);
	packToSend.m_tag = Protocol_Response_GroupOpen;
	strcpy(packToSend.m_groupName, packRecived.m_groupName);
	Protocol_Pack(&packToSend, buffer, &msgSize);
	ServerNet_Send(a_clientSock, buffer, msgSize);
}


static void handleRegisterRequest(ServerApp *a_server, void *a_inMsg, int a_clientSock) {
	Request_LogIn packRecived;
	Response_LogInOut packToSend;
	char buffer[BUFFER_SIZE];
	size_t msgSize;
	UserManager_Result status;
	Protocol_Unpack((char*)a_inMsg, &packRecived);
	status = UserManager_Register(a_server->m_userManager, packRecived.m_name, packRecived.m_password);
	packToSend.m_status = (status == UserManager_Success ? 1 : 0);
	packToSend.m_tag = Protocol_Response_Register;
	Protocol_Pack(&packToSend, buffer, &msgSize);
	ServerNet_Send(a_clientSock, buffer, msgSize);
}


static void handleLogInRequest(ServerApp *a_server, void *a_inMsg, int a_clientSock) {
	Request_LogIn packRecived;
	Response_LogInOut packToSend;
	char buffer[BUFFER_SIZE];
	size_t msgSize;
	UserManager_Result status;
	Protocol_Unpack((char*)a_inMsg, &packRecived);
	status = UserManager_Login(a_server->m_userManager, packRecived.m_name, packRecived.m_password);
	packToSend.m_status = (status == UserManager_Success ? 1 : 0);
	packToSend.m_tag = Protocol_Response_LogIn;
	Protocol_Pack(&packToSend, buffer, &msgSize);
	ServerNet_Send(a_clientSock, buffer, msgSize);	
}
