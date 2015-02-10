/**
 * exposes the interface functions of the TestServer.cpp
 *
 * @sourceFile TestServer.h
 *
 * @program    ProtocolTester.exe
 *
 * @function   void testSvrInit(Server*, struct CtrlSvrSession*);
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
