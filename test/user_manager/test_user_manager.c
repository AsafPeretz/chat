#include <stdio.h>
#include <string.h>
#include "user_manager.h"

#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define RESET "\x1B[0m"

#define PRINT_TEST(a_testResult, a_testName) if (!a_testResult) { printf("%s\n", a_testName); }

#define CAPACITY 10
#define FILE_NAME "users_list.txt"

static int Test_Create(char* a_testName);
static int Test_Create_Null(char* a_testName);
static int Test_Create_Size_0(char* a_testName);
static int Test_Create_File_Not_Exists(char* a_testName);
static int Test_Destory(char* a_testName);
static int Test_Destory_Double_Free(char* a_testName);
static int Test_Register(char* a_testName);
static int Test_Register_Uninitizlized(char* a_testName);
static int Test_Register_Duplicate(char* a_testName);
static int Test_Register_Overflow(char* a_testName);
static int Test_Login(char* a_testName);
static int Test_Login_Wrong_User(char* a_testName);
static int Test_Login_Wrong_Password(char* a_testName);
static int Test_Logout(char* a_testName);
static int Test_Logout_Uninitialized(char* a_testName);
static int Test_Logout_User_Already_Logout(char* a_testName);
static int Test_Logout_User_Not_Found(char* a_testName);

static void clean_file(const char *a_fileName);

int main(void) {
	char testName[100];
	PRINT_TEST(Test_Create(testName), testName);
	PRINT_TEST(Test_Create_Null(testName), testName);
	PRINT_TEST(Test_Create_Size_0(testName), testName);
	PRINT_TEST(Test_Create_File_Not_Exists(testName), testName);
	PRINT_TEST(Test_Destory(testName), testName);
	PRINT_TEST(Test_Destory_Double_Free(testName), testName);
	PRINT_TEST(Test_Register(testName), testName);
	PRINT_TEST(Test_Register_Uninitizlized(testName), testName);
	PRINT_TEST(Test_Register_Duplicate(testName), testName);
	PRINT_TEST(Test_Register_Overflow(testName), testName);
	PRINT_TEST(Test_Login(testName), testName);
	PRINT_TEST(Test_Login_Wrong_User(testName), testName);
	PRINT_TEST(Test_Login_Wrong_Password(testName), testName);
	PRINT_TEST(Test_Logout(testName), testName);
	PRINT_TEST(Test_Logout_Uninitialized(testName), testName);
	PRINT_TEST(Test_Logout_User_Already_Logout(testName), testName);
	PRINT_TEST(Test_Logout_User_Not_Found(testName), testName);
	return 0;
}

static int Test_Create(char* a_testName) {
	UserManager* userManager = UserManager_Create(FILE_NAME, CAPACITY);
	strcpy(a_testName, "Test_Create");
	if (NULL == userManager) {
		UserManager_Destroy(&userManager);
		return 0;
	}
	UserManager_Destroy(&userManager);
	return 1;
}


static int Test_Create_Null(char* a_testName) {
	UserManager* userManager = UserManager_Create(NULL, CAPACITY);
	strcpy(a_testName, "Test_Create_Null");
	if (NULL != userManager) {
		UserManager_Destroy(&userManager);
		return 0;
	}
	return 1;
}

static int Test_Create_Size_0(char* a_testName) {
	UserManager* userManager = UserManager_Create(FILE_NAME, 0);
	strcpy(a_testName, "Test_Create_Size_0");
	if (NULL != userManager) {
		UserManager_Destroy(&userManager);
		return 0;
	}
	return 1;
}

static int Test_Create_File_Not_Exists(char* a_testName) {
	UserManager* userManager = UserManager_Create("file_not_exists.txt", 0);
	strcpy(a_testName, "Test_Create_File_Not_Exists");
	if (NULL != userManager) {
		UserManager_Destroy(&userManager);
		return 0;
	}
	return 1;
}

static int Test_Destory(char* a_testName) {
	UserManager* userManager = UserManager_Create(FILE_NAME, CAPACITY);
	strcpy(a_testName, "Test_Destory");
	UserManager_Destroy(&userManager);
	if (NULL != userManager) {
		UserManager_Destroy(&userManager);
		return 0;
	}
	return 1;
}

static int Test_Destory_Double_Free(char* a_testName) {
	UserManager* userManager = UserManager_Create(FILE_NAME, CAPACITY);
	strcpy(a_testName, "Test_Destory_Double_Free");
	UserManager_Destroy(&userManager);
	UserManager_Destroy(&userManager);
	if (NULL != userManager) {
		UserManager_Destroy(&userManager);
		return 0;
	}
	return 1;
}

static int Test_Register(char* a_testName) {
	UserManager* userManager = UserManager_Create(FILE_NAME, CAPACITY);
	strcpy(a_testName, "Test_Register");
	if (UserManager_Success != UserManager_Register(userManager, "username", "password")) {
		UserManager_Destroy(&userManager);
		return 0;
	}
	if (UserManager_Success != UserManager_Register(userManager, "USERNAME", "PASSWORD")) {
		UserManager_Destroy(&userManager);
		return 0;
	}
	UserManager_Destroy(&userManager);
	clean_file(FILE_NAME);
	return 1;
}

static int Test_Register_Uninitizlized(char* a_testName) {
	UserManager* userManager = UserManager_Create(FILE_NAME, CAPACITY);
	strcpy(a_testName, "Test_Register_Uninitizlized");
	if (UserManager_Uninitialized_Error != UserManager_Register(NULL, "username", "password")) {
		UserManager_Destroy(&userManager);
		return 0;
	}
	if (UserManager_Uninitialized_Error != UserManager_Register(userManager, NULL, "password")) {
		UserManager_Destroy(&userManager);
		return 0;
	}

	if (UserManager_Uninitialized_Error != UserManager_Register(userManager, "username", NULL)) {
		UserManager_Destroy(&userManager);
		return 0;
	}
	UserManager_Destroy(&userManager);
	clean_file(FILE_NAME);
	return 1;
}

static int Test_Register_Duplicate(char* a_testName) {
	UserManager *userManager = UserManager_Create(FILE_NAME, CAPACITY);
	strcpy(a_testName, "Test_Register_Duplicate");
	if (UserManager_Success != UserManager_Register(userManager, "username", "password")) {
		UserManager_Destroy(&userManager);
		return 0;
	}
	if (UserManager_User_Already_Exists_Error != UserManager_Register(userManager, "username", "password")) {
		UserManager_Destroy(&userManager);
		return 0;
	}
	UserManager_Destroy(&userManager);
	clean_file(FILE_NAME);
	return 1;
}

static int Test_Register_Overflow(char* a_testName) {
	UserManager *userManager = UserManager_Create(FILE_NAME, 1);
	strcpy(a_testName, "Test_Register_Overflow");
	if (UserManager_Success != UserManager_Register(userManager, "username", "password")) {
		UserManager_Destroy(&userManager);
		return 0;
	}
	if (UserManager_Overflow != UserManager_Register(userManager, "username2", "password2")) {
		UserManager_Destroy(&userManager);
		return 0;
	}
	UserManager_Destroy(&userManager);
	clean_file(FILE_NAME);
	return 1;
}

static int Test_Login(char* a_testName) {
	UserManager *userManager = UserManager_Create(FILE_NAME, CAPACITY);
	strcpy(a_testName, "Test_Register_Overflow");
	UserManager_Register(userManager, "username", "password");
	if (UserManager_Success != UserManager_Login(userManager,  "username", "password")) {
		UserManager_Destroy(&userManager);
		return 0;
	}
	UserManager_Destroy(&userManager);
	clean_file(FILE_NAME);
	return 1;
}

static int Test_Login_Wrong_User(char* a_testName) {
	UserManager *userManager = UserManager_Create(FILE_NAME, CAPACITY);
	strcpy(a_testName, "Test_Login_Wrong_User");
	if (UserManager_User_Not_Found_Error != UserManager_Login(userManager,  "username", "password")) {
		UserManager_Destroy(&userManager);
		return 0;
	}
	UserManager_Destroy(&userManager);
	clean_file(FILE_NAME);
	return 1;
}

static int Test_Login_Wrong_Password(char* a_testName) {
	UserManager *userManager = UserManager_Create(FILE_NAME, CAPACITY);
	strcpy(a_testName, "Test_Login_Wrong_Password");
	if (UserManager_Success != UserManager_Register(userManager, "username", "password")) {
		UserManager_Destroy(&userManager);
		return 0;
	}
	if (UserManager_Wrong_Password != UserManager_Login(userManager,  "username", "124")) {
		UserManager_Destroy(&userManager);
		return 0;
	}
	UserManager_Destroy(&userManager);
	clean_file(FILE_NAME);
	return 1;
}

static int Test_Logout(char* a_testName) {
	UserManager *userManager = UserManager_Create(FILE_NAME, CAPACITY);
	strcpy(a_testName, "Test_Logout");
	UserManager_Register(userManager, "username", "password");
	UserManager_Login(userManager,  "username", "password");
	if (UserManager_Success != UserManager_Logout(userManager, "username")) {
		UserManager_Destroy(&userManager);
		return 0;
	}
	UserManager_Destroy(&userManager);
	clean_file(FILE_NAME);
	return 1;
}

static int Test_Logout_Uninitialized(char* a_testName) {
	UserManager *userManager = UserManager_Create(FILE_NAME, CAPACITY);
	strcpy(a_testName, "Test_Logout_Uninitialized");
	UserManager_Register(userManager, "username", "password");
	UserManager_Login(userManager,  "username", "password");
	if (UserManager_Uninitialized_Error != UserManager_Logout(userManager, NULL)) {
		UserManager_Destroy(&userManager);
		return 0;
	}
	if (UserManager_Uninitialized_Error != UserManager_Logout(NULL, "username")) {
		UserManager_Destroy(&userManager);
		return 0;
	}
	UserManager_Destroy(&userManager);
	clean_file(FILE_NAME);
	return 1;
}

static int Test_Logout_User_Already_Logout(char* a_testName) {
	UserManager *userManager = UserManager_Create(FILE_NAME, CAPACITY);
	strcpy(a_testName, "Test_Logout_Not_Found");
	UserManager_Register(userManager, "username", "password");
	if (UserManager_User_Already_Logout_Error != UserManager_Logout(userManager, "username")) {
		UserManager_Destroy(&userManager);
		return 0;
	}
	UserManager_Login(userManager, "username", "password");
	if (UserManager_Success != UserManager_Logout(userManager, "username")) {
		UserManager_Destroy(&userManager);
		return 0;
	}
	if (UserManager_User_Already_Logout_Error != UserManager_Logout(userManager, "username")) {
		UserManager_Destroy(&userManager);
		return 0;
	}
	UserManager_Destroy(&userManager);
	clean_file(FILE_NAME);
	return 1;
}

static int Test_Logout_User_Not_Found(char* a_testName) {
	UserManager *userManager = UserManager_Create(FILE_NAME, CAPACITY);
	strcpy(a_testName, "Test_Logout_Not_Found");
	if (UserManager_User_Not_Found_Error != UserManager_Logout(userManager, "username")) {
		UserManager_Destroy(&userManager);
		return 0;
	}
	UserManager_Destroy(&userManager);
	clean_file(FILE_NAME);
	return 1;
}

static void clean_file(const char *a_fileName) {
	FILE* fileHandler = fopen(a_fileName, "w");
	fclose(fileHandler);
}
