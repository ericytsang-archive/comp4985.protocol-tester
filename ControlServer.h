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

void ctrlSvrInit(Server*, struct ServerWnds*);
void ctrlSvrStart(Server*);
void ctrlSvrStop(Server*);
void ctrlSvrSendChat(Server*);

#endif
