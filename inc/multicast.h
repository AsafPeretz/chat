#ifndef MULTICAST_H
#define MULTICAST_H

typedef enum Multicast_Result {
	Multicast_Success = 0,
	Multicast_Port_Error,
	Multicast_Address_Error,
	Multicast_File_Not_Exists_Error,
	Multicast_Unknown_Error
}Multicast_Result;


/**
 * @brief Function dynamically creates two new processes for the chat.
 * 				First window, is a multicast window. In that window, many clients can communicate.
 * 				The second window is the "writing box", that every client has his own process. Everything thats been wrrten in the "writing box"
 * 				showed up in the multicast window.
 *
 * @param[in] a_groupAddress - Group ip address.
 * @param[in] a_groupPort - Group port.
 * @param[in] a_groupName - Group name.
 * @param[in] a_userName - the user name asks to to join the chat.
 *
 * @return success or error code
 * @return Multicast_Success
 * @return Multicast_File_Not_Exists_Error
 * @return Multicate_Port_Error
 * @return Multicast_Unknown_Error
**/
Multicast_Result Multicast_Create(char *a_groupAddress, int a_groupPort, char *a_groupName, char *a_userName);


/**
 * @brief Function dynamically close ("kill") the client's chat processes.
 *
 * @param[in] a_groupName - Group name.
 * @param[in] a_userName - the user name asks to to close the chat.
 *
 * @return success or error code
 * @return Multicast_Success
 * @return Multicast_Address_Error
 * @return Multicast_Unknown_Error
**/
Multicast_Result Multicast_Destroy(char *a_groupName, char *a_userName);


#endif /* MULTICAST_H */
