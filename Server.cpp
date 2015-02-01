#include "Server.h"

/**
 * parameter structure for the asyncAcceptThread routine.
 */
struct AsyncAcceptThreadParams
{
    HANDLE eventHandle;
    SOCKET serverSocket;
    SOCKET* clientSocket;
    sockaddr* clientAddress;
    int* clientLength;
};

typedef struct AsyncAcceptThreadParams AsyncAcceptThreadParams;

// static function declarations
static DWORD WINAPI serverThread(void*);
static DWORD WINAPI asyncAcceptThread(void*);
static HANDLE asyncAccept(HANDLE, SOCKET, SOCKET*, sockaddr*, int*);

// initializes a server structure
void serverInit(Server* server)
{
    memset(&server->_server, 0, sizeof(sockaddr_in));
    server->_server.sin_family      = AF_INET;
    server->_server.sin_port        = htons(0);
    server->_server.sin_addr.s_addr = htonl(INADDR_ANY);

    server->_usrPtr     = 0;

    server->onConnect   = 0;
    server->onError     = 0;
    server->onClose     = 0;

    server->_stopEvent      = CreateEvent(NULL, TRUE, FALSE, NULL);
    server->_serverThread   = INVALID_HANDLE_VALUE;
}

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

int serverOpenUDPPort(Server* server)
{
    sockaddr_in clientAddress;
    memset(&clientAddress, 0, sizeof(sockaddr_in));

    // create the socket
    SOCKET newSocket = socket(PF_INET, SOCK_DGRAM, 0);
    if (newSocket == SOCKET_ERROR)
    {
        return SOCKET_FAIL;
    }

    // bind local address to the socket
    if (bind(newSocket, (sockaddr*) &server->_server, sizeof(sockaddr)) == SOCKET_ERROR)
    {
        return BIND_FAIL;
    }

    // invoke onConnect callback
    server->onConnect(server, newSocket, clientAddress);
}

// starts the server if it is not already started
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

// requests server to close listening socket, but not accepted sockets
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

BOOL serverIsRunning(Server* server)
{
    return (server->_serverThread != INVALID_HANDLE_VALUE
        && WaitForSingleObject(server->_serverThread, 1) == WAIT_TIMEOUT);
}

// server's thread that is used to continuously accept connections
static DWORD WINAPI serverThread(void* params)
{
    Server* server = (Server*) params;

    // threads and synchronization
    HANDLE acceptThread = CreateEvent(NULL, TRUE, TRUE, NULL); // will be reassigned later
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

// signals the event when the accept call finishes...newSocket is what accept returns
// returns -1 if the thread could not be created..0 otherwise
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
