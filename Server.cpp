/**
 * contains functions and thread routines that perform operations on server
 *   structures.
 *
 * @sourceFile Server.cpp
 *
 * @program    ProtocolTester.exe
 *
 * @function   void serverInit(Server* server)
 * @function   int serverSetPort(Server* server, unsigned short port)
 * @function   int serverOpenUDPPort(Server* server)
 * @function   int serverStart(Server* server)
 * @function   int serverStop(Server* server)
 * @function   BOOL serverIsRunning(Server* server)
 * @function   static DWORD WINAPI serverThread(void* params)
 * @function   static HANDLE asyncAccept(HANDLE eventHandle, SOCKET
 *   serverSocket, SOCKET* clientSocket, sockaddr* clientAddress, int*
 *   clientLength)
 * @function   static DWORD WINAPI asyncAcceptThread(void* params)
 *
 * @date       2015-02-01
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 */
#include "Server.h"

/**
 * parameter structure for the asyncAcceptThread routine.
 */
struct AsyncAcceptThreadParams
{
    HANDLE eventHandle;         // signaled when a connection is accepted.
    SOCKET serverSocket;        // listening socket.
    SOCKET* clientSocket;       // socket created from accepting the connection.
    sockaddr* clientAddress;    // address of the remote host of the connection.
    int* clientLength;          // i don't know what this is, but i need it.
};

typedef struct AsyncAcceptThreadParams AsyncAcceptThreadParams;

// static function declarations
static DWORD WINAPI serverThread(void*);
static DWORD WINAPI asyncAcceptThread(void*);
static HANDLE asyncAccept(HANDLE, SOCKET, SOCKET*, sockaddr*, int*);

/////////////////////////
// interface functions //
/////////////////////////

/**
 * initializes a server structure
 *
 * @function   serverInit
 *
 * @date       2015-02-01
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 *
 * @signature  void serverInit(Server* server)
 *
 * @param      server pointer to a Server structure; the "this" pointer.
 */
void serverInit(Server* server)
{
    memset(&server->_server, 0, sizeof(sockaddr_in));
    server->_server.sin_family      = AF_INET;
    server->_server.sin_port        = htons(0);
    server->_server.sin_addr.s_addr = htonl(INADDR_ANY);

    server->usrPtr      = 0;

    server->onConnect   = 0;
    server->onError     = 0;
    server->onClose     = 0;

    server->_stopEvent      = CreateEvent(NULL, TRUE, FALSE, NULL);
    server->_serverThread   = INVALID_HANDLE_VALUE;
}

/**
 * sets the listening port of the server unless the server is running.
 *
 * @function   serverSetPort
 *
 * @date       2015-02-01
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note
 *
 * returns ALREADY_RUNNING_FAIL if the server is currently listening for
 *   connections; the listening port cannot be changed.
 *
 * returns NORMAL_SUCCESS if the listening port is successfully changed.
 *
 * @signature  int serverSetPort(Server* server, unsigned short port)
 *
 * @param      server pointer to a Server structure; the "this" pointer.
 * @param      port new port to listen for new connections on.
 *
 * @return     status code indicating the result of the operation; look at the
 *   notes section for more details.
 */
int serverSetPort(Server* server, unsigned short port)
{
    // make sure server isn't already running
    if(serverIsRunning(server))
    {
        return ALREADY_RUNNING_FAIL;
    }

    server->_server.sin_port = htons(port);
    return NORMAL_SUCCESS;
}

/**
 * tries to open a UDP socket on the specified port, and invokes the onConnect
 *   callback on success.
 *
 * @function   serverOpenUDPPort
 *
 * @date       2015-02-01
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note
 *
 * returns SOCKET_FAIL if socket creation fails.
 *
 * returns BIND_FAIL if binding the socket to the specified port fails.
 *
 * @signature  int serverOpenUDPPort(Server* server)
 *
 * @param      server pointer to a Server structure; the "this" pointer.
 * @param      port port to open the UDP port on.
 *
 * @return     returns a status code indicating the outcome of the operation.
 *   see the notes section for more details.
 */
int serverOpenUDPPort(Server* server, unsigned short port)
{
    sockaddr_in clientAddress;
    memset(&clientAddress, 0, sizeof(sockaddr_in));

    // create the socket
    SOCKET newSocket = socket(PF_INET, SOCK_DGRAM, 0);
    if (newSocket == SOCKET_ERROR)
    {
        return SOCKET_FAIL;
    }

    // initialize local address structure
    sockaddr_in localAddress;
    localAddress.sin_family      = AF_INET;
    localAddress.sin_port        = htons(port);
    localAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    // bind local address to the socket
    if (bind(newSocket, (sockaddr*) &localAddress,
        sizeof(localAddress)) == SOCKET_ERROR)
    {
        closesocket(newSocket);
        return BIND_FAIL;
    }

    // invoke onConnect callback
    server->onConnect(server, newSocket, clientAddress);
}

/**
 * starts the server if it is not already started
 *
 * @function   serverStart
 *
 * @date       2015-02-01
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note
 *
 * returns ALREADY_RUNNING_FAIL if the server is already running and thus cannot
 *   be started.
 *
 * returns THREAD_FAIL if somehow a new thread for the server could not be
 *   created.
 *
 * returns NORMAL_SUCCESS if a server thread is successfully created.
 *
 * @signature  int serverStart(Server* server)
 *
 * @param      server pointer to a Server structure; the "this" pointer.
 *
 * @return     status code indication the result of the operation; see the notes
 *   section for more details.
 */
int serverStart(Server* server)
{
    DWORD threadId;     // useless...

    // make sure server isn't already running
    if(serverIsRunning(server))
    {
        return ALREADY_RUNNING_FAIL;
    }

    // forget about the last server thread (since it's stopped)
    CloseHandle(server->_serverThread);
    server->_serverThread = INVALID_HANDLE_VALUE;

    // start the server
    ResetEvent(server->_stopEvent);
    server->_serverThread =
        CreateThread(NULL, 0, serverThread, server, 0, &threadId);
    if(server->_serverThread == INVALID_HANDLE_VALUE)
    {
        return THREAD_FAIL;
    }

    return NORMAL_SUCCESS;
}

/**
 * requests server to close listening socket, but not accepted sockets
 *
 * @function   serverStop
 *
 * @date       2015-02-01
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note
 *
 * returns ALREADY_STOPPED_FAIL if the server is already stopped, and this
 *   cannot be stopped.
 *
 * returns NORMAL_SUCCESS if the server is has been stopped successfully by this
 *   call.
 *
 * @signature  int serverStop(Server* server)
 *
 * @param      server pointer to a Server structure; the "this" pointer.
 *
 * @return     status code indication the result of the operation; see the notes
 *   section for more details.
 */
int serverStop(Server* server)
{
    // make sure server is still running
    if(!serverIsRunning(server))
    {
        return ALREADY_STOPPED_FAIL;
    }

    // signal server thread to stop
    SetEvent(server->_stopEvent);

    return NORMAL_SUCCESS;
}

/**
 * non-blocking. returns true if the server is currently listening for new
 *   connections; false otherwise.
 *
 * @function   serverIsRunning
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
 * @signature  BOOL serverIsRunning(Server* server)
 *
 * @param      server pointer to a Server structure; the "this" pointer.
 *
 * @return     true if the server is currently listening for new connections;
 *   false otherwise.
 */
BOOL serverIsRunning(Server* server)
{
    return (server->_serverThread != INVALID_HANDLE_VALUE
        && WaitForSingleObject(server->_serverThread, 1) == WAIT_TIMEOUT);
}

//////////////////////
// static functions //
//////////////////////

/**
 * server's thread that is used to continuously accept connections
 *
 * @function   serverThread
 *
 * @date       2015-02-01
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note
 *
 * returns SOCKET_FAIL if the thread fails to make a new server socket to listen
 *   for new connections. this is accompanied with an invocation of the
 *   server.onError callback.
 *
 * returns BIND_FAIL if the thread fails to bind the server socket to a local
 *   port as it may be in use by another program. this is accompanied with an
 *   invocation of the server.onError callback.
 *
 * returns THREAD_FAIL if the server fails to start the asynchronous accept
 *   thread. this is accompanied with an invocation of the server.onError
 *   callback.
 *
 * returns ACCEPT_FAIL if the server fails to accept a connection for whatever
 *   reason. this is accompanied with an invocation of the server.onError
 *   callback.
 *
 * returns NORMAL_SUCCESS if the server is stopped normally. this is accompanied
 *   with an invocation of the server.onError callback.
 *
 * returns UNKNOWN_FAIL if the server encounters some sort of error that i
 *   didn't anticipate. this is accompanied with an invocation of the
 *   server.onError callback.
 *
 * @signature  static DWORD WINAPI serverThread(void* params)
 *
 * @param      params pointer to a Server structure.
 *
 * @return     return code indicating the reason why the thread ended; see the
 *   notes section for more details.
 */
static DWORD WINAPI serverThread(void* params)
{
    Server* server = (Server*) params;

    // threads and synchronization
    HANDLE acceptThread = CreateEvent(NULL, TRUE, TRUE, NULL);
    HANDLE acceptEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    // flow and return value
    int returnValue;
    BOOL breakLoop = FALSE;

    // create a stream socket
    SOCKET serverSocket = socket(server->_server.sin_family, SOCK_STREAM, 0);
    if(serverSocket == SOCKET_ERROR)
    {
        server->onError(server, SOCKET_FAIL, GetLastError());
        server->onClose(server, SOCKET_FAIL);
        return SOCKET_FAIL;
    }

    // Bind an address to the socket
    if(bind(serverSocket, (sockaddr*) &server->_server,
        sizeof(server->_server)) == SOCKET_ERROR)
    {
        server->onError(server, BIND_FAIL, GetLastError());
        server->onClose(server, BIND_FAIL);
        return BIND_FAIL;
    }

    // listen for connections
    listen(serverSocket, 5);     // queue up to 5 connect requests

    // continuously accept connections until error, or cancel
    while(!breakLoop)
    {

        SOCKET clientSocket;
        sockaddr_in clientAddress;
        int clientLength = sizeof(clientAddress);

        // accept a connection asynchronously
        acceptThread = asyncAccept(acceptEvent, serverSocket, &clientSocket,
            (sockaddr*) &clientAddress, &clientLength);
        if(acceptThread == INVALID_HANDLE_VALUE)
        {
            server->onError(server, THREAD_FAIL, GetLastError());
            server->onClose(server, THREAD_FAIL);
            return THREAD_FAIL;
        }

        // wait for a connection to be accepted, or a stop signal
        ResetEvent(acceptEvent);
        HANDLE handles[] = {acceptEvent, server->_stopEvent};
        DWORD waitResult =
            WaitForMultipleObjects(sizeof(handles) / sizeof(HANDLE),
                handles, FALSE, INFINITE);
        switch(waitResult)
        {
            case WAIT_OBJECT_0+0:   // accept event signaled; handle it
                if(clientSocket == SOCKET_ERROR)
                {   // handle error
                    server->onError(server, ACCEPT_FAIL, GetLastError());
                    server->onClose(server, ACCEPT_FAIL);
                    returnValue = ACCEPT_FAIL;
                    breakLoop = TRUE;
                }
                else
                {   // handle new connection
                    server->onConnect(server, clientSocket, clientAddress);
                }
                break;
            case WAIT_OBJECT_0+1:   // stop event signaled; stop the server
                server->onClose(server, NORMAL_SUCCESS);
                returnValue = NORMAL_SUCCESS;
                breakLoop = TRUE;
                break;
            default:                // some sort of something; report error
                server->onError(server, UNKNOWN_FAIL, GetLastError());
                server->onClose(server, UNKNOWN_FAIL);
                returnValue = UNKNOWN_FAIL;
                breakLoop = TRUE;
                break;
        }
    }

    // clean up & return
    closesocket(serverSocket);
    WaitForSingleObject(acceptThread, INFINITE);
    return returnValue;
}

/**
 * signals the event when the accept call finishes. the function returns a
 *   handle to the accept thread.
 *
 * @function   asyncAccept
 *
 * @date       2015-02-01
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 *
 * @signature  static HANDLE asyncAccept(HANDLE eventHandle, SOCKET
 *   serverSocket, SOCKET* clientSocket, sockaddr* clientAddress, int*
 *   clientLength)
 *
 * @param      eventHandle event that is signaled when a connection is accepted.
 * @param      serverSocket reference to the server's socket to accept
 *   connections from.
 * @param      clientSocket new socket that's connected to a remote host from
 *   the accept call.
 * @param      clientAddress remote host's address from the accept call.
 * @param      clientLength i don't know what this is, but it is necessary.
 *
 * @return     handle to the accept thread.
 */
static HANDLE asyncAccept(HANDLE eventHandle, SOCKET serverSocket,
    SOCKET* clientSocket, sockaddr* clientAddress, int* clientLength)
{
    AsyncAcceptThreadParams* threadParams =
        (AsyncAcceptThreadParams*) malloc(sizeof(AsyncAcceptThreadParams));
    DWORD threadId;
    HANDLE threadHandle;

    // prepare thread parameters
    threadParams->eventHandle   = eventHandle;
    threadParams->serverSocket  = serverSocket;
    threadParams->clientSocket  = clientSocket;
    threadParams->clientAddress = clientAddress;
    threadParams->clientLength  = clientLength;

    // make the thread to make asynchronous call
    threadHandle =
        CreateThread(NULL, 0, asyncAcceptThread, threadParams, 0, &threadId);

    return threadHandle;
}

/**
 * routine that's run on a separate thread to make the accepting asynchronous.
 *
 * @function   asyncAcceptThread
 *
 * @date       2015-02-01
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 *
 * @signature  static DWORD WINAPI asyncAcceptThread(void* params)
 *
 * @param      params pointer to a AsyncAcceptThreadParams structure.
 *
 * @return     returns 0.
 */
static DWORD WINAPI asyncAcceptThread(void* params)
{
    // parse thread params
    AsyncAcceptThreadParams* threadParams = (AsyncAcceptThreadParams*) params;

    // make the accept call
    *threadParams->clientSocket = accept(threadParams->serverSocket,
        threadParams->clientAddress, threadParams->clientLength);

    // trigger the signal, because we're no longer blocked by accept
    SetEvent(threadParams->eventHandle);

    // clean up and return
    free(threadParams);
    return 0;
}
