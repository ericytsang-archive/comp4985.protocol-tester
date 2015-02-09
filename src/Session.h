/**
 * header file exposing the interface of the Session.cpp
 *
 * @sourceFile Session.h
 *
 * @program    ProtocolTester.exe
 *
 * @function   void sessionInit(Session*, SOCKET*, sockaddr_in*);
 * @function   int sessionStart(Session*);
 * @function   int sessionClose(Session*);
 * @function   BOOL sessionIsRunning(Session*);
 * @function   void sessionSetBufLen(Session*, int);
 * @function   int sessionSend(Session*, void*, int);
 * @function   IN_ADDR sessionGetIP(Session*);
 * @function   void sessionSendCtrlMsg(Session*, char, void*, int);
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
#ifndef SESSION_H
#define SESSION_H

#include <stdio.h>
#include <winsock2.h>
#include <errno.h>
#include <windows.h>
#include "Packet.h"
#include "ReturnCodes.h"

// constants
/** length of buffer used to receive data. */
#define DEFAULT_BUFFER_LEN 4

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
