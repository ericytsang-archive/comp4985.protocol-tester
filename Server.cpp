#include <stdio.h>
#include <winsock2.h>
#include <errno.h>
#include "Server.h"

#define SERVER_TCP_PORT 7000    // Default port
#define BUFSIZE 255             //Buffer length
#define TRUE    1

static struct Server
{
    sockaddr_in _server;
    SOCKET _acceptSocket;
    void(*_onConnect)(SOCKET*);
    void(*_onError)(int, void*, int);
};

typedef struct Server Server;

int serverInit(Server*, short, unsigned short, IN_ADDR);
int serverStart(Server*);
int serverStop(Server*);
int serverSetOnConnect(Server*, void(*)(SOCKET*));
int serverSetOnError(Server*, void(*)(int, void*, int));

DWORD WINAPI serverThread(void*);

// initializes a server structure
int serverInit(Server* server, short protocolFamily,
    unsigned short port , IN_ADDR remoteInetAddr)
{
    memset(&server->_server, 0, sizeof(sockaddr_in));
    server->_server.sin_family = protocolFamily;
    server->_server.sin_port   = port;
    server->_server.sin_addr   = remoteInetAddr;

    server->_acceptSocket   = 0;
    server->_onConnect      = 0;
    server->_onError        = 0;

    return 0;
}

// starts the server, and makes it listen for connections
int serverStart(Server* server)
{
}

// requests server to close listening socket, but not accepted sockets
int serverStop(Server* server)
{

}

// sets the onConnect function callback of the server
int serverSetOnConnect(Server* server, void(*onConnect)(SOCKET*))
{
    server->_onConnect = onConnect;
}

// sets the onError function callback of the server
int serverSetOnError(Server* server, void(*onError)(int, void*, int))
{
    server->_onError = onError;
}

// server's thread that is used to continuously accept connections
DWORD WINAPI serverThread(void* params)
{
    // parses thread parameters
    Server* server = (Server*) params;

    // error strings
    const char SOCKET_FAIL[] = "Can't create a socket";
    const char BIND_FAIL[] = "Can't bind name to socket";

    // create a stream socket
    server->_acceptSocket = socket(server->_server.sin_family, SOCK_STREAM, 0);
    if(server->_acceptSocket == -1)
    {
        server->_onError(1, (void*) SOCKET_FAIL, sizeof(SOCKET_FAIL));
        return 1;
    }

    // Bind an address to the socket
    if(bind(server->_acceptSocket, (sockaddr*) &server, sizeof(server)) == -1)
    {
        server->_onError(1, (void*) BIND_FAIL, sizeof(BIND_FAIL));
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
