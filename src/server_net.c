#include <stdio.h> 				/* perror */
#include <stdlib.h> 			/* malloc, free */
#include <unistd.h> 			/* close */
#include <string.h> 			/* strcpy, memset */
#include <sys/types.h> 		/* ssize_t, select */
#include <sys/select.h> 	/* fd_set */
#include <netinet/in.h> 	/* struct sockadder_in */
#include <arpa/inet.h> 		/* inet_addr */
#include <errno.h> 				/* errno */
#include <fcntl.h> 				/* fcntl */
#include <sys/select.h> 	/* select */
#include "list.h" 			/* List_Create, List_Destroy, List_PushHead */
#include "list_itr.h" 		/* List, List_Itr */
#include "server_net.h"


#define LOCAL_HOST "127.0.0.1"
#define PORT 9000
#define BUFFER_SIZE 4096
#define PAUSE_BUFFER_SIZE 256
#define SERVER_CAPACITY 1000
#define FAIL -1
#define SUCCESS 1
#define MAX_SD 1024
#define ACCEPT_IN_ROW 4
#define STDIN_SOCKET 0

#define PAUSE_MESSAGE(key) (printf("\nTo pause the server - write \"%s\"\n", key))

struct ServerNet {
	List *m_list;
	int m_optval;
	int m_sock;
	size_t m_counter;
	struct sockaddr_in m_sin;
	fd_set m_readfds;
	char m_running;
	newClientFunc n_newClient;
	newMessageFunc m_newMessage;
	closeClientFunc m_closeClient;
	messageFailedFunc m_messageFailed;
};

static int server_Initialize(ServerNet *a_server, const char *a_address, uint16_t a_port);
static int* new_Client_Handle(ServerNet *a_server, void *a_context);
static void single_Client_Message_Handle(ServerNet *a_server, ListItr **a_element, int *a_sockClient, void *a_context);
static void remove_Socket (int *a_sockList, ListItr **a_element);
static void close_Socket (void *a_item);
static void pause_Server_Check(char *a_runningFlag, const char *a_pauseKey);
static ServerNet_Result add_Clients_To_Server_List(ServerNet *a_server, size_t a_numOfClientsToAccept, int *a_activity, void *a_context);
static void all_Client_Message_Handle(ServerNet *a_server, int *a_activity, void *a_context, fd_set *readCurrent);


ServerNet* ServerNet_Create(newClientFunc a_newClient, newMessageFunc a_newMessage, closeClientFunc a_closeClient, messageFailedFunc a_messageFailed) {
	ServerNet *server = (ServerNet*)calloc(1 , sizeof(ServerNet));
	if (NULL == server) {
		return NULL;
	}
	server->n_newClient = a_newClient;
	server->m_newMessage = a_newMessage;
	server->m_closeClient = a_closeClient;
	server->m_messageFailed = a_messageFailed;
	server->m_optval = 1;
	if (FAIL == server_Initialize(server, LOCAL_HOST, PORT)) {
		free(server);
		return NULL;
	}
	if (NULL == (server->m_list = List_Create())) {
		free(server);
		return NULL;
	}
	return server;	
}


ServerNet_Result ServerNet_Run(ServerNet *a_server, void *a_context) {
	fd_set readCurrent;
	int activity;
	const char pauseKey[] = "pause";

	a_server->m_running = 1;

	PAUSE_MESSAGE(pauseKey);

	while (a_server->m_running) {
		readCurrent = a_server->m_readfds;
		activity = select(MAX_SD, &readCurrent, NULL, NULL, NULL);
		if (activity < 0 && errno != EINTR) {
			return ServerNet_Select_Error;
		}

		if (FD_ISSET(STDIN_SOCKET, &readCurrent)) {
			pause_Server_Check(&a_server->m_running, pauseKey);
		}

		if (FD_ISSET(a_server->m_sock, &readCurrent)) {
			add_Clients_To_Server_List(a_server, ACCEPT_IN_ROW, &activity, a_context);
		}

		all_Client_Message_Handle(a_server, &activity, a_context, &readCurrent);
	}
	return ServerNet_Success;
}


ServerNet_Result ServerNet_Send(int a_sockClient, void *a_message, size_t a_messageSize) {
	ssize_t sendBytes;
	while (0 < a_messageSize) {
		sendBytes = send(a_sockClient, a_message, a_messageSize, 0);
		if (sendBytes < 0) {
			if (errno != EAGAIN && errno != EWOULDBLOCK) {
				return ServerNet_SendMessage_Error;
			}
		}
		a_messageSize -= sendBytes;
	}
	return ServerNet_Success;
}


size_t ServerNet_Pause(ServerNet *a_server) {
	if (NULL == a_server) {
		return 0;
	}
	a_server->m_running = 0;
	return List_Size(a_server->m_list);
}


void ServerNet_Destroy(ServerNet **a_server) {
	if (NULL == a_server || NULL == *a_server) {
		return;
	}
	List_Destroy(&(*a_server)->m_list, close_Socket);
	close((*a_server)->m_sock);
	free(*a_server);
	*a_server = NULL;
	return;
}


/* ---------- Local Functions ---------- */

static ServerNet_Result add_Clients_To_Server_List(ServerNet *a_server, size_t a_numOfClientsToAccept, int *a_activity, void *a_context) {
	int *sockList = NULL;
	size_t i = 0;
	for ( ; i < a_numOfClientsToAccept ; ++i) {
		sockList = new_Client_Handle(a_server, a_context);
		if (NULL != sockList) {
			if (List_Success != List_PushHead(a_server->m_list, sockList)) {
				return ServerNet_Allocation_Error;
			}
		}
		else {
			break;
		}
	}
	--(*a_activity);
	return ServerNet_Success;
}


static int* new_Client_Handle(ServerNet *server, void *context) {
	socklen_t sin_len = sizeof(server->m_sin);
	int sock_client, *sock_list = NULL;

	sock_client = accept(server->m_sock, (struct sockaddr*)&server->m_sin, &sin_len);

	if (sock_client < 0) {
		if (errno != EAGAIN && errno != EWOULDBLOCK) {
			server->m_running = 0;
		}
	}
	else {
		sock_list = (int*)malloc(sizeof(int));
		*sock_list = sock_client;
		if (SERVER_CAPACITY == server->m_counter) {
			close(*sock_list);
			free(sock_list);
			sock_list = NULL;
		}
		else {
			++server->m_counter;
			FD_SET(*sock_list, &server->m_readfds);
			if (NULL != server->n_newClient) {
				server->n_newClient(sock_client, context);
			}
		}
	}
	return sock_list;
}


static void all_Client_Message_Handle(ServerNet *a_server, int *a_activity, void *a_context, fd_set *a_readCurrent) {
	ListItr *element = NULL, *lastElement = NULL;
	int *sockList = NULL;

	element = ListItr_Begin(a_server->m_list);
	lastElement = ListItr_End(a_server->m_list);

	while (0 != *a_activity && element != lastElement) {
		sockList = ListItr_Get(element);

		if (FD_ISSET(*sockList, a_readCurrent)) {
			single_Client_Message_Handle(a_server, &element, sockList, a_context);
			--(*a_activity);
		}
		element = ListItr_Next(element);
	}
}


static void single_Client_Message_Handle(ServerNet *a_server, ListItr **a_element, int *a_sockClient, void *a_context) {
	char messageBuffer[BUFFER_SIZE];
	ssize_t receive = recv(*a_sockClient, messageBuffer, BUFFER_SIZE, 0);

	if (0 >= receive) {
		FD_CLR(*a_sockClient, &a_server->m_readfds);
		remove_Socket(a_sockClient, a_element);
		--(a_server->m_counter);

		/* socket has been closed */
		if (0 == receive) {
			if (NULL != a_server->m_closeClient) {
				a_server->m_closeClient(*a_sockClient, a_context);
			}
		}
		/* an error */
		else {
			if (NULL != a_server->m_messageFailed) {
				a_server->m_messageFailed(*a_sockClient, a_context);
			}			
		}
	}
	/* new message received */
	else {
		if (NULL != a_server->m_newMessage) {
			a_server->m_newMessage(*a_sockClient, messageBuffer, a_context);
		}
	}
	return;
}


static int server_Initialize(ServerNet *a_server, const char *a_address, uint16_t a_port) {
	int flags;
	const int queue_size = 100;

	a_server->m_sin.sin_family = AF_INET;
	a_server->m_sin.sin_addr.s_addr = inet_addr(a_address);
	a_server->m_sin.sin_port = htons(a_port);

	a_server->m_sock = socket(AF_INET , SOCK_STREAM, 0);
	if (a_server->m_sock < 0) {
		return FAIL;
	}

	FD_SET(a_server->m_sock, &a_server->m_readfds);
	FD_SET(STDIN_SOCKET, &a_server->m_readfds);

	if (setsockopt(a_server->m_sock, SOL_SOCKET, SO_REUSEADDR, &a_server->m_optval, sizeof(a_server->m_optval)) < 0) {
		return FAIL;
	}

	flags = fcntl(a_server->m_sock , F_GETFL);
	if (FAIL == flags) {
		return FAIL;
	}
	if (FAIL == fcntl(a_server->m_sock , F_SETFL, flags | O_NONBLOCK)) {
		return FAIL;
	}
	if (bind(a_server->m_sock, (struct sockaddr*)&a_server->m_sin, sizeof(a_server->m_sin)) < 0) {
		return FAIL;
	}
	if (listen(a_server->m_sock, queue_size) < 0) {
		return FAIL;
	}
	return SUCCESS;	
}


static void remove_Socket(int *a_sockList, ListItr **a_element) {
	close(*a_sockList);
	*a_element = ListItr_Next(*a_element);
	ListItr_Remove(ListItr_Prev(*a_element));
	*a_element = ListItr_Prev(*a_element);
	free(a_sockList);
	return;
}


static void close_Socket(void* a_socket) {
	close(*((int*)a_socket));
	free(a_socket);
	return;
}

static void pause_Server_Check(char *a_runningFlag, const char *a_pauseKey) {
	char pause[PAUSE_BUFFER_SIZE];
	scanf("%s", pause);
	if (0 == strcmp(a_pauseKey, pause)) {
		*a_runningFlag = 0;
	}
	else {
		PAUSE_MESSAGE(a_pauseKey);
	}
}