#ifndef CONTROLCLIENTSESSION_H
#define CONTROLCLIENTSESSION_H

#include "ControlClient.h"
#include "Session.h"
#include "Main.h"

struct CtrlClntSession
{
    struct ClientWnds* clientWnds;
    struct Session* ctrlSession;
    struct Session* testSession;
    struct CtrlClnt* ctrlClnt;

    int lastParsedSection;
    char msgType;
};

typedef struct CtrlClntSession CtrlClntSession;

void ctrlClntSessionInit(Session*, CtrlClnt*, SOCKET, sockaddr_in);
void ctrlClntSessionOnMessage(Session*, char*, int);
void ctrlClntSessionOnError(Session*, int, int);
void ctrlClntSessionOnClose(Session*, int);
void ctrlClntSessionStartTest(Session*);

#endif
