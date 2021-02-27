#include <string.h>				/* strcmp, strlen, strcpy */
#include <stdlib.h>				/* malloc, free */
#include <stdio.h> 				/* files */
#include "user_manager.h"
#include "hashmap.h"


#define MAX_NAME_SIZE 30
#define MAX_PASSWORD_SIZE 30


struct UserManager {
	char *m_usersFile;	/* file who contains line of name and line of password repetitively */
	HashMap *m_users; 	/* key = user name, value = User */
};

typedef struct User {
	char m_name[MAX_NAME_SIZE];
	char m_password[MAX_PASSWORD_SIZE];
	int m_isLogIn;
}User;


static size_t hashFuncName(const void *a_key);
static int passwordEqualityFunc(const void *a_savedUser, const void *a_password);
static void userDestroy(void *a_user);
static User* userCreate(char *a_name, char *a_password);
static int stringToNumber(const char *a_str);
static int loadUsersFromFile(UserManager *a_userManager, FILE *a_fileHandler);
static UserManager* userManagerAllocate(const char *a_fileUsers, size_t a_capacity);
static int saveUser(const void *a_key, void *a_value, void *a_context);
static int saveUsersInFile(UserManager *a_userManager);


UserManager* UserManager_Create(const char *a_fileUsers, size_t a_capacity) {
	FILE *fileHandler = NULL;
	UserManager *userManager = NULL;
	if (NULL == a_fileUsers || 0 == a_capacity) {
		return NULL;
	}
	userManager = userManagerAllocate(a_fileUsers, a_capacity);
	if (NULL == userManager) {
		return NULL;
	}
	fileHandler = fopen(userManager->m_usersFile, "r");
	if (NULL == fileHandler) {
		UserManager_Destroy(&userManager);
		return NULL;
	}
	if (0 == loadUsersFromFile(userManager, fileHandler)) {
		fclose(fileHandler);
		UserManager_Destroy(&userManager);
		return NULL;
	}
	fclose(fileHandler);
	return userManager;
}


UserManager_Result UserManager_Logout(UserManager *a_userManager, char *a_name) {
	Map_Result status;
	void *user = NULL;
	if (NULL == a_userManager || NULL == a_name) {
		return UserManager_Uninitialized_Error;
	}
	status = HashMap_Find(a_userManager->m_users, a_name, &user);
	if (Map_Success == status) {
		if (0 == ((User*)user)->m_isLogIn) {
			return UserManager_UserAlreadyLogout_Error;
		}
		((User*)user)->m_isLogIn = 0;
		return UserManager_Success;
	}
	else {
		switch (status) {
			case Map_KeyNotFound_Error:
				return UserManager_UserNotFound_Error;

			default:
				return UserManager_Unknown_Error;
		}
	}
	return UserManager_Unknown_Error;	
}


UserManager_Result UserManager_Register(UserManager *a_userManager, char *a_name, char *a_password) {
	User *user = NULL;
	Map_Result status;
	if (NULL == a_userManager || NULL == a_name || NULL == a_password) {
		return UserManager_Uninitialized_Error;
	}
	user = userCreate(a_name, a_password);
	if (NULL == user) {
		return UserManager_Allocation_Error;
	}
	status = HashMap_Insert(a_userManager->m_users, user->m_name, user);
	if (Map_Success != status) {
		free(user);
		switch (status) {
			case Map_KeyDuplicate_Error:
				return UserManager_UserAlreadyExists_Error;

			case Map_Allocation_Error:
				return UserManager_Allocation_Error;

			case Map_Overflow_Error:
				return UserManager_Overflow_Error;

			default:
				return UserManager_Unknown_Error;
		}
	}
	return UserManager_Success;	
}


UserManager_Result UserManager_Login(UserManager* a_userManager, char *a_name, char *a_password) {
	Map_Result status;
	void *user;
	if (NULL == a_userManager || NULL == a_name || NULL == a_password) {
		return UserManager_Uninitialized_Error;
	}
	status = HashMap_Find(a_userManager->m_users, a_name, &user);
	if (Map_Success == status) {
		if (1 == ((User*)user)->m_isLogIn) {
			return UserManager_UserAlreadyLogin_Error;
		}
		if (!strcmp(((User*)user)->m_password, a_password)) {
			((User*)user)->m_isLogIn = 1;	
			return UserManager_Success;
		}
		else {
			return UserManager_WrongPassword_Error;
		}
	}
	else {
		switch (status) {
			case Map_KeyNotFound_Error:
				return UserManager_UserNotFound_Error;
			default:
				return UserManager_Unknown_Error;
		}
	}

	return UserManager_Unknown_Error;	
}


static User* userCreate(char* _name, char* _password) {
	User* user = (User*)malloc(sizeof(User));
	if(NULL == user)
	{
		return NULL;	
	}
	
	strcpy(user->m_name, _name);
	strcpy(user->m_password, _password);
	user->m_isLogIn = 0;

	return user;
}


void UserManager_Destroy(UserManager **a_userManager) {
	if(NULL == a_userManager || NULL == *a_userManager) {
		return;
	}
	if (0 == saveUsersInFile(*a_userManager)) {
		return;
	}
	free((*a_userManager)->m_usersFile);
	HashMap_Destroy(&((*a_userManager)->m_users), NULL, userDestroy);
	free(*a_userManager);
	*a_userManager = NULL;
}


static int saveUsersInFile(UserManager *a_userManager) {
	FILE *fileHandler = NULL;
	fileHandler = fopen(a_userManager->m_usersFile, "r+");
	if (NULL == fileHandler) {
		return 0;
	}
	HashMap_ForEach(a_userManager->m_users, saveUser, fileHandler);
	fclose(fileHandler);
	return 1;
}


static int saveUser(const void *a_key, void *a_value, void *a_context) {
	FILE *fileHandler = (FILE*)a_context;
	User *user = (User*)a_value;
	fprintf(fileHandler,"%s\n", user->m_name);
	fprintf(fileHandler,"%s\n", user->m_password);
	return 1;
}


void userDestroy(void *a_user) {
	free(a_user);
}


size_t hashFuncName(const void* _key) {
	int returnVal = stringToNumber((char*)_key);
	return returnVal;
}


static int stringToNumber(const char* _str) {
	int count = 0;
	int i = 0;
	while ('\0' != _str[i]) {
		count += (int)_str[i];
		++i;
	}
	return count;
}


int passwordEqualityFunc(const void* _savedUser, const void* _name) {
	return !strcmp((char*)_savedUser, (char*)_name);
}


static UserManager* userManagerAllocate(const char *a_fileUsers, size_t a_capacity) {
	size_t fileNameLen;
	UserManager *userManager = (UserManager*)malloc(sizeof(UserManager));
	if (NULL == userManager) {
		return NULL;
	}
	userManager->m_users = HashMap_Create(a_capacity, hashFuncName, passwordEqualityFunc);
	if (NULL == userManager->m_users) {
		free(userManager);
		return NULL;	
	}
	fileNameLen = strlen(a_fileUsers) + 1;
	userManager->m_usersFile = (char*)malloc(fileNameLen * sizeof(char));
	if (NULL == userManager->m_usersFile) {
			free(userManager->m_users);
			free(userManager);
			return NULL;	
	}
	memcpy(userManager->m_usersFile, a_fileUsers, fileNameLen);
	return userManager;
}


static int loadUsersFromFile(UserManager *a_userManager, FILE *a_fileHandler) {
	User *newUser = NULL;
	size_t len;
	while (!feof(a_fileHandler)) { /*TODO - ruing one more time then it should*/
		newUser = (User*)malloc(sizeof(User));
		if (NULL == newUser) {
			return 0;	
		}
		if (NULL == fgets(newUser->m_name, MAX_NAME_SIZE, a_fileHandler)) {
			free(newUser);
			break;
		}
		len = strlen(newUser->m_name);
		newUser->m_name[len - 1] = '\0';
		if (NULL == fgets(newUser->m_password, MAX_PASSWORD_SIZE, a_fileHandler)) {
			free(newUser);
			break;
		}
		len = strlen(newUser->m_password);
		newUser->m_password[len - 1] = '\0';
		newUser->m_isLogIn = 0;
		if (Map_Success != HashMap_Insert(a_userManager->m_users, newUser->m_name, newUser)) {
			free(newUser);
			return 0;	
		}
	}
	return 1;
}
