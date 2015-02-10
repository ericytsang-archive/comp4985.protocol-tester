/**
 * Eric Tsang
 *
 * @sourceFile ControlServerSession.h
 *
 * @program    ProtocolTester.exe
 *
 * @function   void ctrlSvrSessionInit(Session*, struct CtrlSvr*, SOCKET,
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
#ifndef CONTROLSERVERSESSION_H
#define CONTROLSERVERSESSION_H

#include "ControlClientSession.h"
#include "ControlServer.h"
#include "TestServer.h"
#include "LinkedList.h"
#include "Session.h"
#include "Server.h"
#include "Main.h"

#define MAX_STRING_LEN 1024

/**
 * structure pointed at by the user pointer of the control server session.
 *
 * keeps track of any currently active test session, or test server associated
 *   with this control session.
 *
 * also keeps track of statistics for the current test being run on the test
 *   session.
 */
struct CtrlSvrSession
{
    struct ServerWnds* serverWnds;
    struct CtrlSvr* ctrlSvr;

    struct Server* testServer;

    struct Session* testSession;
    struct Session* ctrlSession;

    unsigned short testPort;

    int testProtocol;
    int testPacketSize;
    double testPacketCount;
    long byteCount;

    int dataSink;
    int dataSource;

    int lastParsedSection;
    char msgType;
};

typedef struct CtrlSvrSession CtrlSvrSession;

void ctrlSvrSessionInit(Session*, struct CtrlSvr*, SOCKET, sockaddr_in);

#endif
