#include <stdio.h> 			/* printf */
#include <stdlib.h> 		/* exit */
#include <string.h> 		/* memset */
#include <unistd.h> 		/* close */
#include <sys/socket.h> /* socklen_t, socket, recvfrom, sendto */
#include <sys/types.h> 	/* ssize_t */
#include <netinet/in.h> /* struct sockadder_in */
#include <sys/select.h> /* fd_set */
#include <arpa/inet.h> 	/* htons */
#include <fcntl.h> 			/* fcntl */

#define SIZE 100
#define PORT 9000
#define STDIN_SOCKET 0
#define LOCAL_HOST_ADDRESS "127.0.0.1"

static void Sock_Init(struct sockaddr_in *a_sin);
static void Sock_Create(int *a_sock);
static void Sock_Set(int *a_sock, int *a_optval, fd_set *a_readfds);
static void Sock_Bind(int *a_sock, struct sockaddr_in *a_sin);
static void Sock_Listen(int *a_sock, size_t a_queueSize);
static void Sock_Accept(int *a_sock, int *a_sockClient, struct sockaddr_in *a_sin);
static void Sock_Receive(int *a_sock);
static void Sock_Send(int *a_sock, void *a_package, size_t a_len);


typedef struct Package {
	size_t m_signal;
	char m_name[30];
	char m_password[30];
	size_t m_clientNumber;
}Package;


int main() {
	struct sockaddr_in sin;
	int sock, clientSock, optval = 1/*, activity*/;
  fd_set readfds/*, readCurrent*/;

	Sock_Create(&sock);
	Sock_Init(&sin);
  Sock_Set(&sock, &optval, &readfds);
  Sock_Bind(&sock, &sin);
  Sock_Listen(&sock, 10);
	
	/* "Test_ClientConnect" */
	Sock_Accept(&sock, &clientSock, &sin);

	/* "Test_ClientSend" */
	Sock_Accept(&sock, &clientSock, &sin);
	Sock_Receive(&clientSock);

	/* "Test_ClientSend_Neg" */
	Sock_Accept(&sock, &clientSock, &sin);

	/* "Test_ClientReceive" */
	Sock_Accept(&sock, &clientSock, &sin);
	Sock_Receive(&clientSock);

	/* "Test_ClientReceive_Neg" */
	Sock_Accept(&sock, &clientSock, &sin);

	close(sock);
	return 0;
}


static void Sock_Init(struct sockaddr_in *a_sin) {
	memset(a_sin , 0 , sizeof(*a_sin));
	a_sin->sin_family = AF_INET;
	inet_pton(AF_INET, LOCAL_HOST_ADDRESS, &a_sin->sin_addr.s_addr);
	a_sin->sin_port = htons(PORT);
	return;	
}


static void Sock_Create(int *a_sock) {
	*a_sock = socket (AF_INET , SOCK_STREAM, 0);
	if (*a_sock < 0) {
		perror("socket failed");
		exit(0);
	}
	return;
}


static void Sock_Set(int *a_sock, int *a_optval, fd_set *a_readfds) {
  int flags;
	FD_SET(*a_sock, a_readfds);
	FD_SET(STDIN_SOCKET, a_readfds);
	if (setsockopt(*a_sock, SOL_SOCKET, SO_REUSEADDR, a_optval, sizeof(*a_optval)) < 0) {
		exit(0);
	}
	flags = fcntl(*a_sock , F_GETFL);
	if (-1 == flags) {
		exit(0);
	}
	if (-1 == fcntl(*a_sock , F_SETFL, flags)) {
		exit(0);
	}
}


static void Sock_Bind(int *a_sock, struct sockaddr_in *a_sin) {
	if (bind(*a_sock, (struct sockaddr*)a_sin, sizeof(*a_sin)) < 0) {
    perror("bind failed");
		exit(0);
	}
}

static void Sock_Listen(int *a_sock, size_t a_queueSize) {
	if (listen(*a_sock, a_queueSize) < 0) {
    perror("listen failed");
		exit(0);
	}
}


static void Sock_Accept(int *a_sock, int *a_sockClient, struct sockaddr_in *a_sin) {
  socklen_t sin_len = sizeof(a_sin);
  *a_sockClient = accept(*a_sock, (struct sockaddr*)a_sin, &sin_len);
	if (0 > *a_sockClient) {
		perror("select failed");
		exit(0);
	}
}


static void Sock_Receive(int *a_sock) {
  char message[] = "server send back", buffer[SIZE] = "empty buffer";
  if (recv(*a_sock, buffer, sizeof(buffer), 0) < 0) {
		perror("recv failed");
	}
	printf("%s\n", buffer);
  if (0 == strcmp(buffer, "send")) {
    Sock_Send(a_sock, message, strlen(message) + 1);
  }
}


static void Sock_Send(int *a_sock, void *a_package, size_t a_len) {
	if (send(*a_sock, a_package, a_len, 0) < 0) {
		perror("sendto failed");
		exit(0);
	}
}
