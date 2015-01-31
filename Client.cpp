#include "Client.h"

static DWORD WINAPI clientThread(void* params);

int clientConnect(Client* client, char* hostName,
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
    strcpy_s(client->_remoteName, hostName);
    client->_remotePort = remotePort;

    // start the client
    ResetEvent(client->_stopEvent);
    client->_clientThread =
        CreateThread(NULL, 0, clientThread, client, 0, &threadId);
    if(client->_clientThread == INVALID_HANDLE_VALUE)
    {
        client->onError(client, THREAD_FAIL);
        return THREAD_FAIL;
    }

    return NORMAL_SUCCESS;
}

int clientCancelConnect(Client* client)
{
    // make sure server is already connecting
    if(!clientIsConnecting(client))
    {
        return ALREADY_STOPPED_FAIL;
    }

    // signal server thread to stop
    SetEvent(client->_stopEvent);

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

    SOCKET clientSocket;
    sockaddr_in remoteAddress;
    hostent* server;

    // create the socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == SOCKET_ERROR)
    {
        client->onError(client, SOCKET_FAIL);
        return SOCKET_FAIL;
    }

    // initialize and set up the address structure
    memset(&remoteAddress, 0, sizeof(sockaddr_in));
    remoteAddress.sin_family = AF_INET;
    remoteAddress.sin_port   = htons(client->_remotePort);

    server = gethostbyname(client->_remoteName);
    if (server == NULL)
    {
        client->onError(client, UNKNOWN_IP_FAIL);
        return UNKNOWN_IP_FAIL;
    }

    // extract host IP from query
    memcpy(&remoteAddress.sin_addr, server->h_addr, server->h_length);

    // connecting to the server
    if (connect(clientSocket, (sockaddr*) &server, sizeof(hostent)) == SOCKET_ERROR)
    {
        client->onError(client, CONNECT_FAIL);
        return CONNECT_FAIL;
    }
    
    client->onConnect(client, clientSocket, remoteAddress);
    return NORMAL_SUCCESS;
}
