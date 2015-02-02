#include "ControlServerSession.h"

static char output[MAX_STRING_LEN];

static void handleMessage(Session* session, char* str, int len);

void svrCtrlSessionOnMessage(Session* session, char* str, int len)
{
    // parse user parameters
    CtrlSvrSession* ctrlSvrSession = (CtrlSvrSession*) session->usrPtr;

    // parse packet
    switch(ctrlSvrSession->lastParsedSection % PACKET_SECTIONS)
    {
        case PACKET_SEQ_TYPE:       // parse packet header
        {
            sessionSetBufLen(session, PACKET_LEN_LENGTH);
            ctrlSvrSession->msgType = str[0];
            break;
        }
        case PACKET_SEQ_LENGTH:     // parse packet payload length
        {
            sessionSetBufLen(session, *((int*) str));
            break;
        }
        case PACKET_SEQ_PAYLOAD:    // parse packet payload & take action
        {
            sessionSetBufLen(session, PACKET_LEN_TYPE);
            handleMessage(session, str, len);
            break;
        }
    }

    if(++(ctrlSvrSession->lastParsedSection) >= PACKET_SECTIONS)
    {
        ctrlSvrSession->lastParsedSection -= PACKET_SECTIONS;
    }
}

void svrCtrlSessionOnError(Session* session, int errCode, int winErrCode)
{
    // parse user parameters
    CtrlSvrSession* ctrlSvrSession = (CtrlSvrSession*) session->usrPtr;

    // print the error to the screen
    sprintf_s(output, "%s:%d encountered an error: %s - %d\r\n",
        inet_ntoa(sessionGetIP(session)),
        htons(session->_remoteAddress.sin_port), rctoa(errCode), winErrCode);
    appendWindowText(ctrlSvrSession->serverWnds->hOutput, output);
}

void svrCtrlSessionOnClose(Session* session, int code)
{
    // parse user parameters
    CtrlSvrSession* ctrlSvrSession = (CtrlSvrSession*) session->usrPtr;

    // print the close to the screen
    sprintf_s(output, "%s:%d disconnected: %s\r\n",
        inet_ntoa(sessionGetIP(session)),
        htons(session->_remoteAddress.sin_port), rctoa(code));
    appendWindowText(ctrlSvrSession->serverWnds->hOutput, output);

    // clean up...
    linkedListRemoveElement(ctrlSvrSession->ctrlSessions, session);
    serverStop(&ctrlSvrSession->testServer);
    free(ctrlSvrSession);
}

static void handleMessage(Session* session, char* str, int len)
{
    // parse user parameters
    CtrlSvrSession* ctrlSvrSession = (CtrlSvrSession*) session->usrPtr;

    switch(ctrlSvrSession->msgType)
    {
        case MSG_CHAT:
        {
            sprintf_s(output, "%s:%d: %.*s\r\n",
                inet_ntoa(sessionGetIP(session)),
                htons(session->_remoteAddress.sin_port), len, str);
            appendWindowText(ctrlSvrSession->serverWnds->hOutput, output);

            // forward all control sessions the same message
            Node* curr;
            for(curr = ctrlSvrSession->ctrlSessions->head; curr != 0; curr = curr->next)
            {
                char msgType = MSG_CHAT;
                sessionSend(session, &msgType, PACKET_LEN_TYPE);
                sessionSend(session, &len, PACKET_LEN_LENGTH);
                sessionSend(session, str, len);
            }
            break;
        }
        case MSG_SET_PROTOCOL:
        {
            ctrlSvrSession->testProtocol = *((int*) str);
            break;
        }
        case MSG_SET_PORT:
        {
            ctrlSvrSession->testPort = *((int*) str);
            break;
        }
        case MSG_START_TEST:
        {
            if(ctrlSvrSession->testProtocol == MODE_TCP)
            {
                str = "TCP test started";
                len = strlen(str);
                char msgType = MSG_CHAT;
                sessionSend(session, &msgType, PACKET_LEN_TYPE);
                sessionSend(session, &len, PACKET_LEN_LENGTH);
                sessionSend(session, str, len);
                // serverInit(&ctrlSvrSession->testServer);
                // ctrlSvrSession->testServer.usrPtr    = ctrlSvrSession;
                // ctrlSvrSession->testServer.onConnect = testServerOnConnect;
                // ctrlSvrSession->testServer.onError   = testServerOnError;
                // ctrlSvrSession->testServer.onClose   = testServerOnClose;
                // serverSetPort(&ctrlSvrSession->testServer, ctrlSvrSession->testPort);
                // serverStart(&ctrlSvrSession->testServer);
            }
            if(ctrlSvrSession->testProtocol == MODE_UDP)
            {
                str = "UDP test started";
                len = strlen(str);
                char msgType = MSG_CHAT;
                sessionSend(session, &msgType, PACKET_LEN_TYPE);
                sessionSend(session, &len, PACKET_LEN_LENGTH);
                sessionSend(session, str, len);
                // serverInit(&ctrlSvrSession->testServer);
                // ctrlSvrSession->testServer.usrPtr    = ctrlSvrSession;
                // ctrlSvrSession->testServer.onConnect = testServerOnConnect;
                // ctrlSvrSession->testServer.onError   = testServerOnError;
                // ctrlSvrSession->testServer.onClose   = testServerOnClose;
                // serverOpenUDPPort(&ctrlSvrSession->testServer, ctrlSvrSession->testPort);
            }
            if(ctrlSvrSession->testProtocol == MODE_UNDEFINED)
            {
                str = "protocol not defined; failed to begin test";
                len = strlen(str);
                char msgType = MSG_CHAT;
                sessionSend(session, &msgType, PACKET_LEN_TYPE);
                sessionSend(session, &len, PACKET_LEN_LENGTH);
                sessionSend(session, str, len);
            }
            break;
        }
        default:
        {
            sprintf_s(output, "UNKOWN MSG TYPE%s:%d: %.*s\r\n",
                inet_ntoa(sessionGetIP(session)),
                htons(session->_remoteAddress.sin_port), len, &str[0]);
            appendWindowText(ctrlSvrSession->serverWnds->hOutput, output);
            break;
        }
    }
}
