#include "ControlClientSession.h"

static void handleMessage(Session* session, char* str, int len);

void ctrlClntSessionInit(Session* session, CtrlClnt* ctrlClnt, SOCKET clientSock, sockaddr_in clientAddr)
{
    // create control structure for client control session
    CtrlClntSession* ctrlClntSession =
        (CtrlClntSession*) malloc(sizeof(CtrlClntSession));
    ctrlClntSession->clientWnds        = ctrlClnt->clientWnds;
    ctrlClntSession->ctrlSession       = &ctrlClnt->ctrlSession;
    ctrlClntSession->testSession       = &ctrlClnt->testSession;
    ctrlClntSession->ctrlClnt          = ctrlClnt;
    ctrlClntSession->lastParsedSection = 0;
    ctrlClntSession->msgType           = 0;

    // create and start the client control session
    Session* session = &ctrlClnt->ctrlSession;
    sessionInit(session, &clientSock, &clientAddr);
    session->usrPtr    = ctrlClntSession;
    session->onMessage = ctrlClntSessionOnMessage;
    session->onError   = ctrlClntSessionOnError;
    session->onClose   = ctrlClntSessionOnClose;
    sessionSetBufLen(session, PACKET_LEN_TYPE);
}

// good
void ctrlClntSessionOnMessage(Session* session, char* str, int len)
{
    char output[MAX_STRING_LEN];

    // parse user parameters
    CtrlClntSession* ctrlClntSession = (CtrlClntSession*) session->usrPtr;

    // parse packet
    switch(ctrlClntSession->lastParsedSection % PACKET_SECTIONS)
    {
        case PACKET_SEQ_TYPE:       // parse packet header
            sessionSetBufLen(session, PACKET_LEN_LENGTH);
            ctrlClntSession->msgType = str[0];
            break;
        case PACKET_SEQ_LENGTH:     // parse packet payload length
            sessionSetBufLen(session, *((int*) str));
            break;
        case PACKET_SEQ_PAYLOAD:    // parse packet payload & take action
            sessionSetBufLen(session, PACKET_LEN_TYPE);
            handleMessage(session, str, len);
            break;
    }

    if(++(ctrlClntSession->lastParsedSection) >= PACKET_SECTIONS)
    {
        ctrlClntSession->lastParsedSection -= PACKET_SECTIONS;
    }
}

// good
void ctrlClntSessionOnError(Session* session, int errCode, int winErrCode)
{
    char output[MAX_STRING_LEN];    // temporary output buffer

    // parse user pointer
    CtrlClntSession* ctrlClntSession = (CtrlClntSession*) session->usrPtr;

    // print failure message to the screen
    sprintf_s(output, "Control encountered an error: %s - %d\r\n",
        rctoa(errCode), winErrCode);
    appendWindowText(ctrlClntSession->clientWnds->hOutput, output);
}

// good
void ctrlClntSessionOnClose(Session* session, int code)
{
    char output[MAX_STRING_LEN];    // temporary output buffer

    // parse user pointer
    CtrlClntSession* ctrlClntSession = (CtrlClntSession*) session->usrPtr;

    // print failure message to the screen
    sprintf_s(output, "Control disconnected: %s\r\n", rctoa(code));
    appendWindowText(ctrlClntSession->clientWnds->hOutput, output);

    // clean up...
    free(ctrlClntSession);
}

// good
void ctrlClntSessionStartTest(Session* session)
{
    char port[MAX_STRING_LEN];

    // parse user pointer
    CtrlClntSession* ctrlClntSession = (CtrlClntSession*) session->usrPtr;

    // get test parameters from user input
    GetWindowText(ctrlClntSession->clientWnds->hTestPort, port, MAX_STRING_LEN);
    ctrlClntSession->ctrlClnt->testPort = atoi(port);

    // send the test parameters through control to the server
    sessionSendCtrlMsg(session, MSG_SET_PORT,
        &ctrlClntSession->ctrlClnt->testPort,
        sizeof(ctrlClntSession->ctrlClnt->testPort));
    sessionSendCtrlMsg(session, MSG_SET_PROTOCOL,
        &ctrlClntSession->ctrlClnt->testProtocol,
        sizeof(ctrlClntSession->ctrlClnt->testProtocol));
    sessionSendCtrlMsg(session, MSG_START_TEST, "\0", 1);
}

static void handleMessage(Session* session, char* str, int len)
{
    char output[MAX_STRING_LEN];    // temporary output buffer

    // parse user pointer
    CtrlClntSession* ctrlClntSession = (CtrlClntSession*) session->usrPtr;

    // handle the message depending on type
    switch(ctrlClntSession->msgType)
    {
        case MSG_CHAT:
            sprintf_s(output, "Control: %.*s\r\n", len, str);
            appendWindowText(ctrlClntSession->clientWnds->hOutput,
                output);
            break;
        default:
            sprintf_s(output, "UNKNOWN MSG TYPE: %.*s\r\n", len, str);
            appendWindowText(ctrlClntSession->clientWnds->hOutput,
                output);
            break;
    }
}
