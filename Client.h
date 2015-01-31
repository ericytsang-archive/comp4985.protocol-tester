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
    char _remoteName[80];
    unsigned short _remotePort;

    // callbacks
    void(*onConnect)(struct Client*, SOCKET, sockaddr_in);
    void(*onError)(struct Client*, int);

    // threads and synchronization
    HANDLE _stopEvent;
    HANDLE _clientThread;
};

typedef struct Client Client;

// client functions
int clientConnect(Client*, unsigned long, unsigned short);
int clientCancelConnect(Client*);
BOOL clientIsConnecting(Client*);
void clientSetUserPtr(Client*, void*);
void* clientGetUserPtr(Client*);

#endif
