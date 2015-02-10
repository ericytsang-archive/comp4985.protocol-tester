/**
 * exposes the interface of the ControlServer.cpp file.
 *
 * @sourceFile ControlServer.h
 *
 * @program    ProtocolTester.exe
 *
 * @function   void ctrlSvrInit(Server*, struct ServerWnds*);
 * @function   void ctrlSvrStart(Server*);
 * @function   void ctrlSvrStop(Server*);
 * @function   void ctrlSvrSendChat(Server*);
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
#ifndef CONTROLSERVER_H
#define CONTROLSERVER_H

#include "ControlServerSession.h"
#include "LinkedList.h"
#include "Server.h"
#include "Main.h"

struct CtrlSvr
{
    struct ServerWnds* serverWnds;
    struct LinkedList ctrlSessions;

    HANDLE file;
};

typedef struct CtrlSvr CtrlSvr;

void ctrlSvrInit(Server*, struct ServerWnds*);
void ctrlSvrStart(Server*);
void ctrlSvrStop(Server*);
void ctrlSvrSendChat(Server*);

#endif
