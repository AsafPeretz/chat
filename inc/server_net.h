#ifndef SERVER_NET_H
#define SERVER_NET_H

#include <sys/types.h> /* size_t */

typedef struct ServerNet ServerNet;

typedef enum ServerNet_Result {
	ServerNet_Success = 0,
	ServerNet_Accept_Error,
	ServerNet_SendMessage_Error,
	ServerNet_Select_Error,
	ServerNet_Allocation_Error,
	ServerNet_Unkown_Error
} ServerNet_Result;

/**
 * @brief Function pointers. The ServerNet will use those function as follows:
 *
 * newClientFunc - function to act when new client is connected.
 * newMessageFunc - function to act when new message is received.
 * closeClientFunc - function to act when one of the clients closed his socket.
 * messageFailedFunc - function to act when the server is failed to receive a message from one of his clients.
 */
typedef void (*newClientFunc)(int client_sock, void *context);
typedef void (*newMessageFunc)(int client_sock, void *message_buffer, void *context);
typedef void (*closeClientFunc)(int client_sock, void *context);
typedef void (*messageFailedFunc)(int client_sock, void *context);


/**
 * @brief Function dynamically creates a new ServerNet ADT of a given function pointers.
 *
 * @param[in] newClientFunc
 * @param[in] newMessageFunc
 * @param[in] closeClientFunc
 * @param[in] messageFailedFunc
 *
 * @return ServerNet* on success / NULL on fail
**/
ServerNet* ServerNet_Create(newClientFunc a_newClient, newMessageFunc a_newMessage, closeClientFunc a_closeClient, messageFailedFunc a_messageFailed);


/**
 * @brief Function dynamically deallocates a previously allocated ServerNet, and puts a NULL in the ServerNet pointer.
 *
 * @params[in] a_server - a pointer to previously created ServerNet returned via ServerNet_Create
**/
void ServerNet_Destroy(ServerNet **a_server);


/**
 * @brief Function starts running the server that created by ServerNet_create function.
 *  		  The server wait for new clients or for message from exist clients, and respond with the appropriate function.
 *
 * @param[in] a_server - a server to be run.
 * @param[in] a_context - parameter to be send to functions "newClientFunc", "newMessageFunc", "closeClientFunc" and "messageFailedFunc".
 *
 * @return success or error code
 * @return ServerNet_Success
 * @return ServerNet_SelectFunctionFailed
 * @return ServerNet_AllocationFailed
**/
ServerNet_Result ServerNet_Run(ServerNet *a_server, void *a_context);


/**
 * @brief Function sends a message to the last client who's send a message to the server.
 *
 * @param[in] a_sockClient - a client socket to send the message.
 * @param[in] a_message - a message to be send.
 * @param[in] a_messageSize - the size of the message.
 *
 * @return success or error code
 * @return ServerNet_Success
 * @return ServerNet_SendMessageFailed
**/
ServerNet_Result ServerNet_Send(int a_sockClient, void *a_message, size_t a_messageSize);


/**
 * @brief Function pause the ServerNet running. 
 *
 * @param[in] a_server - server to be paused.
 *
 * @return return the amout of existing clients.
**/
size_t ServerNet_Pause(ServerNet *a_server);


#endif /* SERVER_NET_H */
