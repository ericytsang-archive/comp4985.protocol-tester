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
    ctrlSvrSession->ctrlSessions      = &ctrlSvr->ctrlSessions;
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
    linkedListPrepend(&ctrlSvr->ctrlSessions, session);
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
    // parse user pointer
    CtrlSvr* ctrlSvr = (CtrlSvr*) server->usrPtr;

    // print stop message
    sprintf_s(output, "Server stopped - %s\r\n", rctoa(code));
    appendWindowText(ctrlSvr->serverWnds->hOutput, output);
}

void ctrlSvrStart(Server* server)
{
    char output[MAX_STRING_LEN];        // buffer for output
    char portString[MAX_STRING_LEN];    // buffer holding remote port
    unsigned short port;                // remote port in unsigned short form
    int returnCode;

    // parse user pointer
    CtrlSvr* ctrlSvr = (CtrlSvr*) server->usrPtr;

    // get and parse user input
    GetWindowText(ctrlSvr->serverWnds->hPort, portString, MAX_STRING_LEN);
    port = atoi(portString);

    // set the port and start the server
    serverSetPort(server, port);
    returnCode = serverStart(server);
    switch(serverStart(server))
    {
        case NORMAL_SUCCESS:
        {
            sprintf_s(output, "Server started; listening on port %d\r\n", port);
            appendWindowText(ctrlSvr->serverWnds->hOutput, output);
            break;
        }
        default:
        {
            sprintf_s(output, "Failed to start server: %s\r\n",
                rctoa(returnCode));
            appendWindowText(ctrlSvr->serverWnds->hOutput, output);
            break;
        }
    }
}

void ctrlSvrStop(Server* server)
{
    char output[MAX_STRING_LEN];    // temporary buffer for output
    int returnCode;

    // parse user pointer
    CtrlSvr* ctrlSvr = (CtrlSvr*) server->usrPtr;

    // stop the server
    returnCode = serverStop(server);
    switch(returnCode)
    {
        case NORMAL_SUCCESS:
            sprintf_s(output, "Server stopped\r\n");
            appendWindowText(ctrlSvr->serverWnds->hOutput, output);
            break;
        default:
            sprintf_s(output, "Failed to stop the Server: %s\r\n",
                rctoa(returnCode));
            appendWindowText(ctrlSvr->serverWnds->hOutput, output);
            break;
    }
}
