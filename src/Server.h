/**
 * header file exposes the interface of the Server.cpp file.
 *
 * @sourceFile Server.h
 *
 * @program    ProtocolTester.exe
 *
 * @function   void serverInit(Server*);
 * @function   int serverSetPort(Server*, unsigned short);
 * @function   int serverOpenUDPPort(Server* server, unsigned short port);
 * @function   int serverStart(Server*);
 * @function   int serverStop(Server*);
 * @function   BOOL serverIsRunning(Server*);
 *
 * @date       2015-02-09
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 */
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

    // public data members
    void* usrPtr;

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
int serverOpenUDPPort(Server* server, unsigned short port);
int serverStart(Server*);
int serverStop(Server*);
BOOL serverIsRunning(Server*);

#endif
