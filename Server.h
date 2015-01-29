#ifndef SERVER_H

#include <stdio.h>
#include <winsock2.h>
#include <errno.h>
#include <windows.h>

#define NORMAL_SUCCESS                  0

#define UNKNOWN_FAIL                    -1
#define THREAD_FAIL                     -2
#define SOCKET_FAIL                     -3
#define BIND_FAIL                       -4
#define ACCEPT_FAIL                     -5
#define SERVER_ALREADY_RUNNING_FAIL     -6
#define SERVER_ALREADY_STOPPED_FAIL     -7

// structures
struct Server
{
    // data members
    sockaddr_in _server;

    // callbacks
    void(*onConnect)(struct Server*, SOCKET, sockaddr_in);
    void(*onError)(struct Server*, int, void*, int);
    void(*onClose)(struct Server*, int, void*, int);

    // threads and synchronization
    HANDLE _stopEvent;
    HANDLE _serverThread;
};

typedef struct Server Server;

// server functions
void serverInit(Server*, short, unsigned short, unsigned long);
int serverStart(Server*);
int serverStop(Server*);

#define SERVER_H
#endif
