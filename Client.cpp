#include "Client.h"

static char debugString[1000];

static int clientConnect(Client*, LPTHREAD_START_ROUTINE, char*,
    unsigned short);
static DWORD WINAPI clientTCPThread(void*);
static DWORD WINAPI clientUDPThread(void*);

/////////////////////////
// interface functions //
/////////////////////////

void clientInit(Client* client)
{
    client->_usrPtr       = 0;
    client->_remoteName   = 0;
    client->_remotePort   = 0;

    client->onConnect     = 0;
    client->onError       = 0;

    client->_clientThread = INVALID_HANDLE_VALUE;
}

int clientConnectUDP(Client* client, char* remoteName,
    unsigned short remotePort)
{
    return clientConnect(client, clientUDPThread, remoteName, remotePort);
}

int clientConnectTCP(Client* client, char* remoteName,
    unsigned short remotePort)
{
    return clientConnect(client, clientTCPThread, remoteName, remotePort);
}

BOOL clientIsConnecting(Client* client)
{
    return (client->_clientThread != INVALID_HANDLE_VALUE
        && WaitForSingleObject(client->_clientThread, 1) == WAIT_TIMEOUT);
}

void clientSetUserPtr(Client* client, void* ptr)
{
    client->_usrPtr = ptr;
}

void* clientGetUserPtr(Client* client)
{
    return client->_usrPtr;
}

//////////////////////
// static functions //
//////////////////////

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
        return CONNECT_FAIL;
    }

    // connection success
    client->onConnect(client, newSocket, remoteAddress);
    return NORMAL_SUCCESS;
}

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
        return BIND_FAIL;
    }

    // connection success
    client->onConnect(client, newSocket, remoteAddress);
    return NORMAL_SUCCESS;
}
