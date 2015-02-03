#ifndef CONTROLCLIENTSESSION_H
#define CONTROLCLIENTSESSION_H

#include "ControlClient.h"
#include "Session.h"
#include "Main.h"

struct CtrlClntSession
{
    struct ClientWnds* clientWnds;
    struct CtrlClnt* ctrlClnt;

    int lastParsedSection;
    char msgType;
};

typedef struct CtrlClntSession CtrlClntSession;

void ctrlClntSessionInit(Session*, CtrlClnt*, SOCKET, sockaddr_in);
void ctrlClntSessionStartTest(Session*);

#endif
