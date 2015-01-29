#ifndef SESSION_H

#include <stdio.h>
#include <winsock2.h>
#include <errno.h>
#include <windows.h>

#define NORMAL_SUCCESS                  0

#define THREAD_FAIL                     -1
#define SESSION_ALREADY_STOPPED_FAIL    -2
#define SESSION_ALREADY_RUNNING_FAIL    -3

// structures
struct Session
{
    // data members
    sockaddr_in _clientAddress;
    SOCKET _clientSocket;

    // callbacks
    void(*onMessage)(struct Session*, int, void*, int);
    void(*onError)(struct Session*, int, void*, int);
    void(*onClose)(struct Session*, int, void*, int);

    // threads and synchronization
    HANDLE _stopEvent;
    HANDLE _sessionThread;
};

typedef struct Session Session;

// server functions
void sessionInit(Session*, SOCKET*, sockaddr_in*);
int sessionClose(Session*);

#define SESSION_H
#endif
