#ifndef SESSION_H
#define SESSION_H

#include <stdio.h>
#include <winsock2.h>
#include <errno.h>
#include <windows.h>
#include "Packet.h"
#include "ReturnCodes.h"

// constants
#define DEFAULT_BUFFER_LEN 4
//65535

// structures
struct Session
{
    // data members
    sockaddr_in _remoteAddress;
    int _remoteAddressLen;
    SOCKET _remoteSocket;
    int _bufLen;

    // public data members
    void* usrPtr;

    // callbacks
    void(*onMessage)(struct Session*, char*, int);
    void(*onError)(struct Session*, int, int);
    void(*onClose)(struct Session*, int);

    // threads and synchronization
    HANDLE _stopEvent;
    HANDLE _sessionThread;
    HANDLE _accessMutex;
};

typedef struct Session Session;

// server functions
void sessionInit(Session*, SOCKET*, sockaddr_in*);
int sessionStart(Session*);
int sessionClose(Session*);
BOOL sessionIsRunning(Session*);
void sessionSetBufLen(Session*, int);
int sessionSend(Session*, void*, int);
IN_ADDR sessionGetIP(Session*);
void sessionSendCtrlMsg(Session*, char, void*, int);

#endif