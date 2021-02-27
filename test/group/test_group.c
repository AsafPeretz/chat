#include <stdio.h>
#include <string.h>
#include "../../inc/group.h"

#define PRINT_TEST(a_testResult, a_testName) if (!a_testResult) { printf("%s\n", a_testName); }

static int Test_Create(char *a_testName);
static int Test_Create_Neg(char *a_testName);
static int Test_Destroy(char *a_testName);
static int Test_Destroy_Neg(char *a_testName);
static int Test_Add_Remove_Size(char *a_testName);
static int Test_Get_Name(char *a_testName);
static int Test_Change_Name(char *a_testName);

int main() {
	char testName[100];
	PRINT_TEST(Test_Create(testName), testName);
	PRINT_TEST(Test_Create_Neg(testName), testName);
	PRINT_TEST(Test_Destroy(testName), testName);
	PRINT_TEST(Test_Destroy_Neg(testName), testName);
	PRINT_TEST(Test_Add_Remove_Size(testName), testName);
	PRINT_TEST(Test_Get_Name(testName), testName);
	PRINT_TEST(Test_Change_Name(testName), testName);
	return 0;
}



static int Test_Create(char *a_testName) {
	Group *group = Group_Create("Group1" , "127.0.0.1", "5000");
	strcpy(a_testName, "Test_Create");
	if (NULL == group) {
		return 0;
	}
	Group_Destroy(&group);
	return 1;
}


static int Test_Create_Neg(char *a_testName) {
	Group *group1 = NULL, *group2 = NULL, *group3 = NULL;
	strcpy(a_testName, "Test_Create_Neg");
	group1 = Group_Create(NULL , "127.0.0.1", "5000");
	group2 = Group_Create("Groupppp" , NULL, "5000");
	group3 = Group_Create("Groupppp2" , "127.0.0.1", NULL);
	if (NULL != group1 || NULL != group2 || NULL != group3) {
		return 0;
	}
	return 1;
}


static int Test_Destroy(char *a_testName) {
	char *addr = NULL;
	Group *group = Group_Create("Group1" , "127.0.0.1", "5000");
	strcpy(a_testName, "Test_Destroy");
	addr = Group_Destroy(&group);
	if (strcmp("127.0.0.1" , addr)) {
		return 0;
	}
	return 1;
}


static int Test_Destroy_Neg(char *a_testName) {
	Group *group = Group_Create("Group1" , "127.0.0.1", "5000");
	strcpy(a_testName, "Test_Destroy_Neg");
	Group_Destroy(&group);
	Group_Destroy(&group);
	Group_Destroy(NULL);
	return 1;
}


static int Test_Add_Remove_Size(char *a_testName) {
	char *_address;
	Group *group = Group_Create("Group1" , "127.58.22.1", "5000");
	strcpy(a_testName, "Test_Add_Remove_Size");
	if (0 != Group_Size(group)) {
		return 0;
	}
	Group_AddUser(group);
	Group_AddUser(group);
	Group_AddUser(group);
	if (3 != Group_Size(group)) {
		return 0;
	}
	Group_RemoveUser(group);
	if (2 != Group_Size(group)) {
		return 0;
	}
	Group_AddUser(group);
	Group_AddUser(group);
	if (4 != Group_Size(group)) {
		return 0;
	}
	Group_RemoveUser(group);
	Group_RemoveUser(group);
	Group_RemoveUser(group);
	Group_RemoveUser(group);
	if (0 != Group_Size(group)) {
		return 0;
	}
	Group_AddUser(group);
	if (1 != Group_Size(group)) {
		return 0;
	}
	_address = Group_Destroy(&group);
	if (strcmp("127.58.22.1" , _address)) {
		return 0;
	}
	return 1;
}


static int Test_Get_Name(char *a_testName) {
	const char *name = NULL;
	Group *group = Group_Create("Group1" , "127.0.0.1", "5000");
	strcpy(a_testName, "Test_Get_Name");
	name = Group_GetName(group);
	if (strcmp("Group1" , name)) {
		return 0;
	}
	Group_Destroy(&group);
	return 1;
}


static int Test_Change_Name(char *a_testName) {
	const char *newName;
	Group *group = Group_Create("Group1" , "127.0.0.1", "5000");
	strcpy(a_testName, "Test_Change_Name");
	Group_ChangeName(group, "Yoooosssssiiiiiii");
	newName = Group_GetName(group);
	if (strcmp("Yoooosssssiiiiiii" , newName)) {
		return 0;
	}
	Group_Destroy(&group);
	return 1;
}
