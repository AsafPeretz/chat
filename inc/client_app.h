#ifndef CLIENT_APP_H
#define CLIENT_APP_H

typedef struct ClientApp ClientApp;

typedef enum ClientApp_Result {
	ClientApp_Success,
	ClientApp_Connect_Error,
	ClientApp_Uninitialized_Error,
	ClientApp_Allocation_Error,
	ClientApp_Unknown_Error
}ClientApp_Result;


/**
 * @brief Function dynamically creates a new ClientPP ADT.
 *
 * @return ClientNet* on success / NULL on fail
**/
ClientApp* Client_Create();


/**
 * @brief Function Dynamically deallocates a previously allocated ClientApp, and puts a NULL in the ClientApp pointer.
 *
 * @params[in] a_client - a pointer to previously created ClientApp returned via ClientApp_Create
**/
void Client_Destroy(ClientApp **a_client);


/**
 * @brief Function starts running the client that created by ClientApp_Create function.
 *  		  The clientApp is used to communicate with the server.
 * 				The clientApp manages the client requests for register, login, groups, etc.
 *
 * @param[in] a_client - a client to be run.
 *
 * @return success or error code
 * @return ClientApp_Success
 * @return ClientApp_Uninitialized_Error
 * @return ClientApp_Allocation_Error
 * @return ClientApp_Connect_Error
**/
ClientApp_Result Client_Run(ClientApp *a_client);


#endif /* CLIENT_APP_H */
