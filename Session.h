#ifndef SESSION_H
#define SESSION_H

#include <stdio.h>
#include <winsock2.h>
#include <errno.h>
#include <windows.h>

// constants
#define DEFAULT_BUFFER_LEN 80

// return codes
#define NORMAL_SUCCESS                  0
#define THREAD_FAIL                     -1
#define SESSION_ALREADY_STOPPED_FAIL    -2
#define SESSION_ALREADY_RUNNING_FAIL    -3
#define UNKNOWN_FAIL                    -4
#define SOCKET_FAIL                     -5

// structures
struct Session
{
    // data members
    sockaddr_in _clientAddress;
    SOCKET _clientSocket;
    int _bufLen;

    // callbacks
    void(*onMessage)(struct Session*, char*, int);
    void(*onError)(struct Session*, int);
    void(*onClose)(struct Session*, int);

    // threads and synchronization
    HANDLE _stopEvent;
    HANDLE _sessionThread;
};

typedef struct Session Session;

// server functions
void sessionInit(Session*, SOCKET*, sockaddr_in*);
int sessionClose(Session*);
void sessionSetBufLen(Session*, int);
int sessionSend(Session*, void*, int);

#endif
