#include <stdio.h>
#include <string.h>
#include "protocol.h"

typedef size_t (*Pack_Function)(const void* a_data, char* a_pack);
typedef void (*Unpack_Function)(void* a_data, const char* a_pack);

static size_t pack_RegisterRequest(const void* a_data, char* a_pack);
static size_t pack_LogoutRequest(const void* a_data, char* a_pack);
static size_t pack_GroupRequest(const void* a_data, char* a_pack);
static size_t pack_GroupNamesRequest(const void* a_data, char* a_pack);

static size_t pack_RegisterResponse(const void* a_data, char* a_pack);
static size_t pack_GroupResponse(const void* a_data, char* a_pack);
static size_t pack_GroupNamesResponse(const void* a_data, char* a_pack);
static size_t pack_LeaveGroupResponse(const void* a_data, char* a_pack);


static void unpack_RegisterRequest(void* a_data, const char* a_pack);
static void unpack_LogoutRequest(void* a_data, const char* a_pack);
static void unpack_GroupRequest(void* a_data, const char* a_pack);
static void unpack_GroupNamesRequest(void* a_data, const char* a_pack);

static void unpack_ResponseLogInOut(void* a_data, const char* a_pack);
static void unpack_GroupResponse(void* a_data, const char* a_pack);
static void unpack_GroupNamesResponse(void* a_data, const char* a_pack);
static void unpack_LeaveGroupResponse(void* a_data, const char* a_pack);





Protocol_Result Protocol_CheckTag(const char* a_pack, char *a_tag) {
	if (NULL == a_pack) {
		return Protocol_Uninitialized_Error;
	}
	*a_tag = a_pack[0];
	return Protocol_Success;
}


Protocol_Result Protocol_Pack(const void* a_data, char* a_pack, size_t *a_size) {
	/* function for every Tag */
	Pack_Function functions[] = {pack_RegisterRequest, pack_RegisterRequest, pack_LogoutRequest, pack_GroupRequest, pack_GroupRequest, pack_GroupRequest, pack_GroupNamesRequest, 
																pack_RegisterResponse, pack_RegisterResponse, pack_RegisterResponse, pack_GroupResponse, pack_GroupResponse, pack_GroupNamesResponse, pack_LeaveGroupResponse};
	size_t funcTagIndex;
	if (NULL == a_data || NULL == a_pack || NULL == a_size) {
		return Protocol_Uninitialized_Error;
	}
	funcTagIndex = ((char*)a_data)[0];
	*a_size = functions[funcTagIndex](a_data,a_pack);
	return Protocol_Success;
}


Protocol_Result Protocol_Unpack(const char* a_pack, void* a_data) {
	/* function for every Tag */
	Unpack_Function functions[] = {unpack_RegisterRequest, unpack_RegisterRequest, unpack_LogoutRequest, unpack_GroupRequest, unpack_GroupRequest, unpack_GroupRequest, unpack_GroupNamesRequest, 
																unpack_ResponseLogInOut, unpack_ResponseLogInOut, unpack_ResponseLogInOut, unpack_GroupResponse, unpack_GroupResponse, unpack_GroupNamesResponse, unpack_LeaveGroupResponse};
	size_t funcTagIndex;
	if (NULL == a_pack || NULL == a_data || a_pack[0] < 0) {
		return Protocol_Uninitialized_Error;
	}
	funcTagIndex = a_pack[0];
	functions[funcTagIndex](a_data,a_pack);
	return Protocol_Success;
}




/* --------------- Static Pack Functions --------------- */

/* ----- Request Functions ----- */

static size_t pack_RegisterRequest (const void* a_data, char* a_pack) {
	size_t i = 2, size;
	Request_LogIn *reg = (Request_LogIn*)a_data;

	a_pack[0] = reg->m_tag;
	a_pack[1] = '\0';

	size = strlen(reg->m_name) + 1;
	memcpy(&a_pack[i], reg->m_name, size);
	i += size;

	size = strlen(reg->m_password) + 1;
	memcpy(&a_pack[i], reg->m_password, size);
	i += size;

	return i;
}


static size_t pack_LogoutRequest (const void* a_data, char* a_pack) {
	size_t i = 2, size;
	Request_Logout *logout = (Request_Logout*)a_data;

	a_pack[0] = logout->m_tag;
	a_pack[1] = '\0';

	size = strlen(logout->m_name) + 1;
	memcpy(&a_pack[i], logout->m_name, size);
	i += size;

	memcpy(&a_pack[i], logout->m_groupsNames, MAX_BUFFER_SIZE);
	i += MAX_BUFFER_SIZE;

	return i;
}



static size_t pack_GroupRequest (const void* a_data, char* a_pack) {
	size_t i = 2, size;
	Request_Group *group = (Request_Group*)a_data;

	a_pack[0] = group->m_tag;
	a_pack[1] = '\0';

	size = strlen(group->m_groupName) + 1;
	memcpy(&a_pack[i], group->m_groupName, size);
	i += size;

	return i;
}


static size_t pack_GroupNamesRequest (const void* a_data, char* a_pack) {
	Request_GroupsNames *group = (Request_GroupsNames*)a_data;
	a_pack[0] = group->m_tag;
	a_pack[1] = '\0';
	return 2;
}




/* ----- Response Functions ----- */

static size_t pack_RegisterResponse (const void* a_data, char* a_pack) {
	a_pack[0] = ((Response_LogInOut*)a_data)->m_tag;
	a_pack[1] = '\0';
	a_pack[2] = ((Response_LogInOut*)a_data)->m_status;
	return 3;
}


static size_t pack_GroupResponse (const void* a_data, char* a_pack) {
	size_t i = 3, size;
	Response_Group *group = (Response_Group*)a_data;

	a_pack[0] = group->m_tag;
	a_pack[1] = group->m_status;
	a_pack[2] = '\0';

	size = strlen(group->m_ipAddress) + 1;
	memcpy(&a_pack[i], group->m_ipAddress, size);
	i += size;

	size = strlen(group->m_port) + 1;
	memcpy(&a_pack[i], group->m_port, size);
	i += size;

	size = strlen(group->m_groupName) + 1;
	memcpy(&a_pack[i], group->m_groupName, size);
	i += size;

	return i;
}


static size_t pack_GroupNamesResponse (const void* a_data, char* a_pack) {
	Response_GroupNames *group = (Response_GroupNames*)a_data;
	a_pack[0] = group->m_tag;
	a_pack[1] = '\0';
	memcpy(&a_pack[2], group->m_allGroups, MAX_BUFFER_SIZE);
	return MAX_BUFFER_SIZE + 2;
}


static size_t pack_LeaveGroupResponse (const void* a_data, char* a_pack) {
	Response_LeaveGroup *group = (Response_LeaveGroup*)a_data;
	size_t size;

	a_pack[0] = group->m_tag;
	a_pack[1] = '\0';
	a_pack[2] = group->m_status;

	size = strlen(group->m_groupName) + 1;
	/* strcpy(&a_pack[3] , group->m_groupName); */
	memcpy(a_pack + 3, group->m_groupName, size);
	return size + 3;
}



/* --------------- Static Unpack Functions --------------- */

/* ----- Request Functions ----- */

static void unpack_RegisterRequest (void* a_data, const char* a_pack) {
	size_t i = 2;
	Request_LogIn *reg = (Request_LogIn*)a_data;

	reg->m_tag = a_pack[0];

	strcpy(reg->m_name, &a_pack[i]);
	i += strlen(reg->m_name) + 1;

	strcpy(reg->m_password, &a_pack[i]);
	i += strlen(reg->m_password) + 1;
}


static void unpack_LogoutRequest(void* a_data, const char* a_pack) {
	size_t i = 2;
	Request_Logout *logout = (Request_Logout*)a_data;

	logout->m_tag = a_pack[0];

	strcpy(logout->m_name, &a_pack[i]);
	i += strlen(logout->m_name) + 1;
	memcpy(logout->m_groupsNames, &a_pack[i], MAX_BUFFER_SIZE);
}


static void unpack_GroupRequest (void* a_data, const char* a_pack) {
	size_t i = 2;
	Request_Group *group = (Request_Group*)a_data;
	group->m_tag = a_pack[0];
	strcpy(group->m_groupName, &a_pack[i]);
}


static void unpack_GroupNamesRequest (void* a_data, const char* a_pack) {
	((Request_GroupsNames*)a_data)->m_tag = a_pack[0];
}


/* ----- Response Functions ----- */

static void unpack_ResponseLogInOut (void* a_data, const char* a_pack) {
	((Response_LogInOut*)a_data)->m_tag = a_pack[0];
	((Response_LogInOut*)a_data)->m_status = a_pack[2];
	return;
}


static void unpack_GroupResponse (void* a_data, const char* a_pack) {
	size_t i = 3;
	Response_Group *group = (Response_Group*)a_data;
	group->m_tag = a_pack[0];
	group->m_status = a_pack[1];

	strcpy(group->m_ipAddress, &a_pack[i]);
	i += strlen(group->m_ipAddress) + 1;

	strcpy(group->m_port, &a_pack[i]);
	i += strlen(group->m_port) + 1;

	strcpy(group->m_groupName, &a_pack[i]);
	i += strlen(group->m_groupName) + 1;
}


static void unpack_GroupNamesResponse (void* a_data, const char* a_pack) {
	Response_GroupNames *group = (Response_GroupNames*)a_data;
	group->m_tag = a_pack[0];
	memcpy(group->m_allGroups, &a_pack[2], MAX_BUFFER_SIZE);
}


static void unpack_LeaveGroupResponse (void* a_data, const char* a_pack) {
	Response_LeaveGroup *group = (Response_LeaveGroup*)a_data;
	group->m_tag = a_pack[0];
	group->m_status = a_pack[2];
	strcpy(group->m_groupName , &a_pack[3]);
}
