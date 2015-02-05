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
