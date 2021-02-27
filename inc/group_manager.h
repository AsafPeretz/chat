#ifndef GROUP_MANAGER_H
#define GROUP_MANAGER_H

#include <sys/types.h> /* size_t */

typedef struct GroupManager GroupManager;

typedef enum GroupManager_Result {
	GroupManager_Success = 0 ,
	GroupManager_Uninitialized_Error,
	GroupManager_Allocation_Error,
	GroupManager_GroupAlreadyExists_Error,
	GroupManager_GroupNotExists_Error,
	GroupManager_Underflow_Error,
	GroupManager_Overflow_Error,
	GroupManager_Unknown_Error
} GroupManager_Result;


/**
 * @brief Function dynamically creates a new GroupManager ADT of a given expected amount of groups.
 *
 * @param[in] a_numOfGroups - amount of groups.
 *
 * @return GroupManager* on success / NULL on fail
**/
GroupManager* GroupManager_Create(size_t a_numOfGroups);


/**
 * @brief Function Dynamically deallocates a previously allocated GroupManager.
 *
 * @params[in] a_groupManager - a pointer to previously created GroupManager returned via GroupManager_Create.
 *
 * @return (void).
**/
void GroupManager_Destroy(GroupManager **a_groupManager);


/**
 * @brief Function open a new Group.
 *
 * @param[in] a_groupManager
 * @param[in] a_groupName - a name for the new group.
 *
 * @return a_address - the address of the new group.
 * @return a_port - the port of the new group.
 * @return success or error code
 * @return GroupManager_Success
 * @return GroupManager_Uninitialized_Error
 * @return GroupManager_Allocation_Error
 * @return GroupManager_Overflow_Error
 * @return GroupManager_Duplicate_Error
**/
GroupManager_Result GroupManager_OpenGroup(GroupManager *a_groupManager, char *a_groupName, char *a_address, char *a_port);


/**
 * @brief Function add a user to exist group. 
 *
 * @param[in] a_groupManager
 * @param[in] a_groupName - name of the group to be increased.
 *
 * 
 * @return a_address - Group address
 * @return a_port - Group port
 * @return success or error code
 * @return GroupManager_Success
 * @return GroupManager_Uninitialized_Error
 * @return GroupManager_Group_Not_Exist_Error
**/
GroupManager_Result GroupManager_AddUser(GroupManager *a_groupManager, char *a_groupName, char *a_address, char *a_port);


/**
 * @brief Function remove a user from exist Group, 
 *
 * @param[in] a_groupManager
 * @param[in] a_groupName - name of the Group to be decreased.
 *
 * @return success or error code
 * @return GroupManager_Success
 * @return GroupManager_Uninitialized_Error
 * @return GroupManager_Group_Not_Exist_Error
**/
GroupManager_Result GroupManager_RemoveUser(GroupManager *a_groupManager, char *a_groupName);


/**
 * @brief Function return all the Groups names in a given string (a_groupNames).
 * 				The first byte in the string equal to the number of groups, and then all the groups names(with '\0' between them). 
 *
 * @param[in] a_groupManager
 *
 * @return a_groupsNames - The Groups names stream.
**/
void GroupManager_GetAllGroups(GroupManager *a_groupManager, char *a_groupsNames);


/**
 * @brief Return the actual number of groups currently in the GroupManager.
 *
 * @param[in] a_groupManager
 *
 * @return number of Groups on success, 0 if GroupManager is empty or invalid
**/
size_t GroupManager_NumOfGroups(const GroupManager *a_groupManager);


/**
 * @brief Return the actual number of groups currently in the GroupManager.
 *
 * @param[in] a_groupManager
 * @param[in] a_groupName - Group name to check.
 *
 * @return number of users in the Group.
**/
size_t GroupManager_UsersInGroup(const GroupManager *a_groupManager, char *a_groupName);


#endif /* GROUP_MANAGER_H */
