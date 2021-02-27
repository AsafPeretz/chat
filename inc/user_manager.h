#ifndef USER_MANAGER_H
#define USER_MANAGER_H

typedef struct UserManager UserManager;

typedef enum UserManager_Result {
	UserManager_Success = 0,
	UserManager_UserNotFound_Error,
	UserManager_Allocation_Error,
	UserManager_Uninitialized_Error,
	UserManager_UserAlreadyExists_Error,
	UserManager_UserAlreadyLogin_Error,
	UserManager_UserAlreadyLogout_Error,
	UserManager_FileNotOpen_Error,
	UserManager_Overflow_Error,
	UserManager_WrongPassword_Error,
	UserManager_Unknown_Error
}UserManager_Result;


/**
 * @brief Function dynamically creates a new UserManager ADT of a given file name.
 *
 * @param[in] a_users - a file that contains all the registered users (username and password).
 * @param[in] a_capacity - the capacity of registered users.
 *
 * @return UserManager* on success / NULL on fail
**/
UserManager* UserManager_Create(const char *a_fileUsers, size_t a_capacity);


/**
 * @brief Dynamically deallocates a previously allocated UserManager, and puts a NULL in the UserManager pointer.
 *
 * @params[in] a_userManager - a pointer to previously created UserManager returned via UserManager_Create
**/
void UserManager_Destroy(UserManager **a_userManager);


/**
 * @brief The function register and save a new user in the UserManager.
 *
 * @param[in] a_userManager
 * @param[in] a_name - a name for the new user.
 * @param[in] a_password - a password for the new user.
 *
 * @return success or error code
 * @return UserManager_Success
 * @return UserManager_Uninitialized_Error
 * @return UserManager_User_Already_Exists_Error
**/
UserManager_Result UserManager_Register(UserManager *a_userManager, char *a_name, char *a_password);


/**
 * @brief The function change user mode to login.
 *
 * @param[in] a_userManager
 * @param[in] a_name - a user name to login.
 * @param[in] a_password - a user password to login.
 *
 * @return success or error code
 * @return UserManager_Success
 * @return UserManager_Uninitialized_Error
 * @return UserManager_User_Already_Exists_Error
**/
UserManager_Result UserManager_Login(UserManager *UserManager_Logout, char *a_name, char *a_password);


/**
 * @brief The function change user mode to logout.
 *
 * @param[in] a_userManager
 * @param[in] a_name - a user name to logout.
 *
 * @return success or error code
 * @return UserManager_Success
 * @return UserManager_Uninitialized_Error
 * @return UserManager_User_Already_Logout_Error
 * @return UserManager_User_Not_Found_Error
**/
UserManager_Result UserManager_Logout(UserManager *a_userManager, char *a_name);

#endif /* USER_MANAGER_H */
