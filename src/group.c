#include <stdlib.h>
#include <string.h>
#include "group.h"

#define PORT_SIZE 6

struct Group {
	char *m_name;
	char *m_address;
	char m_port[PORT_SIZE];
	size_t m_numOfUsers;
};


Group* Group_Create(char *a_groupName, char *a_address, char *a_port) {
	Group *group = NULL;
	if(NULL == a_groupName || NULL == a_address || NULL == a_port) {
		return NULL;
	}

	if(NULL == (group = (Group*)calloc(1 , sizeof(Group)))) {
		return NULL;
	}
	if(NULL == (group->m_name = (char*)malloc(strlen(a_groupName) + 1))) {
		free(group);
		return NULL;
	}

	strcpy(group->m_name , a_groupName);
	group->m_address = a_address;
	strcpy(group->m_port , a_port);

	return group;
}


char* Group_Destroy(Group **a_group) {
	char *address = NULL;
	if(NULL == a_group || NULL == *a_group) {
		return NULL;
	}
	address = (*a_group)->m_address;
	free(*a_group);
	return address;
}


void Group_AddUser(Group *a_group) {
	if(NULL == a_group) {
		return;
	}
	++a_group->m_numOfUsers;
}


void Group_RemoveUser(Group *a_group) {
	if(NULL == a_group) {
		return;
	}
	--a_group->m_numOfUsers;
}


size_t Group_Size(const Group *a_group) {
	return (NULL == a_group ? 0 : a_group->m_numOfUsers);
}


const char* Group_GetName(const Group *a_group) {
	if(NULL == a_group) {
		return NULL;
	}
	return a_group->m_name;
}


void Group_ChangeName(Group *a_group, const char *a_newName) {
	if(NULL == a_group || NULL == a_newName) {
		return;
	}
	strcpy(a_group->m_name , a_newName);
}


const char* Group_GetAddress (const Group *a_group) {
	if(NULL == a_group) {
		return NULL;
	}
	return a_group->m_address;
}


const char* Group_GetPort (const Group *a_group) {
	if(NULL == a_group) {
		return NULL;
	}
		return a_group->m_port;
}
