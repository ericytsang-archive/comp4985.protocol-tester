/**
 * exposes the interface of TestClientSession.cpp
 *
 * @sourceFile TestClientSession.h
 *
 * @program    ProtocolTester.exe
 *
 * @function   void testClntSessionInit(Session*, struct CtrlClnt*, SOCKET,
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
