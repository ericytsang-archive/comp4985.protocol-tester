/**
 * exposes the TestServerSession.coo interface.
 *
 * @sourceFile TestServerSession.h
 *
 * @program    ProtocolTester.exe
 *
 * @function   void testSvrSessionInit(Session*, struct CtrlSvrSession*, SOCKET,
 *   sockaddr_in);
 *
 * @date       2015-02-09
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 */
#ifndef TESTSERVERSESSION_H
#define TESTSERVERSESSION_H

#include "ControlServerSession.h"
#include "Session.h"

struct TestSvrSession
{
    struct CtrlSvrSession* ctrlSvrSession;
    SYSTEMTIME startTime;
    SYSTEMTIME endTime;
    HANDLE delayedStop;
};

void testSvrSessionInit(Session*, struct CtrlSvrSession*, SOCKET, sockaddr_in);

#endif
