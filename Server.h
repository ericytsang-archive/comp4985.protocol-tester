#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <winsock2.h>
#include <errno.h>
#include <windows.h>
#include "ReturnCodes.h"

// structures
struct Server
{
    // data members
    sockaddr_in _server;
    void* _usrPtr;

    // callbacks
    void(*onConnect)(struct Server*, SOCKET, sockaddr_in);
    void(*onError)(struct Server*, int, int);
    void(*onClose)(struct Server*, int);

    // threads and synchronization
    HANDLE _stopEvent;
    HANDLE _serverThread;
};

typedef struct Server Server;

// server functions
void serverInit(Server*);
int serverSetPort(Server*, unsigned short);
int serverOpenUDPPort(Server*);
int serverStart(Server*);
int serverStop(Server*);
void serverSetUserPtr(Server*, void*);
void* serverGetUserPtr(Server*);
BOOL serverIsRunning(Server*);

#endif
