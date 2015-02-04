#ifndef TESTSERVER_H
#define TESTSERVER_H

#include "ControlServerSession.h"
#include "TestServerSession.h"
#include "ControlServer.h"
#include "Server.h"
#include "Main.h"

struct TestSvr
{
    struct CtrlSvrSession* ctrlSvrSession;
    int packetCount;
};

void testSvrInit(Server*, struct CtrlSvrSession*);

#endif
