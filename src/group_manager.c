#include <stdlib.h> 		/* malloc, free */
#include <string.h> 		/* strcpy, strlen */
#include <stdio.h> 			/* sprintf */
#include <limits.h> 		/* UCHAR_MAX */
#include "group_manager.h"
#include "group.h"
#include "hashmap.h"
#include "queue.h"

#define MAX_BUFFER_SIZE 4000
#define ADDRESS_SIZE 15
#define ADDRESS_FIRST_BYTE 225
#define PORT "5000"
#define PREV_BYTE(NUM) ((NUM) -= 4)
#define UNITY 2
#define TENS 1
#define HUNDREDS 0


struct GroupManager {
	HashMap *m_groups; 		/* key = group name, value = group */
	Queue *m_addressBank; /* Pull of IP addresses for Groups */
};

typedef struct Stream {
	char m_groupsNames[MAX_BUFFER_SIZE];
	size_t m_index;
}Stream;


static GroupManager_Result fillAddressesBank(Queue *a_addressesBank, size_t a_pullSize);
static void updateAddress(char *a_address, unsigned char *a_num1, unsigned char *a_num2, unsigned char *a_num3, unsigned char *a_num4);
static size_t hashFunc(const void *a_key); /* return the last 6 digits of an address */
static int groupEqualityCheck(const void *a_firstKey, const void *a_secondKey);
static int OpenGroup_checkMapFindStatus(int a_status);
static void groupDestroy(void *a_key);
static int fillStreamWithGroupsNames(const void *a_key, void *a_value, void *a_context);



GroupManager* GroupManager_Create(size_t a_numOfGroups) {
	GroupManager *groupM = NULL;
	if (NULL == (groupM = (GroupManager*)malloc(sizeof(GroupManager)))) {
		return NULL;
	}
	if (NULL == (groupM->m_groups = HashMap_Create(a_numOfGroups, hashFunc, groupEqualityCheck))) {
		free(groupM);
		return NULL;
	}
	if (NULL == (groupM->m_addressBank = Queue_Create(a_numOfGroups))) {
		free(groupM->m_groups);
		free(groupM);
		return NULL;
	}
	if (GroupManager_Success != fillAddressesBank(groupM->m_addressBank, a_numOfGroups)) {
		return NULL;
	}
	return groupM;
}


void GroupManager_Destroy(GroupManager **a_groupM) {
	if (NULL == a_groupM || NULL == *a_groupM) {
		return;
	}
	Queue_Destroy(&(*a_groupM)->m_addressBank , free);
	HashMap_Destroy(&(*a_groupM)->m_groups, NULL , groupDestroy);
	free(*a_groupM);
	*a_groupM = NULL;
}


GroupManager_Result GroupManager_OpenGroup(GroupManager *a_groupM, char *a_groupName, char *a_address, char *a_port) {
	Group *group = NULL;
	Map_Result statusMap;
	Queue_Result statusQueue;
	char *address = NULL;
	if (NULL == a_groupM || NULL == a_groupName || NULL == a_address || NULL == a_port) {
		return GroupManager_Uninitialized_Error;
	}

	/* Looking for duplicate Group name */
	if (Map_KeyNotFound_Error != (statusMap = HashMap_Find(a_groupM->m_groups, a_groupName, (void**)&group))) {
		return OpenGroup_checkMapFindStatus(statusMap);
	}
	/* getting an address from the Queue bank */
	if (Queue_Success != (statusQueue = Queue_Dequeue(a_groupM->m_addressBank, (void**)&address))) {
		return GroupManager_Overflow_Error;
	}
	/* Creating a new group */
	if (NULL == (group = Group_Create(a_groupName, address, PORT))) {
		Queue_Enqueue(a_groupM->m_addressBank, address);
		return GroupManager_Allocation_Error;
	}
	Group_AddUser(group);
	/* Adding the new group to the Groups container */
	if (Map_Success != (statusMap = HashMap_Insert(a_groupM->m_groups, Group_GetName(group), group))) {
		Queue_Enqueue(a_groupM->m_addressBank, address);
		Group_Destroy(&group);
		return OpenGroup_checkMapFindStatus(statusMap);
	}
	strcpy(a_address , address);
	strcpy(a_port , PORT);
	return GroupManager_Success;
}


GroupManager_Result GroupManager_AddUser(GroupManager *a_groupM, char *a_groupName, char *a_address, char *a_port) {
	Group *group = NULL;
	if (NULL == a_groupM || NULL == a_groupName) {
		return GroupManager_Uninitialized_Error;
	}
	if (Map_Success != HashMap_Find(a_groupM->m_groups, a_groupName, (void**)&group)) {
		return GroupManager_GroupNotExists_Error;
	}
	Group_AddUser(group);
	strcpy(a_address, Group_GetAddress(group));
	strcpy(a_port, Group_GetPort(group));
	return GroupManager_Success;
}


GroupManager_Result GroupManager_RemoveUser(GroupManager *a_groupM, char *a_groupName) {
	Group *group = NULL;
	void *removedGroupName = NULL, *removedGroup = NULL;
	char *address = NULL;
	if (NULL == a_groupM || NULL == a_groupName) {
		return GroupManager_Uninitialized_Error;
	}
	if (Map_Success != HashMap_Find(a_groupM->m_groups, a_groupName, (void**)&group)) {
		return GroupManager_GroupNotExists_Error;
	}
	/* close the group if it contains only one user */
	if (1 == Group_Size(group)) {
		HashMap_Remove(a_groupM->m_groups, a_groupName, &removedGroupName, &removedGroup);
		address = Group_Destroy(&group);
		if(Queue_Success != Queue_Enqueue(a_groupM->m_addressBank, address)) {
			return GroupManager_Allocation_Error;
		}
	}
	else {
		Group_RemoveUser(group);
	}
	return GroupManager_Success;
}


void GroupManager_GetAllGroups(GroupManager *a_groupM, char *a_groupsNames) {
	Stream stream;
	size_t numOfGroups;
	if (NULL == a_groupM || NULL == a_groupsNames) {
		return;
	}
	stream.m_index = 2; 										/* first index for groups names */
	numOfGroups = HashMap_ForEach(a_groupM->m_groups, fillStreamWithGroupsNames , &stream);
	stream.m_groupsNames[0] = numOfGroups;  /* num of groups in the first byte */
	stream.m_groupsNames[1] = '\0'; 				/* '\0' as a separate delimiter */
	memcpy(a_groupsNames, stream.m_groupsNames, stream.m_index);
}


size_t GroupManager_NumOfGroups(const GroupManager *a_groupM) {
	return (NULL == a_groupM ? 0 : HashMap_Size(a_groupM->m_groups));
}


size_t GroupManager_UsersInGroup(const GroupManager *a_groupM, char *a_groupName) {
	Group *group = NULL;
	if (NULL == a_groupM || NULL == a_groupName) {
		return 0;
	}
	if (Map_Success != HashMap_Find(a_groupM->m_groups, a_groupName, (void**)&group)) {
		return GroupManager_GroupNotExists_Error;
	}
	return Group_Size(group);
}


/* ---------- Local Functions ---------- */

static GroupManager_Result fillAddressesBank(Queue *a_addressesBank, size_t a_pullSize) {
	char *address = NULL;
	unsigned char num1 = ADDRESS_FIRST_BYTE, num2 = 0, num3 = 0, num4 = 1;
	size_t i;
	for (i = 0 ; i < a_pullSize ; ++i) {
		address = (char*)malloc(ADDRESS_SIZE + 1);
		updateAddress(address, &num1, &num2, &num3, &num4);
		if (Queue_Success != Queue_Enqueue(a_addressesBank , address)) {
			return -1;
		}
	}
	return GroupManager_Success;
}


static void updateAddress(char *a_address, unsigned char *a_num1, unsigned char *a_num2, unsigned char *a_num3, unsigned char *a_num4) {
	sprintf(a_address, "%d.%d.%d.%d" , *a_num1, *a_num2, *a_num3, *a_num4);
	if (*a_num4 < UCHAR_MAX) {
		++(*a_num4);
	}
	else if (*a_num3 < UCHAR_MAX) {
		*a_num4 = 1;
		++(*a_num3);
	}
	else {
		*a_num4 = 1;
		*a_num3 = 0;
		++(*a_num2);
	}
}




/* ---------- Hash functions ---------- */

 /* make a number from the Group Name */
static size_t hashFunc(const void *a_key) {
	char *groupName = (char*)a_key;
	size_t i, len = strlen(groupName), letterSum = 0;
	for (i = 0 ; groupName[i] != '\0' ; ++i) {
		letterSum += groupName[i];
	}
	return (len * letterSum);
}


static int groupEqualityCheck(const void *a_firstKey, const void *a_secondKey) {
	return (strcmp((char*)a_firstKey , (char*)a_secondKey) == 0);
}


static int OpenGroup_checkMapFindStatus(int status) {
	switch (status) {
		case Map_Success:
			return GroupManager_GroupAlreadyExists_Error;

		case Map_Allocation_Error:
			return GroupManager_Allocation_Error;

		case Map_KeyDuplicate_Error:
			return GroupManager_GroupAlreadyExists_Error;

		case Map_Overflow_Error:
			return GroupManager_Overflow_Error;

		case Map_Uninitialized_Error:
			return GroupManager_Uninitialized_Error;

		default:
			return -1;
	}
}


static void groupDestroy(void *a_value) {
	char *address = NULL;
	Group *group = (Group*)a_value;
	address = Group_Destroy(&group);
	free(address);
}


static int fillStreamWithGroupsNames(const void *a_key, void *a_value, void *a_context) {
	char *name = (char*)a_key , *groupsNames = ((Stream*)a_context)->m_groupsNames;
	size_t size, index = ((Stream*)a_context)->m_index;
	size = strlen(name) + 1;
	memcpy(groupsNames + index, name, size);
	((Stream*)a_context)->m_index += size;
	return 1;
}
