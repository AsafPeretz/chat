#ifndef PROTOCOL_H
#define PROTOCOL_H

#define MAX_BUFFER_SIZE 4000
#define USERNAME_SIZE 30
#define PASSWORD_SIZE 30
#define GROUPNAME_SIZE 30
#define IP_ADDRESS_SIZE 16
#define PORT_SIZE 8


typedef enum Protocol_Result {
	Protocol_Success = 0,
	Protocol_Uninitialized_Error,
	Protocol_Unknown_Error
}Protocol_Result;


typedef enum Tag {
/*     Request Tags     */

	Protocol_Request_Register = 0, /* Request_LogIn struct */
	Protocol_Request_LogIn,		 		 /* Request_LogIn struct */

	Protocol_Request_LogOut, 			 /* Request_Logout struct */

	Protocol_Request_GroupOpen,		 /* Request_Group struct */
	Protocol_Request_GroupJoin,		 /* Request_Group struct */
	Protocol_Request_GroupLeave,	 /* Request_Group struct */

	Protocol_Request_GroupsNames,	 /* Request_GroupsNames */


/*     Response Tags     */

	Protocol_Response_Register,		 /* Response_LogInOut struct */
	Protocol_Response_LogIn,			 /* Response_LogInOut struct */
	Protocol_Response_LogOut,			 /* Response_LogInOut struct */

	Protocol_Response_GroupOpen,	 /* Response_Group struct */
	Protocol_Response_GroupJoin,	 /* Response_Group struct */

	Protocol_Response_GroupsNames, /* Response_GroupNames struct */

	Protocol_Response_LeaveGroup	 /* Response_LeaveGroup struct */

}Tag;




/* ---------- Request Structs ---------- */


/* Struct for Register and LogIn operations */
typedef struct Request_LogIn {
	char m_tag;
	char m_name[USERNAME_SIZE];
	char m_password[PASSWORD_SIZE];
}Request_LogIn;


/* Struct for Logout operation */
typedef struct Request_Logout {
	char m_tag;
	char m_name[USERNAME_SIZE];
	char m_groupsNames[MAX_BUFFER_SIZE];
}Request_Logout;


/* Struct for Open Group, Join Group and Leave Group operations */
typedef struct Request_Group {
	char m_tag;
	char m_groupName[GROUPNAME_SIZE];
}Request_Group;


/* Struct for Groups Names Request (before Join Group operation)*/
typedef struct Request_GroupsNames {
	char m_tag;
}Request_GroupsNames;





/* ---------- Response Structs ---------- */

/* Struct for Register, Login and Logout operations */
typedef struct Response_LogInOut {
	char m_tag;
	char m_status; /* accept - 1 , failed - 0 */
}Response_LogInOut;


/* Struct for Open Group and Join Group operations */
typedef struct Response_Group {
	char m_tag;
	char m_status; /* accept - 1 , failed - 0 */
	char m_ipAddress[IP_ADDRESS_SIZE];
	char m_port[PORT_SIZE];
	char m_groupName[GROUPNAME_SIZE];
}Response_Group;


/* Struct for show all Groups names operation */
typedef struct Response_GroupNames {
	char m_tag;
	char m_allGroups[MAX_BUFFER_SIZE];
}Response_GroupNames;


/* Struct for Leave Group operations */
typedef struct Response_LeaveGroup {
	char m_tag;
	char m_status; /* accept - 1 , failed - 0 */
	char m_groupName[GROUPNAME_SIZE];
}Response_LeaveGroup;



/**
 * @brief The function returns the tag of a spesific package.
 *
 * @param[in] a_pack - a pack to be checked by tag.
 *
 * @return a_tag - tag from the enum tag list.
 * @return success or error code
 * @return Protocol_Success
 * @return Protocol_Uninitialized
**/
Protocol_Result Protocol_CheckTag(const char* a_pack, char *a_tag);


/**
 * @brief The function packing the a_data structure and put it in the a_pack.
 *
 * @param[in] a_data - one of the Protocol struct according the choosen tag - filled with the rellevant data.
 *
 * @return a_pack - empty buffer to be detailed.
 * @return a_size - the package (a_pack) size [Bytes].
 * @return success or error code
 * @return Protocol_Success
 * @return Protocol_Uninitialized
**/
Protocol_Result Protocol_Pack(const void *a_data, char *a_pack, size_t *a_size);


/**
 * @brief The function unpacking a pack that packed in Protocol_Pack.
 *
 * @param[in] a_pack - a pack to be unpacked.
 *
 * @return success or error code
 * @return Protocol_Success
 * @return Protocol_Uninitialized
 * @return a_msg - one of the Protocol structs list.
**/
Protocol_Result Protocol_Unpack(const char *a_pack, void *a_msg);


#endif /* PROTOCOL_H */
