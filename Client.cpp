#include "Client.h"

static char debugString[1000];
static DWORD WINAPI clientThread(void* params);

void clientInit(Client* client)
{
    client->_usrPtr       = 0;
    client->_remotePort   = 0;

    client->onConnect     = 0;
    client->onError       = 0;

    client->_clientThread = INVALID_HANDLE_VALUE;
}

int clientConnectTCP(Client* client, char* hostName,
    unsigned short remotePort)
{
    DWORD threadId;     // useless...

    // make sure client isn't already connecting
    if(clientIsConnecting(client))
    {
        return ALREADY_RUNNING_FAIL;
    }

    // forget about the last client thread
    CloseHandle(client->_clientThread);
    client->_clientThread = INVALID_HANDLE_VALUE;

    // prepare thread parameters
    client->_remoteName = (char*) malloc(strlen(hostName)+1);
    strcpy(client->_remoteName, hostName);
    client->_remotePort = remotePort;

    // start the client
    client->_clientThread =
        CreateThread(NULL, 0, clientThread, client, 0, &threadId);
    if(client->_clientThread == INVALID_HANDLE_VALUE)
    {
        client->onError(client, THREAD_FAIL);
        return THREAD_FAIL;
    }

    return NORMAL_SUCCESS;
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

static DWORD WINAPI clientThread(void* params)
{
    Client* client = (Client*) params;    

    // resolve host name to IP
    hostent* server = gethostbyname(client->_remoteName);
    if (server == NULL)
    {
        client->onError(client, UNKNOWN_IP_FAIL);
        return UNKNOWN_IP_FAIL;
    }

    // initialize remote address structure
    sockaddr_in remoteAddress;
    memset(&remoteAddress, 0, sizeof(sockaddr_in));
    remoteAddress.sin_family      = AF_INET;
    remoteAddress.sin_port        = htons(client->_remotePort);
    memcpy(&remoteAddress.sin_addr, server->h_addr, server->h_length);

    // create the socket
    SOCKET newSocket = socket(remoteAddress.sin_family, SOCK_STREAM, 0);
    if (newSocket == SOCKET_ERROR)
    {
        client->onError(client, SOCKET_FAIL);
        return SOCKET_FAIL;
    }

    // connecting to the server
    if (connect(newSocket, (sockaddr*) &remoteAddress, sizeof(sockaddr_in)) == SOCKET_ERROR)
    {
        client->onError(client, CONNECT_FAIL);
        return CONNECT_FAIL;
    }

    // connection success
    client->onConnect(client, newSocket, remoteAddress);
    return NORMAL_SUCCESS;
}
