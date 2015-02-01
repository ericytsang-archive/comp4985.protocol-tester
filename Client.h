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
    // data members
    char* _remoteName;
    unsigned short _remotePort;

    // public data members
    void* usrPtr;

    // callbacks
    void(*onConnect)(struct Client*, SOCKET, sockaddr_in);
    void(*onError)(struct Client*, int, int);

    // threads and synchronization
    HANDLE _clientThread;
};

typedef struct Client Client;

// client functions
void clientInit(Client*);
int clientConnectUDP(Client*, char*, unsigned short);
int clientConnectTCP(Client*, char*, unsigned short);
BOOL clientIsConnecting(Client*);

#endif
