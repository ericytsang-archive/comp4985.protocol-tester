/**
 * the file contains functions and thread routines used to perform operations on
 *   the Client structures.
 *
 * @sourceFile Client.cpp
 *
 * @program    ProtocolTester.exe
 *
 * @function   void clientInit(Client* client)
 * @function   int clientConnectUDP(Client* client, char* remoteName, unsigned
 *   short remotePort)
 * @function   int clientConnectTCP(Client* client, char* remoteName, unsigned
 *   short remotePort)
 * @function   BOOL clientIsConnecting(Client* client)
 * @function   static int clientConnect(Client* client, LPTHREAD_START_ROUTINE
 *   connectThreadFunc, char* remoteName, unsigned short remotePort)
 * @function   static DWORD WINAPI clientTCPThread(void* params)
 * @function   static DWORD WINAPI clientUDPThread(void* params)
 *
 * @date       2015-01-31
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 */
#include "Client.h"

// static function declarations
static int clientConnect(Client*, LPTHREAD_START_ROUTINE, char*,
    unsigned short);
static DWORD WINAPI clientTCPThread(void*);
static DWORD WINAPI clientUDPThread(void*);

/////////////////////////
// interface functions //
/////////////////////////

/**
 * initializes the passed client structure.
 *
 * @function   clientInit
 *
 * @date       2015-01-31
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 *
 * @signature  void clientInit(Client* client)
 *
 * @param      client pointer to client structure to initialize
 */
void clientInit(Client* client)
{
    client->_usrPtr       = 0;
    client->_remoteName   = 0;
    client->_remotePort   = 0;

    client->onConnect     = 0;
    client->onError       = 0;

    client->_clientThread = INVALID_HANDLE_VALUE;
}

/**
 * starts a UDP session that's messages are directed at the specified address
 *   and port.
 *
 * @function   clientConnectUDP
 *
 * @date       2015-01-31
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 *
 * @signature  int clientConnectUDP(Client* client, char* remoteName, unsigned
 *   short remotePort)
 *
 * @param      client pointer to the client structure; the "this" pointer.
 * @param      remoteName null terminated string that identifies the remote
 *   host. it's either a name, or in dotted IP format.
 * @param      remotePort port on the remote host to direct messages to.
 *
 * @return     status code indicating the result of the operation; see the notes
 *   section of the function clientConnect for more details.
 */
int clientConnectUDP(Client* client, char* remoteName,
    unsigned short remotePort)
{
    return clientConnect(client, clientUDPThread, remoteName, remotePort);
}

/**
 * starts a TCP session that's messages are directed at the specified address
 *   and port.
 *
 * @function   clientConnectTCP
 *
 * @date       2015-01-31
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 *
 * @signature  int clientConnectTCP(Client* client, char* remoteName, unsigned
 *   short remotePort)
 *
 * @param      client pointer to the client structure; the "this" pointer.
 * @param      remoteName null terminated string that identifies the remote
 *   host. it's either a name, or in dotted IP format.
 * @param      remotePort port on the remote host to direct messages to.
 *
 * @return     status code indicating the result of the operation; see the notes
 *   section of the function clientConnect for more details.
 */
int clientConnectTCP(Client* client, char* remoteName,
    unsigned short remotePort)
{
    return clientConnect(client, clientTCPThread, remoteName, remotePort);
}

/**
 * non-blocking. returns true if the client is currently connecting to a remote
 *   host; false otherwise.
 *
 * @function   clientIsConnecting
 *
 * @date       2015-01-31
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 *
 * @signature  BOOL clientIsConnecting(Client* client)
 *
 * @param      client pointer to the client structure; the "this" pointer.
 *
 * @return     true if the client is currently connecting to a remote host;
 *   false otherwise.
 */
BOOL clientIsConnecting(Client* client)
{
    return (client->_clientThread != INVALID_HANDLE_VALUE
        && WaitForSingleObject(client->_clientThread, 1) == WAIT_TIMEOUT);
}

//////////////////////
// static functions //
//////////////////////

/**
 * makes sure that the client is in a valid state to connect, and begins the
 *   passed routine on a new thread that's used to make the new connection.
 *
 * @function   clientConnect
 *
 * @date       2015-01-31
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note
 *
 * returns ALREADY_RUNNING_FAIL when another instance of a connecting thread is
 *   in progress.
 *
 * returns THREAD_FAIL when the function fails to create a new thread for
 *   whatever reason.
 *
 * returns NORMAL_SUCCESS when the connection routine is started on a new
 *   thread.
 *
 * @signature  static int clientConnect(Client* client, LPTHREAD_START_ROUTINE
 *   connectThreadFunc, char* remoteName, unsigned short remotePort)
 *
 * @param      client pointer to the client structure; the "this" pointer.
 * @param      connectThreadFunc connection routine to start on another thread.
 * @param      remoteName null terminated string that identifies the remote
 *   host. it's either a name, or in dotted IP format.
 * @param      remotePort port on the remote host to direct messages to.
 *
 * @return     status code indicating the result of the operation; see the notes
 *   section for more details.
 */
static int clientConnect(Client* client,
    LPTHREAD_START_ROUTINE connectThreadFunc, char* remoteName,
    unsigned short remotePort)
{
    DWORD threadId;     // useless...

    // make sure client isn't already connecting
    if(clientIsConnecting(client))
    {
        client->onError(client, ALREADY_RUNNING_FAIL, GetLastError());
        return ALREADY_RUNNING_FAIL;
    }

    // forget about the last client thread
    CloseHandle(client->_clientThread);
    client->_clientThread = INVALID_HANDLE_VALUE;

    // prepare thread parameters
    client->_remoteName = (char*) malloc(strlen(remoteName)+1);
    strcpy_s(client->_remoteName, strlen(remoteName)+1, remoteName);
    client->_remotePort = remotePort;

    // start the client
    client->_clientThread =
        CreateThread(NULL, 0, connectThreadFunc, client, 0, &threadId);
    if(client->_clientThread == INVALID_HANDLE_VALUE)
    {
        client->onError(client, THREAD_FAIL, GetLastError());
        return THREAD_FAIL;
    }

    return NORMAL_SUCCESS;
}

/**
 * creates a TCP connection, and calls the onConnect callback on success.
 *
 * @function   clientTCPThread
 *
 * @date       2015-01-31
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note
 *
 * returns UNKNOWN_IP_FAIL when a host name was passed, but it could not be
 *   resolved into a dotted IP format. this is accompanied with an invocation of
 *   the client.onError callback.
 *
 * returns SOCKET_FAIL when the function fails to create a socket. this is
 *   accompanied with an invocation of the client.onError callback.
 *
 * returns CONNECT_FAIL when the function fails to make the connection. this is
 *   accompanied with an invocation of the client.onError callback.
 *
 * returns NORMAL_SUCCESS when the function succeeds at connecting with the
 *   remote host. this is accompanied with an invocation of the client.onConnect
 *   callback.
 *
 * @signature  static DWORD WINAPI clientTCPThread(void* params)
 *
 * @param      params pointer to a client structure
 *
 * @return     integer indicating the outcome of the operation.
 */
static DWORD WINAPI clientTCPThread(void* params)
{
    Client* client = (Client*) params;

    // resolve host name to IP
    hostent* server = gethostbyname(client->_remoteName);
    if (server == NULL)
    {
        client->onError(client, UNKNOWN_IP_FAIL, GetLastError());
        return UNKNOWN_IP_FAIL;
    }

    // initialize remote address structure
    sockaddr_in remoteAddress;
    memset(&remoteAddress, 0, sizeof(sockaddr_in));
    remoteAddress.sin_family      = AF_INET;
    remoteAddress.sin_port        = htons(client->_remotePort);
    memcpy(&remoteAddress.sin_addr, server->h_addr, server->h_length);

    // create the socket
    SOCKET newSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (newSocket == SOCKET_ERROR)
    {
        client->onError(client, SOCKET_FAIL, GetLastError());
        return SOCKET_FAIL;
    }

    // connecting to the server
    if (connect(newSocket, (sockaddr*) &remoteAddress,
        sizeof(sockaddr_in)) == SOCKET_ERROR)
    {
        client->onError(client, CONNECT_FAIL, GetLastError());
        closesocket(newSocket);
        return CONNECT_FAIL;
    }

    // connection success
    client->onConnect(client, newSocket, remoteAddress);
    return NORMAL_SUCCESS;
}

/**
 * creates a TCP connection, and calls the onConnect callback on success.
 *
 * @function   clientUDPThread
 *
 * @date       2015-01-31
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note
 *
 * returns UNKNOWN_IP_FAIL when a host name was passed, but it could not be
 *   resolved into a dotted IP format. this is accompanied with an invocation of
 *   the client.onError callback.
 *
 * returns SOCKET_FAIL when the function fails to create a socket. this is
 *   accompanied with an invocation of the client.onError callback.
 *
 * returns BIND_FAIL when the function fails to bind the socket to the local
 *   port. this is accompanied with an invocation of the client.onError
 *   callback.
 *
 * returns NORMAL_SUCCESS when the function succeeds at connecting with the
 *   remote host. this is accompanied with an invocation of the client.onConnect
 *   callback.
 *
 * @signature  static DWORD WINAPI clientUDPThread(void* params)
 *
 * @param      params pointer to a client structure
 *
 * @return     integer indicating the outcome of the operation.
 */
static DWORD WINAPI clientUDPThread(void* params)
{
    Client* client = (Client*) params;

    // resolve host name to IP
    hostent* server = gethostbyname(client->_remoteName);
    if (server == NULL)
    {
        client->onError(client, UNKNOWN_IP_FAIL, GetLastError());
        return UNKNOWN_IP_FAIL;
    }

    // initialize remote address structure
    sockaddr_in remoteAddress;
    remoteAddress.sin_family = AF_INET;
    remoteAddress.sin_port   = htons(client->_remotePort);
    memcpy(&remoteAddress.sin_addr, server->h_addr, server->h_length);

    // initialize local address structure
    sockaddr_in localAddress;
    localAddress.sin_family      = AF_INET;
    localAddress.sin_port        = htons(0);
    localAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    // create the socket
    SOCKET newSocket = socket(PF_INET, SOCK_DGRAM, 0);
    if (newSocket == SOCKET_ERROR)
    {
        client->onError(client, SOCKET_FAIL, GetLastError());
        return SOCKET_FAIL;
    }

    // bind local address to the socket
    if (bind(newSocket, (sockaddr*) &localAddress,
        sizeof(sockaddr)) == SOCKET_ERROR)
    {
        client->onError(client, BIND_FAIL, GetLastError());
        closesocket(newSocket);
        return BIND_FAIL;
    }

    // connection success
    client->onConnect(client, newSocket, remoteAddress);
    return NORMAL_SUCCESS;
}
