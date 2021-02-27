#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> 
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_DATA_SIZE 4096
#define STRING_FILE_SIZE 50


static void initSin(struct sockaddr_in* _sinClient,char* _group, int _port);
static void sendSocket(int _sock, struct sockaddr_in *_sinClient, char _data[]);
static int initSocket(struct sockaddr_in* _sinClient, char* _group, int _port);
static void savePid(char* _groupName, char* _userName);


/* arg[1]  - Group number
	 arg[2]  - Port number	*/
int main(int argc, char **argv) {
	char *group = NULL, *groupName = NULL, *userName = NULL, data[MAX_DATA_SIZE], dataToSend[MAX_DATA_SIZE];;
	int port, sock;
	struct sockaddr_in sinClient;

	if (argc != 5) {
		printf("wrong argument as been inserted\n");	
		return 1;
	}
	group = argv[1];
	port = atoi(argv[2]);
	groupName = argv[3];
	userName = argv[4];

	savePid(groupName, userName);
	sock = initSocket(&sinClient, group, port);
	printf("This window is for writing only, please write your messages here:\n");
	printf("hello %s !, welcome %s group !\n",groupName, userName);

	while (1) {
		fgets(data, MAX_DATA_SIZE, stdin); 
		sprintf(dataToSend,"%s: %s", userName, data);
		sendSocket(sock, &sinClient, dataToSend);
		sleep(1); 
	}
	return 0;
}

static void savePid(char *a_groupName, char *a_userName) {
	FILE* file = NULL;
	char fileStr[STRING_FILE_SIZE];

	sprintf(fileStr,"./Files/%s%s", _groupName, _userName);
	file = fopen(fileStr, "a");
	fprintf(file,"%d\n", getpid());
	fclose(file);
}

static void sendSocket(int a_sock, struct sockaddr_in *a_sinClient, char *a_data) {
	ssize_t sent_bytes;
	sent_bytes = sendto(a_sock, a_data, strlen(a_data) + 1, 0, (struct sockaddr*)a_sinClient, sizeof(*a_sinClient));
	if(sent_bytes < 0) {
		perror("sento falier");
		exit(0);
	}
}

static int initSocket(struct sockaddr_in *a_sinClient, char *a_group, int a_port) {
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0) {
		perror("socket falied");
		exit(0);
	}
	initSin(a_sinClient, a_group, a_port);
	return sock;
}


static void initSin(struct sockaddr_in *a_sinClient,char *a_group, int a_port) {
	memset(a_sinClient, 0, sizeof(*a_sinClient));
	a_sinClient -> sin_family = AF_INET;
	a_sinClient -> sin_addr.s_addr = inet_addr(a_group);
	a_sinClient -> sin_port = htons(a_port);
}
