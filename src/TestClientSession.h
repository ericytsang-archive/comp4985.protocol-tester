#ifndef TESTCLIENTSESSION_H
#define TESTCLIENTSESSION_H

#include "ControlClient.h"
#include "Session.h"

struct TestClntSession
{
    struct CtrlClnt* ctrlClnt;
};

void testClntSessionInit(Session*, struct CtrlClnt*, SOCKET, sockaddr_in);

#endif
