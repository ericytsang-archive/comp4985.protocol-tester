#ifndef CONTROLSERVERSESSION_H
#define CONTROLSERVERSESSION_H

#include "ControlClientSession.h"
#include "LinkedList.h"
#include "Session.h"
#include "Server.h"
#include "Main.h"

struct CtrlSvrSession
{
    struct ServerWnds* serverWnds;
    struct LinkedList* ctrlSessions;

    struct Server testServer;

    int testProtocol;
    int testPort;

    int lastParsedSection;
    char msgType;
};

typedef struct CtrlSvrSession CtrlSvrSession;

void svrCtrlSessionOnMessage(Session*, char*, int);
void svrCtrlSessionOnError(Session*, int, int);
void svrCtrlSessionOnClose(Session*, int);

#endif
