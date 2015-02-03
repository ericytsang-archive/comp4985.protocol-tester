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
};

typedef struct CtrlSvr CtrlSvr;

void ctrlSvrInit(Server*, ServerWnds*);
void ctrlSvrOnConnect(Server*, SOCKET, sockaddr_in);
void ctrlSvrOnError(Server*, int, int);
void ctrlSvrOnClose(Server*, int);
void ctrlSvrStart(Server*);
void ctrlSvrStop(Server*);
void ctrlSvrSendChat(Server*);

#endif
