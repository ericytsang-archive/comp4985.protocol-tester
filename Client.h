#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <winsock2.h>
#include <errno.h>
#include <windows.h>
#include "ReturnCodes.h"

// structures
struct Client
{
    void* _usrPtr;
    char* _remoteName;
    unsigned short _remotePort;

    // callbacks
    void(*onConnect)(struct Client*, SOCKET, sockaddr_in);
    void(*onError)(struct Client*, int);

    // threads and synchronization
    HANDLE _clientThread;
};

typedef struct Client Client;

// client functions
void clientInit(Client*);
int clientConnectUDP(Client*, char*, unsigned short);
int clientConnectTCP(Client*, char*, unsigned short);
BOOL clientIsConnecting(Client*);
void clientSetUserPtr(Client*, void*);
void* clientGetUserPtr(Client*);

#endif
