#ifndef TESTSERVERSESSION_H
#define TESTSERVERSESSION_H

#include "ControlServerSession.h"
#include "Session.h"

struct TestSvrSession
{
    struct CtrlSvrSession* ctrlSvrSession;
    SYSTEMTIME startTime;
    SYSTEMTIME endTime;
};

void testSvrSessionInit(Session*, CtrlSvrSession*, SOCKET, sockaddr_in);

#endif
