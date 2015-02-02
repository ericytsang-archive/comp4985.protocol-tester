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

void ctrlSvrOnConnect(Server*, SOCKET, sockaddr_in);
void ctrlSvrOnError(Server*, int, int);
void ctrlSvrOnClose(Server*, int);

#endif
