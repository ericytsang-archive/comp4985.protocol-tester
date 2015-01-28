#include <stdio.h>
#include <winsock2.h>
#include <errno.h>
#include "Server.h"

#define THREAD_FAIL 1
#define SOCKET_FAIL 2
#define BIND_FAIL   3

static struct Server
{
    sockaddr_in _server;
    SOCKET _acceptSocket;
    HANDLE _stopSignal;
    void(*_onConnect)(SOCKET*);
    void(*_onError)(int, void*, int);
};

typedef struct Server Server;

void serverInit(Server*, short, unsigned short, IN_ADDR);
void serverStart(Server*);
void serverStop(Server*);
void serverSetOnConnect(Server*, void(*)(SOCKET*));
void serverSetOnError(Server*, void(*)(int, void*, int));

DWORD WINAPI serverThread(void*);

// initializes a server structure
void serverInit(Server* server, short protocolFamily,
    unsigned short port , IN_ADDR remoteInetAddr)
{
    memset(&server->_server, 0, sizeof(sockaddr_in));
    server->_server.sin_family = protocolFamily;
    server->_server.sin_port   = port;
    server->_server.sin_addr   = remoteInetAddr;

    server->_stopSignal = CreateEvent(NULL, TRUE, FALSE, NULL);
    server->_acceptSocket   = 0;
    server->_onConnect      = 0;
    server->_onError        = 0;
}

// starts the server, and makes it listen for connections
void serverStart(Server* server)
{
    DWORD threadId;
    HANDLE threadHandle;

    threadHandle = CreateThread(NULL, 0, serverThread, server, 0, &threadId);
    if(threadHandle == INVALID_HANDLE_VALUE)
    {
        server->_onError(THREAD_FAIL, 0, 0);
    }
}

// requests server to close listening socket, but not accepted sockets
void serverStop(Server* server)
{

}

// sets the onConnect function callback of the server
void serverSetOnConnect(Server* server, void(*onConnect)(SOCKET*))
{
    server->_onConnect = onConnect;
}

// sets the onError function callback of the server
void serverSetOnError(Server* server, void(*onError)(int, void*, int))
{
    server->_onError = onError;
}

// server's thread that is used to continuously accept connections
DWORD WINAPI serverThread(void* params)
{
    // parses thread parameters
    Server* server = (Server*) params;

    // create a stream socket
    server->_acceptSocket = socket(server->_server.sin_family, SOCK_STREAM, 0);
    if(server->_acceptSocket == -1)
    {
        server->_onError(SOCKET_FAIL, 0, 0);
        return 1;
    }

    // Bind an address to the socket
    if(bind(server->_acceptSocket, (sockaddr*) &server, sizeof(server)) == -1)
    {
        server->_onError(BIND_FAIL, 0, 0);
        return 1;
    }

    // listen for connections
    listen(server->_acceptSocket, 5);     // queue up to 5 connect requests

    while(TRUE)
    {
        client_len= sizeof(client);
        if ((new_sd = accept (sd, (sockaddr *)&client, &client_len)) == -1)
        {
            fprintf(stderr, "Can't accept client\n");
            exit(1);
        }

        printf(" Remote Address:  %s\n", inet_ntoa(client.sin_addr));
        bp = buf;
        bytes_to_read = BUFSIZE;
        while ((n = recv (new_sd, bp, bytes_to_read, 0)) < BUFSIZE)
        {
            bp += n;
            bytes_to_read -= n;
            if (n == 0)
                break;
        }

        ns = send (new_sd, buf, BUFSIZE, 0);
        closesocket (new_sd);
    }
}
