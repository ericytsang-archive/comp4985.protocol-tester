#ifndef CONTROLCLIENT_H
#define CONTROLCLIENT_H

#include "ControlClientSession.h"
#include "Session.h"
#include "Main.h"

struct CtrlClnt
{
    struct ClientWnds* clientWnds;
    struct Session* ctrlSession;
    struct Session* testSession;

    int testProtocol;
    int testPort;
};

typedef struct CtrlClnt CtrlClnt;

void ctrlClntOnConnect(Client*, SOCKET, sockaddr_in);
void ctrlClntOnError(Client*, int, int);

#endif
