#include <stdio.h>        /*print, scan*/ 
#include <stdlib.h>       /*calloc, free*/
#include <string.h>       /*strcpy*/
#include <sys/select.h>   /*select*/
#include "protocol.h"
#include "client_net.h"
#include "multicast.h"
#include "client_app.h"
#include "list.h"
#include "list_itr.h"
#include "list_functions.h"
#include "ui_manager.h"

#define PORT 9000
#define PACK_SIZE 4096
#define BUFFER_SIZE 4096
#define GROUP_NAME_SIZE 30

struct ClientApp {
	char m_name[20];
	ClientNet *m_clientNet;
	List *m_listGroups;
	char m_login;
};


static ClientNet_Result responseHandling(ClientApp *a_client);
static void registerResponseHandling(char *a_buffer);
static void loginResponseHandling(ClientApp *a_client ,char *a_buffer);
static void openGroupResponseHandling(ClientApp *a_client ,char *a_buffer);
static void logoutResponseHandling(ClientApp *a_client ,char *a_buffer, int a_flag);
static void groupNamesResponseHandling(ClientApp *a_client ,char *a_buffer);
static void leaveGroupResponseHandling(ClientApp *a_client ,char *a_buffer);

static void requestHandling(ClientApp *a_client);
static void registerRequestHandling(ClientApp *a_client);
static void loginRequestHandling(ClientApp *a_client);
static void logoutRequestHandling(ClientApp *a_client);
static void openGroupRequestHandling(ClientApp *a_client);
static void groupsNamesRequestHandling(ClientApp *a_client);
static void leaveGroupRequestHandling(ClientApp *a_client);

static void freeGroups(void *a_groupName);
static int printAction(void *a_element, void *a_context);
static int Predicate(void *a_element, void *a_context);


ClientApp* Client_Create() {
	ClientApp *client = (ClientApp*)calloc(1, sizeof(ClientApp));
	if (NULL == client) {
		return NULL;
	}
	return client;
}


ClientApp_Result Client_Run(ClientApp *a_client) {
	int port = PORT, activity, serverSock, IO_sock = 0;
	char ipDest[] = "127.0.0.1";
	fd_set readfd;
	FD_ZERO(&readfd);

	a_client->m_clientNet = ClientNet_Create(ipDest, port);
	if (NULL == a_client->m_clientNet) {
		return ClientApp_Allocation_Error;
	}
	if (ClientNet_Success != ClientNet_Connect(a_client->m_clientNet)) {
		return ClientApp_Connect_Error;
	}
	serverSock = ClientNet_GetSock(a_client->m_clientNet);

	UI_OpenScreen();
  UI_StartMenu();
	while (1) {
		FD_SET(serverSock, &readfd);
		FD_SET(IO_sock, &readfd);
		activity = select(serverSock+1, &readfd, NULL, NULL, NULL);
		if (activity < 0) {
			return ClientApp_Connect_Error;
		}
		if (FD_ISSET(serverSock, &readfd)) {
			if (ClientNet_Success != responseHandling(a_client)) {
				break;
			}
		} 
		if (FD_ISSET(IO_sock, &readfd)) {
			requestHandling(a_client);
		}
	}	
	return ClientApp_Success;	
}


void Client_Destroy(ClientApp **a_client) {
	char buffer[BUFFER_SIZE];
	if (NULL == a_client) {
		return;
	}
	if ((*a_client)->m_login) {
		logoutRequestHandling(*a_client);
		ClientNet_Receive((*a_client)->m_clientNet, buffer, BUFFER_SIZE);
		logoutResponseHandling(*a_client, buffer, 0);
	}
	free(*a_client);
	*a_client = NULL;
}


/* static functions */

static ClientNet_Result responseHandling(ClientApp *a_client) {
	char tag, buffer[BUFFER_SIZE];

	if (ClientNet_Success != ClientNet_Receive(a_client->m_clientNet, buffer, BUFFER_SIZE)) {
		return ClientNet_MessageReceive_Error;
	}
	Protocol_CheckTag(buffer, &tag);
  	switch (tag) {
  		case Protocol_Response_Register:
			registerResponseHandling(buffer);
  			break;
  			
  		case Protocol_Response_LogIn:
  			loginResponseHandling(a_client, buffer);
  			break;
  			
  		case Protocol_Response_LogOut:
				logoutResponseHandling(a_client, buffer, 1);
				break;
  		    	
  		case Protocol_Response_GroupOpen:
  			openGroupResponseHandling(a_client, buffer);
  			break;
  		
  		case Protocol_Response_GroupJoin:
  			openGroupResponseHandling(a_client, buffer);
  			break;

  		case Protocol_Response_GroupsNames:
  			groupNamesResponseHandling(a_client, buffer);
  			break;

  		case Protocol_Response_LeaveGroup:
  			leaveGroupResponseHandling(a_client, buffer);
  			break;
  	}
	return ClientNet_Success;
}


static void requestHandling(ClientApp *a_client) {
	int userChoice;
  /* char buffer[BUFFER_SIZE]; */

	scanf("%d",&userChoice);
	if (!a_client->m_login) {
		switch (userChoice) {
			case 0:
				registerRequestHandling(a_client);
				break;

			case 1:
				loginRequestHandling(a_client);
				break;

			case 9:
				exit(0);
				break;

			default:
				printf("The number %d is not one of the options !\nPlease enter again.\n", userChoice);
				break;
		}
	}
	else {
		switch (userChoice) {
			case 2:
				logoutRequestHandling(a_client);
				break;

			case 3:
				openGroupRequestHandling(a_client);
				break;

			case 4:
				groupsNamesRequestHandling(a_client);
				break;

			case 5:
				leaveGroupRequestHandling(a_client);
				break;

			case 9:
				Client_Destroy(&a_client);
				exit(0);
				break;

			default:
				printf("The number %d is not one of the options !\nPlease enter again.\n", userChoice);
				break;
    }
	}
}


static void registerResponseHandling(char *a_buffer) {
  Response_LogInOut registerResponse;
	Protocol_Unpack(a_buffer , (void*)&registerResponse);		
	if(1 == registerResponse.m_status) {
		UI_OK();
		UI_StartMenu();
	}
	else {
		UI_Err();
		UI_StartMenu();
  }
}


static void loginResponseHandling(ClientApp *a_client ,char *a_buffer) {
	Response_LogInOut loginResponse;
	Protocol_Unpack(a_buffer, (void*)&loginResponse);
	if (loginResponse.m_status == 1) {
		UI_OK();
		a_client->m_listGroups = List_Create();
		if (NULL == a_client->m_listGroups) {
			ClientNet_Destroy(&a_client->m_clientNet);
			return;
		}
		a_client->m_login = 1;
		UI_LoginMenu();
	}
	else {
	  UI_Err();
    UI_StartMenu();
	}
}


static void logoutResponseHandling(ClientApp *a_client ,char *a_buffer, int a_flag) {
	Response_LogInOut logOutResponse;
	ListItr begin, end;
	Protocol_Unpack(a_buffer, (void*)&logOutResponse);
	if (logOutResponse.m_status) {
		UI_OK();
		begin = ListItr_Begin(a_client->m_listGroups);
		end = ListItr_End(a_client->m_listGroups);
		while (begin != end) {
			Multicast_Destroy((char*)ListItr_Get(begin), a_client->m_name);
			begin = ListItr_Next(begin);
		}
		List_Destroy(&a_client->m_listGroups, freeGroups);
	}
	else {
		UI_Err();
	}
	a_client->m_login = 0;
	if (a_flag) {
		UI_StartMenu();
	}
	return;
}


static void openGroupResponseHandling(ClientApp *a_client ,char *a_buffer) {
	Response_Group openGroupResponse;
	int port;
	char *groupName = NULL;
	Multicast_Result status = Multicast_Unknown_Error;
	 
	Protocol_Unpack(a_buffer, (void*)&openGroupResponse);
	port = atoi(openGroupResponse.m_port);
	if (openGroupResponse.m_status) {
		groupName = (char*)malloc(strlen(openGroupResponse.m_groupName) + 1);
		if (NULL == groupName) {
			printf("There is an allocation error\n");
			return;
		}
		strcpy(groupName, openGroupResponse.m_groupName);
		List_PushHead(a_client->m_listGroups, groupName);
		status = Multicast_Create(openGroupResponse.m_ipAddress, port, groupName, a_client->m_name);
	}
	if (status == Multicast_Success) {
		UI_OK();
		UI_GroupMenu();
	}
	else {
		UI_Err();
		if (List_IsEmpty(a_client->m_listGroups)) {
			UI_LoginMenu();
		}
		else {
			UI_GroupMenu();
		}
	}	
}


static void groupNamesResponseHandling(ClientApp *a_client ,char *a_buffer) {
  Request_Group groupRequest;
	Response_GroupNames groupNamesResponse;
	size_t i, j = 2, size;
	char *groups = NULL, pack[PACK_SIZE];
	ListItr begin, end;

	Protocol_Unpack(a_buffer, (void*)&groupNamesResponse);
	groups = groupNamesResponse.m_allGroups;
	if (0 != groups[0]) {
		printf("Please choose a group name from the list below:\n");
		for (i = groups[0] ; i > 0; --i) { 
			j += printf("%s\n", &groups[j]);
		}
		scanf("%s", groupRequest.m_groupName);
		begin = ListItr_Begin(a_client->m_listGroups);
		end = ListItr_End(a_client->m_listGroups);
		if (end == ListItr_FindFirst(begin, end, Predicate, groupRequest.m_groupName)) {
			groupRequest.m_tag = Protocol_Request_GroupJoin;
			Protocol_Pack((void*)&groupRequest, pack, &size);
			ClientNet_Send(a_client->m_clientNet, pack, size); 
		}
		else {
			UI_Err();
		}
	}
	else {
		printf("There is no exist groups to join !\n");
	}
	UI_LoginMenu();
}


static void leaveGroupResponseHandling(ClientApp *a_client ,char *a_buffer) {
	Response_Group leaveGroupResponse;
	Protocol_Unpack(a_buffer, (void*)&leaveGroupResponse);
	if (leaveGroupResponse.m_status) {
		UI_OK();
	}
	else {
		UI_Err();
	}
	if (List_IsEmpty(a_client->m_listGroups)) {
		UI_LoginMenu();
	}
	else {
		UI_GroupMenu();
	}
}


static void registerRequestHandling(ClientApp *a_client) {
	Request_LogIn registerRequest;
	size_t size;
	char pack[PACK_SIZE];    
	UI_Register(registerRequest.m_name,registerRequest.m_password);
	registerRequest.m_tag = Protocol_Request_Register;
 	Protocol_Pack((void*)&registerRequest, pack, &size);
 	ClientNet_Send(a_client->m_clientNet, pack, size);
}


static void loginRequestHandling(ClientApp *a_client) {
	Request_LogIn logInRequest;
	size_t size;
	char pack[PACK_SIZE];
	UI_Login(logInRequest.m_name,logInRequest.m_password);
	strcpy(a_client->m_name , logInRequest.m_name);
  logInRequest.m_tag = Protocol_Request_LogIn;
 	Protocol_Pack((void*)&logInRequest, pack, &size);
 	ClientNet_Send(a_client->m_clientNet, pack, size);
}


static void logoutRequestHandling(ClientApp *a_client) {
	Request_Logout logOutRequest;
	char pack[PACK_SIZE], *name = NULL;
	size_t size, index = 2;
	ListItr begin, end;
	logOutRequest.m_tag = Protocol_Request_LogOut;
  strcpy(logOutRequest.m_name, a_client->m_name);
	begin = ListItr_Begin(a_client->m_listGroups);
	end = ListItr_End(a_client->m_listGroups);
	logOutRequest.m_groupsNames[0] = List_Size(a_client->m_listGroups);;
	logOutRequest.m_groupsNames[1] = '\0';
	while (begin != end) {
		name = (char*)ListItr_Get(begin);
		strcpy(logOutRequest.m_groupsNames + index , name);
		index += strlen(name) + 1;
		begin = ListItr_Next(begin);
	}
	Protocol_Pack((void*)&logOutRequest, pack, &size);
	ClientNet_Send(a_client->m_clientNet, pack, size);
}


static void openGroupRequestHandling(ClientApp *a_client) {
	Request_Group openGroupRequest;
	size_t size;
	char pack[PACK_SIZE];
	openGroupRequest.m_tag = Protocol_Request_GroupOpen;
	printf("\nPlease insert group name\n");
	scanf("%s",openGroupRequest.m_groupName);
	Protocol_Pack((void*)&openGroupRequest, pack, &size);
 	ClientNet_Send(a_client->m_clientNet, pack, size);
}


static void groupsNamesRequestHandling(ClientApp *a_client) {
	Request_GroupsNames groupNameRequest;
	size_t size;
	char pack[PACK_SIZE];
	groupNameRequest.m_tag = Protocol_Request_GroupsNames;
	Protocol_Pack((void*)&groupNameRequest, pack, &size);
	ClientNet_Send(a_client->m_clientNet, pack, size);
}


static void leaveGroupRequestHandling(ClientApp *a_client) {
	Request_Group leaveGroupRequest;
	char groupName[GROUP_NAME_SIZE], pack[PACK_SIZE];
	size_t size;
	ListItr begin, end, returnVal;

	begin = ListItr_Begin(a_client->m_listGroups);
	end = ListItr_End(a_client->m_listGroups);
	if (begin == end) {
		printf("You have not joined to any group!\n");
		return;
	}
	ListItr_ForEach(begin, end, printAction, NULL);
	printf("Please enter the name of the group that you want to leave\n");
	scanf("%s", groupName);
	begin = ListItr_Begin(a_client->m_listGroups);
	end = ListItr_End(a_client->m_listGroups);
	returnVal = ListItr_FindFirst(begin, end, Predicate, groupName);
	if (returnVal == end) {
		printf("You entered incorrect name\n");
		UI_Err();
		UI_GroupMenu();
		return;
	}
	ListItr_Remove(returnVal);
	if (Multicast_Success != Multicast_Destroy(groupName, a_client-> m_name)) {
		printf("There as been a problem in closing the multicast window\n");
		return;
	}
	leaveGroupRequest.m_tag = Protocol_Request_GroupLeave;
	strcpy(leaveGroupRequest.m_groupName, groupName);
	Protocol_Pack((void*)&leaveGroupRequest, pack, &size);
	ClientNet_Send(a_client->m_clientNet, pack, size);
}


static void freeGroups(void *a_groupName) {
	free(a_groupName);
}


static int printAction(void *a_element, void *a_context) {
	printf("%s\n",(char*)a_element);
	return 1;
}


static int Predicate(void *a_element, void *a_context)  {
	return !strcmp((char*)a_element, (char*)a_context);
}
