#ifndef SERVER_APP_H
#define SERVER_APP_H

#include "group_manager.h"
#include "user_manager.h"
#include "server_net.h"
#include "protocol.h"

typedef struct ServerApp ServerApp;

typedef enum ServerApp_Result {
	ServerApp_Success = 0,
	ServerApp_Uninitialized_Error,
	ServerApp_Allocation_Error,
	ServerApp_Network_Error,
	ServerApp_General_Error,
	ServerApp_Unknown_Error
}ServerApp_Result;

/**
 * @brief Function dynamically creates a new ServerApp ADT.
 * 
 * @param[in] a_fileUsers - the server load the registed users
 * @param[in] a_capacity - the server uses Hash Table and. the capacity for the table.
 *
 * @return ServerApp* on success / NULL on fail
**/
ServerApp* ServerApp_Create(char *a_fileUsers, size_t a_capacity);


/**
 * @brief Function Dynamically deallocates a previously allocated ServerApp, and puts a NULL in the ServerApp pointer.
 *
 * @params[in] a_server - a pointer to previously created ServerApp returned via ServerApp_Create
**/
void ServerApp_Destroy(ServerApp **a_server);


/**
 * @brief Function starts running the server that created by ServerApp_Create function.
 *  		  The server is used as a data base for the chat application.
 * 				The server stores and manages Groups and Users.
 *
 * @param[in] a_server - a server to be run.
 *
 * @return success or error code
 * @return ServerApp_Success
 * @return ServerApp_Uninitialized
 * @return ServerApp_AllocationFailed
**/
ServerApp_Result ServerApp_Run(ServerApp *a_server);


#endif /* SERVER_APP_H */
