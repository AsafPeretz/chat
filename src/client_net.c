#include <stdlib.h> 
#include <stdio.h> 
#include <errno.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>     
#include "client_net.h"

#define NOT_VALID_PORT(port) (0 > port || port > 64000)
 
struct ClientNet {
	int   m_sock;
	int   m_port;
	char *m_address;
};


static void initAddress(ClientNet* a_client, struct sockaddr_in *a_sin);


ClientNet* ClientNet_Create(char *a_address, int a_port) {
	ClientNet *client = NULL;
	size_t addrLen = 0;
	if (NULL == a_address || NOT_VALID_PORT(a_port)) {
		return NULL;
	}
	client = (ClientNet*)malloc(sizeof(ClientNet));
	if (NULL == client) {
		return NULL;
	}
	addrLen = strlen(a_address) + 1;
	client->m_address = (char*)malloc(sizeof(char) * addrLen);
	if (NULL == client->m_address) {
		free(client);
		return NULL;
	}
	client->m_port = a_port;
	strcpy(client->m_address, a_address);
	client->m_sock = 0;
	return client;
}


void ClientNet_Destroy(ClientNet **a_client) {
	if (NULL == a_client || NULL == *a_client) {
		return;
	}
	close((*a_client)->m_sock);
	free(*a_client);
	*a_client = NULL;
}


ClientNet_Result ClientNet_Connect(ClientNet *a_client) {
 	struct sockaddr_in sin;
 	if (NULL == a_client) {
 		return ClientNet_Uninitialized_Error;
 	}
	a_client->m_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == a_client->m_sock) {
		return ClientNet_Connect_Error;
	}
	initAddress(a_client, &sin);
	if (connect(a_client->m_sock, (struct sockaddr*)&sin, sizeof(sin)) < 0) {
		return ClientNet_Connect_Error;
	} 
	return ClientNet_Success;
}


ClientNet_Result ClientNet_Send(ClientNet *a_client, char *a_message, size_t a_msgLength) {
	if (NULL == a_client|| NULL == a_message) {
 		return ClientNet_Uninitialized_Error;
 	}
	send(a_client->m_sock, a_message, a_msgLength, 0);
	return ClientNet_Success;
} 


ClientNet_Result ClientNet_Receive(ClientNet *a_client, char *a_buffer, size_t a_bufferSize) {
  ssize_t readBytes = 0; 
	if (NULL == a_client|| NULL == a_buffer) {
 		return ClientNet_Uninitialized_Error;
 	}
 	readBytes = recv(a_client->m_sock, a_buffer, a_bufferSize ,0); 
	if (0 >= readBytes) {
		/* socket has been closed */
		if (0 == readBytes) {
			return ClientNet_SocketHasBeenClosed_Error;	
		}
		/* an error */
		else {
			return ClientNet_MessageReceive_Error;		
		}
	}
	return ClientNet_Success;
}


int ClientNet_GetSock(ClientNet *a_client) {
  return (NULL == a_client) ? -1 :  a_client->m_sock;
}



/* static functions */

static void initAddress(ClientNet *a_client, struct sockaddr_in *a_sin) {
	memset(a_sin, 0, sizeof(*a_sin));	
	a_sin->sin_family = AF_INET;
	a_sin->sin_addr.s_addr = inet_addr(a_client->m_address);
	a_sin->sin_port = htons(a_client->m_port);
}
