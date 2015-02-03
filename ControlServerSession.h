#ifndef CONTROLSERVERSESSION_H
#define CONTROLSERVERSESSION_H

#include "ControlClientSession.h"
#include "ControlServer.h"
#include "TestServer.h"
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

void ctrlSvrSessionInit(Session*, CtrlSvr*, SOCKET, sockaddr_in);
void ctrlSvrSessionOnMessage(Session*, char*, int);
void ctrlSvrSessionOnError(Session*, int, int);
void ctrlSvrSessionOnClose(Session*, int);

#endif
