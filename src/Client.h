/**
 * header file that exposes the interface functions of client.cpp
 *
 * @sourceFile Client.h
 *
 * @program    ProtocolTester.exe
 *
 * @function   void clientInit(Client*);
 * @function   int clientConnectUDP(Client*, char*, unsigned short);
 * @function   int clientConnectTCP(Client*, char*, unsigned short);
 * @function   BOOL clientIsConnecting(Client*);
 *
 * @date       2015-02-05
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 */
#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <winsock2.h>
#include <errno.h>
#include <windows.h>
#include "ReturnCodes.h"

// structures

/** structure for the Client object. */
struct Client
{
    // private data members
    /**
     * null-terminated string of remote host when establishing new connections;
     *   null otherwise.
     */
    char* _remoteName;
    /**
     * port of remote host to connect to during connection establishment; 0
     *   otherwise.
     */
    unsigned short _remotePort;

    // public data members
    /**
     * pointer to anything you like.
     */
    void* usrPtr;

    // callbacks
    /**
     * invoked when a new connection is established.
     *
     * @param dis        pointer to client structure that's invoking callback.
     * @param newSocket  new socket object used to send to and receive from
     *   remote host.
     * @param remoteAddr address of the remote host that the socket would send
     *   to.
     */
    void(*onConnect)(struct Client* dis, SOCKET newSocket,
        sockaddr_in remoteAddr);
    /**
     * invoked when an error occurred while trying to connect to remote address.
     *
     * @param dis        pointer to client structure that's invoking callback.
     * @param errCode    error code indicating the general nature of the error.
     * @param winErrCode window's error code indicating the nature of the error.
     */
    void(*onError)(struct Client* dis, int errCode, int winErrCode);

    // threads and synchronization
    /**
     * handle to the client's connection establishing thread. while the thread
     *   is running, this is a handle to that thread; INVALID_HANDLE_VALUE
     *   otherwise.
     */
    HANDLE _clientThread;
};

typedef struct Client Client;

// client functions
void clientInit(Client*);
int clientConnectUDP(Client*, char*, unsigned short);
int clientConnectTCP(Client*, char*, unsigned short);
BOOL clientIsConnecting(Client*);

#endif
