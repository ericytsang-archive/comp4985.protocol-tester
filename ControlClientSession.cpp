#include "ControlClientSession.h"

static void handleMessage(Session* session, char* str, int len);
static void onMessage(Session*, char*, int);
static void onError(Session*, int, int);
static void onClose(Session*, int);

void ctrlClntSessionInit(Session* session, CtrlClnt* ctrlClnt, SOCKET clientSock, sockaddr_in clientAddr)
{
    sessionInit(session, &clientSock, &clientAddr);
    session->usrPtr    = malloc(sizeof(CtrlClntSession));
    session->onMessage = onMessage;
    session->onError   = onError;
    session->onClose   = onClose;
    sessionSetBufLen(session, PACKET_LEN_TYPE);

    // create control structure for client control session
    CtrlClntSession* ctrlClntSession = (CtrlClntSession*) session->usrPtr;
    ctrlClntSession->ctrlClnt          = ctrlClnt;
    ctrlClntSession->lastParsedSection = 0;
    ctrlClntSession->msgType           = 0;
}

// good
void ctrlClntSessionStartTest(Session* session)
{
    // parse user pointer
    CtrlClntSession* ctrlClntSession = (CtrlClntSession*) session->usrPtr;

    // send the test parameters through control to the server
    sessionSendCtrlMsg(session, MSG_SET_PORT,
        &ctrlClntSession->ctrlClnt->testPort,
        sizeof(ctrlClntSession->ctrlClnt->testPort));
    sessionSendCtrlMsg(session, MSG_SET_PROTOCOL,
        &ctrlClntSession->ctrlClnt->testProtocol,
        sizeof(ctrlClntSession->ctrlClnt->testProtocol));
    sessionSendCtrlMsg(session, MSG_SET_PKTSIZE,
        &ctrlClntSession->ctrlClnt->testPacketSize,
        sizeof(ctrlClntSession->ctrlClnt->testPacketSize));
    sessionSendCtrlMsg(session, MSG_SET_PKTCOUNT,
        &ctrlClntSession->ctrlClnt->packetsToSend,
        sizeof(ctrlClntSession->ctrlClnt->packetsToSend));
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
        appendWindowText(ctrlClntSession->ctrlClnt->clientWnds->hOutput,
            output);
        break;
    case MSG_START_TEST:
        ctrlClntConnectTest(ctrlClntSession->ctrlClnt->client);
        break;
    case MSG_STOP_TEST:
        sessionClose(ctrlClntSession->ctrlClnt->testSession);
        break;
    default:
        sprintf_s(output, "UNKNOWN MSG TYPE: %.*s\r\n", len, str);
        appendWindowText(ctrlClntSession->ctrlClnt->clientWnds->hOutput,
            output);
        break;
    }
}

// good
static void onMessage(Session* session, char* str, int len)
{
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
static void onError(Session* session, int errCode, int winErrCode)
{
    char output[MAX_STRING_LEN];    // temporary output buffer

    // parse user pointer
    CtrlClntSession* ctrlClntSession = (CtrlClntSession*) session->usrPtr;

    // print failure message to the screen
    sprintf_s(output, "Control encountered an error: %s - %d\r\n",
        rctoa(errCode), winErrCode);
    appendWindowText(ctrlClntSession->ctrlClnt->clientWnds->hOutput, output);
}

// good
static void onClose(Session* session, int code)
{
    char output[MAX_STRING_LEN];    // temporary output buffer

    // parse user pointer
    CtrlClntSession* ctrlClntSession = (CtrlClntSession*) session->usrPtr;

    // print failure message to the screen
    sprintf_s(output, "Control disconnected: %s\r\n", rctoa(code));
    appendWindowText(ctrlClntSession->ctrlClnt->clientWnds->hOutput, output);

    // clean up...
    ctrlClntSession->ctrlClnt->ctrlSession = 0;
    free(ctrlClntSession);
    free(session);
}
