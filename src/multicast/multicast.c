#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "multicast.h"

#define STRING_SIZE 200
#define PID_SIZE 10
#define SYSTEM_STRING_SIZE 60
#define PORT_SIZE 6
#define MAX_PORT 64000

static int fileCreate(char *a_groupName ,char *a_userName);

Multicast_Result Multicast_Create(char *a_group, int a_port, char *a_groupName, char *a_userName) {
	char str[STRING_SIZE];
	if(a_port < 0 || a_port > MAX_PORT) {
		return Multicast_Port_Error;
	}
	if (!fileCreate(a_groupName, a_userName)) {
		return Multicast_File_Not_Exists_Error;
	}
	sprintf(str, "gnome-terminal -- ./mul_server.out '%s' '%d' '%s' '%s'", a_group, a_port, a_groupName, a_userName);
	if (-1 == system(str)) {
		return Multicast_Unknown_Error;
	}
	sprintf(str, "gnome-terminal -- ./mul_client.out '%s' '%d' '%s' '%s'", a_group, a_port, a_groupName, a_userName);
	if (-1 == system(str)) {
		return Multicast_Unknown_Error;
	}
	return Multicast_Success;
}


Multicast_Result Multicast_Destroy(char *a_groupName, char *a_userName) {
	FILE* file;
	char fileName[STRING_SIZE], pid[PID_SIZE], SysStr[SYSTEM_STRING_SIZE];
	size_t len;

	sprintf(fileName,"./Files/%s%s", a_groupName, a_userName);
	file = fopen(fileName, "r");
	if (NULL == file) {
		return Multicast_File_Not_Exists_Error;
	}
	while (!feof(file)) {
		if (!fgets(pid, STRING_SIZE, file)) {
			break;
		}
		len = strlen(pid);
		pid[len - 1] = '\0';
		sprintf(SysStr, "kill -9 %s", pid);	
		if (-1 == system(SysStr)) {
			return Multicast_Unknown_Error;
		}
	}
	fclose(file);
	remove(fileName);
	return Multicast_Success;
}

static int fileCreate(char *a_groupName ,char *a_userName) {
	FILE* file;
	char fileStr[STRING_SIZE];

	/* check if the file exists */
	sprintf(fileStr,"./Files/%s%s", a_groupName, a_userName);
	file = fopen(fileStr, "w");
	if (NULL == file) {
		return 0;
	}
	fclose(file);
	return 1;
}
