#include <stdio.h> 			/* printf */
#include <stdlib.h> 		/* exit */
#include <string.h> 		/* memset */
#include <unistd.h> 		/* close */
#include <sys/socket.h> /* socklen_t, socket, recvfrom, sendto */
#include <sys/types.h> 	/* ssize_t */
#include <netinet/in.h> /* struct sockadder_in */
#include <arpa/inet.h> 	/* htons */

#define SIZE 100
#define PORT 9000

static void Sock_Init(struct sockaddr_in *a_sin);
static void Sock_Create(int *a_sock);
static void Sock_Connect(int *a_sock, struct sockaddr_in *a_sin);
static void Sock_Send(int *a_sock, void* _package);
static void Sock_Receive(int *a_sock, char *_buffer);


typedef struct Package {
	size_t m_signal;
	char m_name[30];
	char m_password[30];
	size_t m_clientNumber;
}Package;


int main() {
	struct sockaddr_in sin;
	int sock;
	char pack[] = { '1', '2', '\0', 'z', 'i', 'v' , '\0' , '1' , '2' , '3' , '\0', '5', '1' , '\0'};
	char buffer[SIZE];
	Sock_Create(&sock);
	Sock_Init(&sin);
	Sock_Connect(&sock, &sin);
	Sock_Send(&sock, pack);
	Sock_Receive(&sock, buffer);
	puts(buffer);
	close(sock);
	return 0;
}


static void Sock_Init(struct sockaddr_in *a_sin) {
	memset (a_sin , 0 , sizeof(*a_sin));
	a_sin->sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &a_sin->sin_addr.s_addr);
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


static void Sock_Connect(int *a_sock, struct sockaddr_in *a_sin) {
	if (connect(*a_sock, (struct sockaddr*)a_sin, sizeof(*a_sin)) < 0 ) {
		perror("connection failed");
		exit(0);
	}
	return;
}


static void Sock_Send(int *a_sock, void *a_package) {
	if (send(*a_sock, a_package, SIZE, 0) < 0) {
		perror("sendto failed");
		exit(0);
	}
	return;
}


static void Sock_Receive(int *a_sock, char *a_buffer) {
	if (recv(*a_sock, a_buffer, SIZE, 0) < 0) {
		perror("recvfrom failed");
		exit(0);
	}
	return;
}
