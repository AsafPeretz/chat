#ifndef Client_Net_H
#define Client_Net_H

typedef struct ClientNet ClientNet;

typedef enum ClientNet_Result {
	ClientNet_Success,
	ClientNet_Uninitialized_Error,
	ClientNet_Connect_Error,
	ClientNet_MessageReceive_Error,
	ClientNet_SocketHasBeenClosed_Error,
	ClientNet_MessageSend_Error,
	ClientNet_Unknown_Error
}ClientNet_Result;
	

/**
 * @brief Function dynamically creates a new ClientNet ADT of a given ip address and port.
 *
 * @param[in] a_address - server ip address to connect and communicate.
 * @param[in] a_port - server port.
 *
 * @return ClientNet* on success / NULL on fail
**/
ClientNet* ClientNet_Create(char *a_address, int a_port);


/**
 * @brief Function Dynamically deallocates a previously allocated ClientNet, and puts a NULL in the ClientNet pointer.
 *
 * @params[in] a_server - a pointer to previously created ClientNet returned via ClientNet_Create
**/
void ClientNet_Destroy(ClientNet **a_client);


/**
 * @brief Function creates conection with required server (given in ClientNet_Create).
 *
 * @param[in] a_client
 *
 * @return success or error code
 * @return ClientNet_Success
 * @return ClientNet_UninitializedError
 * @return ClientNet_ConnectError
**/
ClientNet_Result ClientNet_Connect(ClientNet *a_client);


/**
 * @brief Function sends data via TCP connection.
 *
 * @param[in] a_client
 * @param[in] a_message - a message to be send.
 * @param[in] a_msgLength - the message length
 *
 * @return success or error code
 * @return ClientNet_Success
 * @return ClientNet_UninitializedError
 * @return ClientNet_MessageSendError
**/
ClientNet_Result ClientNet_Send(ClientNet *a_client, char *a_message, size_t a_msgLength);


/**
 * @brief Function receives data from server via TCP connection.
 *
 * @param[in] a_client
 * @param[in] a_buffer - a buffer to receive the message.
 * @param[in] a_bufferSize - the message length
 *
 * @return success or error code
 * @return ClientNet_Success
 * @return ClientNet_UninitializedError
 * @return ClientNet_MessageReceiveError
 * @return ClientNet_SocketHasBeenClosedError
**/
ClientNet_Result ClientNet_Receive(ClientNet *a_client, char *a_buffer, size_t a_bufferSize);


/**
 * @brief Function return socket number.
 *
 * @param[in] a_client
 *
 * @return socket number
**/
int ClientNet_GetSock(ClientNet *a_client);

#endif /* Client_Net_H */
