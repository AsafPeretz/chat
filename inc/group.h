#ifndef GROUP_H
#define GROUP_H

#include <sys/types.h> /* size_t */

#define ADRRESS_SIZE 25

typedef struct Group Group;

/**
 * @brief Function dynamically creates a new Group ADT of a given group name and address.
 *
 * @param[in] a_groupName - Group name.
 * @param[in] a_adrress - Group IP Address.
 *
 * @return Group* on success / NULL on fail
**/
Group* Group_Create(char *a_groupName, char *a_address, char *a_port);


/**
 * @brief Function Dynamically deallocates a previously allocated Group.
 *
 * @params[in] a_group - a pointer to previously created Group returned via Group_Create
 *
 * @return char* - the Group address.
**/
char* Group_Destroy(Group **a_group);


/**
 * @brief Function Increase the number of users in a group.
 *
 * @param[in] a_group
**/
void Group_AddUser(Group *a_group);


/**
 * @brief Function Decrease the number of users in a group.
 *
 * @param[in] a_group
**/
void Group_RemoveUser(Group *a_group);


/**
 * @brief Function Returns the actual number of users currently in the Group.
 *
 * @param[in]	a_group
 *
 * @return number of users on success, 0 if Group is empty or invalid
**/
size_t Group_Size(const Group *a_group);


/**
 * @brief Function Returns the name of the group.
 *
 * @param[in] a_group
 *
 * @return Group name
**/
const char* Group_GetName(const Group *a_group);


/**
 * @brief Function Changes the Group name.
 *
 * @param[in]	a_group
 * @param[in]	a_newName - a new name for the group.
**/
void Group_ChangeName(Group *a_group, const char *a_newName);


/**
 * @brief Function gets the groups address.
 *
 * @param[in]	a_group
 * 
 * @return Group address
**/
const char* Group_GetAddress(const Group *a_group);


/**
 * @brief Function gets the groups port.
 *
 * @param[in]	a_group
 * 
 * @return Group port
**/
const char* Group_GetPort(const Group *a_group);


#endif /* GROUP_H */
