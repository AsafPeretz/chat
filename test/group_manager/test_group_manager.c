#include <stdio.h>
#include <string.h>
#include "group_manager.h"
#include "group.h"
#include "GenQueue.h"

#define PRINT_TEST(a_testResult, a_testName) if (!a_testResult) { printf("%s\n", a_testName); }

static int Test_Create(char* a_testName);
static int Test_Destroy(char* a_testName);
static int Test_Open_Group(char* a_testName);
static int Test_Get_All_Groups(char* a_testName);
static int Test_Add_User(char* a_testName);
static int Test_Remove_User(char* a_testName);


int main() {
	char testName[100];
	PRINT_TEST(Test_Create(testName), testName);
	PRINT_TEST(Test_Destroy(testName), testName);
	PRINT_TEST(Test_Open_Group(testName), testName);
	PRINT_TEST(Test_Get_All_Groups(testName), testName);
	PRINT_TEST(Test_Add_User(testName), testName);
	PRINT_TEST(Test_Remove_User(testName), testName);

	return 0;
}


static int Test_Create(char* a_testName) {
	int numOfGroups = 200;
	GroupManager *groupM = GroupManager_Create(numOfGroups);
	strcpy(a_testName, "Test_Create");
	if (NULL == groupM) {
		return 0;
	}
	GroupManager_Destroy(&groupM);
	return 1;
}


static int Test_Destroy(char* a_testName) {
	int numOfGroups = 200;
	GroupManager *groupM = GroupManager_Create(numOfGroups);
	strcpy(a_testName, "Test_Destroy");
	GroupManager_Destroy(&groupM);
	if (NULL != groupM) {
		return 0;
	}
	GroupManager_Destroy(&groupM);
	return 1;
}


static int Test_Open_Group(char* a_testName) {
	int numOfGroups = 1000;
	char port [10], address[16];
	GroupManager *groupM = GroupManager_Create(numOfGroups);
	strcpy(a_testName, "Test_Open_Group");
	
	if (GroupManager_Success != GroupManager_OpenGroup (groupM, "ZivMaorAsaf" , address, port)) {
		return 0;
	}
	if (1 != GroupManager_UsersInGroup(groupM, "ZivMaorAsaf")) {
		return 0;
	}
	if (GroupManager_Group_Already_Exists_Error != GroupManager_OpenGroup (groupM, "ZivMaorAsaf", address, port)) {
		return 0;
	}
	if (GroupManager_Success != GroupManager_OpenGroup (groupM, "Team3", address, port)) {
		return 0;
	}
	if (GroupManager_Group_Already_Exists_Error != GroupManager_OpenGroup (groupM, "Team3", address, port)) {
		return 0;
	}
	GroupManager_Destroy(&groupM);
	return 1;
}


static int Test_Get_All_Groups(char* a_testName) {
	int groupAmount;
	int numOfGroups = 1000;
	char port [10], address[16];
	char groupStream[4096];
	GroupManager *groupM = GroupManager_Create(numOfGroups);
	strcpy(a_testName, "Test_Get_All_Groups");

	GroupManager_OpenGroup (groupM, "zzzz" , address, port);
	GroupManager_OpenGroup (groupM, "mmmm" , address, port);
	GroupManager_OpenGroup (groupM, "aaaa" , address, port);
	GroupManager_OpenGroup (groupM, "bbbb" , address, port);

	GroupManager_GetAllGroups (groupM, groupStream);
	if (4 != (groupAmount = groupStream[0])) {
		return 0;
	}
	if (strcmp("aaaa" , &groupStream[2]) || strcmp("bbbb" , &groupStream[7]) || strcmp("mmmm" , &groupStream[12]) || strcmp("zzzz" , &groupStream[17]) ) {
		return 0;
	}
	GroupManager_Destroy(&groupM);
	return 1;
}


static int Test_Add_User(char* a_testName) {
	int numOfGroups = 1000;
	char port [10], address[16];
	GroupManager *groupM = GroupManager_Create(numOfGroups);
	strcpy(a_testName, "Test_Add_User");

	GroupManager_OpenGroup (groupM, "Ziv" , address, port);
	GroupManager_AddUser(groupM ,"Ziv", address, port);
	GroupManager_AddUser(groupM ,"Ziv", address, port);
	GroupManager_AddUser(groupM ,"Ziv", address, port);
	GroupManager_AddUser(groupM ,"Ziv", address, port);

	if( strcmp("225.0.0.1" , address) || strcmp("5000" , port)) {
		return 0;
	}
	if(5 != GroupManager_UsersInGroup(groupM, "Ziv")) {
		return 0;
	}
	GroupManager_Destroy(&groupM);
	return 1;
}


static int Test_Remove_User(char* a_testName) {
	int numOfGroups = 1000;
	char port [10], address[16];
	GroupManager *groupM = GroupManager_Create(numOfGroups);
	strcpy(a_testName, "Test_Remove_User");

	GroupManager_OpenGroup (groupM, "Ziv" , address, port);
	GroupManager_AddUser(groupM ,"Ziv", address, port);
	GroupManager_AddUser(groupM ,"Ziv", address, port);
	GroupManager_AddUser(groupM ,"Ziv", address, port);
	GroupManager_AddUser(groupM ,"Ziv", address, port);

	if (5 != GroupManager_UsersInGroup(groupM, "Ziv")) {
		return 0;
	}
	if (1 != GroupManager_NumOfGroups(groupM)) {
		return 0;
	}
	GroupManager_RemoveUser(groupM ,"Ziv");
	GroupManager_RemoveUser(groupM ,"Ziv");
	if (3 != GroupManager_UsersInGroup(groupM, "Ziv")) {
		return 0;
	}
	GroupManager_RemoveUser(groupM ,"Ziv");
	GroupManager_RemoveUser(groupM ,"Ziv");
	if (1 != GroupManager_UsersInGroup(groupM, "Ziv")) {
		return 0;
	}
	if (1 != GroupManager_NumOfGroups(groupM)) {
		return 0;
	}
	GroupManager_RemoveUser(groupM ,"Ziv");
	if (0 != GroupManager_NumOfGroups(groupM)) {
		return 0;
	}
	GroupManager_Destroy(&groupM);
	return 1;
}
