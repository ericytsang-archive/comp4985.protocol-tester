#include "ControlClient.h"

static char output[MAX_STRING_LEN];

void ctrlClntOnConnect(Client* client, SOCKET clientSock, sockaddr_in clientAddr)
{
    // parse user parameters
    CtrlClnt* ctrlClnt = (CtrlClnt*) client->usrPtr;

    // print connected message
    sprintf_s(output, "Control connected to \"%s:%d\"\r\n",
        inet_ntoa(clientAddr.sin_addr),
        htons(clientAddr.sin_port));
    appendWindowText(ctrlClnt->clientWnds->hOutput, output);

    // create control structure for client control session
    CtrlClntSession* ctrlClntSession =
        (CtrlClntSession*) malloc(sizeof(CtrlClntSession));
    ctrlClntSession->clientWnds        = ctrlClnt->clientWnds;
    ctrlClntSession->ctrlSession       = ctrlClnt->ctrlSession;
    ctrlClntSession->testSession       = ctrlClnt->testSession;
    ctrlClntSession->lastParsedSection = 0;
    ctrlClntSession->msgType           = 0;
    ctrlClntSession->msgLen            = 0;

    // create and start the client control session
    Session* session = ctrlClnt->ctrlSession;
    sessionInit(session, &clientSock, &clientAddr);
    session->usrPtr     = ctrlClntSession;
    session->onMessage  = ctrlClntSessionOnMessage;
    session->onError    = ctrlClntSessionOnError;
    session->onClose    = ctrlClntSessionOnClose;
    sessionSetBufLen(session, PACKET_LEN_TYPE);
    sessionStart(session);
}

void ctrlClntOnError(Client* client, int errCode, int winErrCode)
{
    CtrlClntSession* ctrlClntSession = (CtrlClntSession*) client->usrPtr;
    sprintf_s(output, "Failed to connect: %s - %d\r\n",
        rctoa(errCode), winErrCode);
    appendWindowText(ctrlClntSession->clientWnds->hOutput, output);
}
