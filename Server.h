#ifndef SERVER_H

#include <stdio.h>
#include <winsock2.h>
#include <errno.h>
#include <windows.h>

#define NORMAL_SUCCESS 0

#define THREAD_FAIL -1
#define SOCKET_FAIL -2
#define BIND_FAIL -3
#define SERVER_ALREADY_RUNNING_FAIL -4
#define ACCEPT_FAIL -5
#define UNKNOWN_FAIL -6

// structures
struct Server
{
    // data members
    sockaddr_in _server;
    SOCKET _acceptSocket;

    // callbacks
    void(*_onConnect)(struct Server*, SOCKET);
    void(*_onError)(struct Server*, int, void*, int);
    void(*_onClose)(struct Server*, int, void*, int);

    // threads and synchronization
    HANDLE _acceptEvent;
    HANDLE _stopEvent;
    HANDLE _serverThread;
};

typedef struct Server Server;

// server functions
void serverInit(Server*, short, unsigned short, unsigned long);
void serverStart(Server*);
void serverStop(Server*);
void serverSetOnConnect(Server*, void(*)(Server*, SOCKET));
void serverSetOnError(Server*, void(*)(Server*, int, void*, int));
void serverSetOnClose(Server*, void(*)(Server*, int, void*, int));

#define SERVER_H
#endif
