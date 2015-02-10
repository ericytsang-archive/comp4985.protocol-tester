/**
 * exposes the interface of ColtrolClientSession.cpp
 *
 * @sourceFile ControlClientSession.h
 *
 * @program    ProtocolTester.exe
 *
 * @function   void ctrlClntSessionInit(Session*, struct CtrlClnt*, SOCKET,
 *   sockaddr_in);
 * @function   void ctrlClntSessionStartTest(Session*);
 *
 * @date       2015-02-09
 *
 * @revision   none
 *
 * @designer   [c description]
 *
 * @programmer [c description]
 *
 * @note       none
 */
#ifndef CONTROLCLIENTSESSION_H
#define CONTROLCLIENTSESSION_H

#include "ControlClient.h"
#include "Session.h"
#include "Main.h"

struct CtrlClntSession
{
    struct CtrlClnt* ctrlClnt;

    int lastParsedSection;
    char msgType;
};

typedef struct CtrlClntSession CtrlClntSession;

void ctrlClntSessionInit(Session*, struct CtrlClnt*, SOCKET, sockaddr_in);
void ctrlClntSessionStartTest(Session*);

#endif
