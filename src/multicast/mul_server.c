#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>/*getpid*/
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 4096
#define STRING_FILE_SIZE 50


static int initSocket(struct sockaddr_in *a_sinServer, struct ip_mreq *a_mreq, int a_port, char *groupAddress);
static void bindSocket(int a_sock, struct sockaddr_in *a_sinServer);
static void recivedSocket(int a_sock, struct sockaddr_in *a_sinServer);
static void savePid(char *a_groupName, char *a_userName);


/* arg[1]  - Group ip address
	 arg[2]  - Port number	*/
int main(int argc, char **argv) {
	char *groupAddress = NULL, *groupName = NULL, *userName = NULL;
	int port, sock;
	struct sockaddr_in sinServer;
	struct ip_mreq mreq;

	if (argc != 5) {
		printf("wrong argument as been inserted\n");
		return 1;
	}
	groupAddress = argv[1]; 
	port = atoi(argv[2]);
	groupName = argv[3];
	userName = argv[4];
	savePid(groupName, userName);

	sock = initSocket(&sinServer, &mreq, port, groupAddress);
	bindSocket(sock, &sinServer);
	printf("this window is the muilticast windos,don't right in here\n\n");
	printf("welcome to group:  %s. \n",groupName);
	while (1) {
		recivedSocket(sock,&sinServer);	
	}
	return 0;
}

static int initSocket(struct sockaddr_in *a_sinServer, struct ip_mreq *a_mreq, int a_port, char *groupAddress) {
	__u_int optval = 1;
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		perror("socket falied");
		exit(0);
	}

	/* allows multiple sockets to use the same port */
	if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval)) < 0) {
		perror("setsockopt failed");
		abort();
	}
	memset(a_sinServer, 0, sizeof(*a_sinServer));
	a_sinServer->sin_family = AF_INET;
	a_sinServer->sin_addr.s_addr = inet_addr(groupAddress);
	a_sinServer->sin_port = htons(a_port);

	/* add sock to the multicast group address*/
	a_mreq->imr_multiaddr.s_addr = inet_addr(groupAddress);
	a_mreq->imr_interface.s_addr = htonl(INADDR_ANY);
	if (setsockopt(a_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)a_mreq, sizeof(*a_mreq)) < 0) {
		perror("setsockopt failed");
		exit(0);
	}
	return sock;
}

static void savePid(char *a_groupName, char *a_userName) {
	FILE* file = NULL;
	char fileStr[STRING_FILE_SIZE];
	sprintf(fileStr,"Files/%s%s", _groupName, _userName);
	file = fopen(fileStr, "a");
	fprintf(file,"%d\n", getpid());
	fclose(file);
}

static void recivedSocket(int a_sock, struct sockaddr_in *a_sinServer) {
	socklen_t sin_len = sizeof(*a_sinServer);
	char buffer[BUFFER_SIZE];
	ssize_t read_bytes;
	read_bytes = recvfrom(a_sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*)a_sinServer, &sin_len);
	if(read_bytes < 0) {
		perror("revfrom falied");
	}
	printf("%s\n", buffer);
}


static void bindSocket(int a_sock, struct sockaddr_in *a_sinServer) {
	if (bind(a_sock, (struct sockaddr*)a_sinServer, sizeof(*a_sinServer)) < 0) {
		perror("bind falied");
		exit(0);
	}
}

