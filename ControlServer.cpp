#include "ControlServer.h"

static char output[MAX_STRING_LEN];

void ctrlSvrOnConnect(Server* server, SOCKET clientSock, sockaddr_in clientAddr)
{
    // parse user parameters
    CtrlSvr* ctrlSvr = (CtrlSvr*) server->usrPtr;

    // print connected message
    sprintf_s(output, "%s:%d connected\r\n", inet_ntoa(clientAddr.sin_addr),
        htons(clientAddr.sin_port));
    appendWindowText(ctrlSvr->serverWnds->hOutput, output);

    // creating user structure for control server session
    CtrlSvrSession* ctrlSvrSession =
        (CtrlSvrSession*) malloc(sizeof(CtrlSvrSession));
    ctrlSvrSession->serverWnds        = ctrlSvr->serverWnds;
    ctrlSvrSession->ctrlSessions      = ctrlSvr->ctrlSessions;
    ctrlSvrSession->testProtocol      = MODE_UNDEFINED;
    ctrlSvrSession->testPort          = 0;
    ctrlSvrSession->lastParsedSection = 0;
    ctrlSvrSession->msgType           = 0;
    serverInit(&ctrlSvrSession->testServer);

    // create and start the control server session
    Session* session = (Session*) malloc(sizeof(Session));
    sessionInit(session, &clientSock, &clientAddr);
    session->usrPtr     = ctrlSvrSession;
    session->onMessage  = svrCtrlSessionOnMessage;
    session->onError    = svrCtrlSessionOnError;
    session->onClose    = svrCtrlSessionOnClose;
    sessionSetBufLen(session, PACKET_LEN_TYPE);
    sessionStart(session);

    // add the session to out list of control server sessions
    linkedListPrepend(ctrlSvr->ctrlSessions, session);
}

void ctrlSvrOnError(Server* server, int errCode, int winErrCode)
{
    // parse user parameters
    CtrlSvr* ctrlSvr = (CtrlSvr*) server->usrPtr;

    // print error message
    sprintf_s(output, "Server encountered an error: %s - %d\r\n",
        rctoa(errCode), winErrCode);
    appendWindowText(ctrlSvr->serverWnds->hOutput, output);
}

void ctrlSvrOnClose(Server* server, int code)
{
    // parse user parameters
    CtrlSvr* ctrlSvr = (CtrlSvr*) server->usrPtr;

    // print stop message
    sprintf_s(output, "Server stopped - %s\r\n", rctoa(code));
    appendWindowText(ctrlSvr->serverWnds->hOutput, output);
}
