/**
 * contains the functions used by the control line session on the server side.
 *
 * @sourceFile ControlServerSession.cpp
 *
 * @program    ProtocolTester.exe
 *
 * @signature  void ctrlSvrSessionInit(Session* session, CtrlSvr* ctrlSvr,
 *   SOCKET clientSock, sockaddr_in clientAddr)
 * @signature  static void onMessage(Session* session, char* str, int len)
 * @signature  static void onError(Session* session, int errCode, int
 *   winErrCode)
 * @signature  static void onClose(Session* session, int code)
 * @signature  static void handleMessage(Session* session, char* str, int len)
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
#include "ControlServerSession.h"

static void handleMessage(Session*, char*, int);
static void onMessage(Session*, char*, int);
static void onError(Session*, int, int);
static void onClose(Session*, int);

/**
 * initializes the session structure on the server side for the control line.
 *
 * @function   ctrlSvrSessionInit
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
 *
 * @signature  void ctrlSvrSessionInit(Session* session, CtrlSvr* ctrlSvr,
 *   SOCKET clientSock, sockaddr_in clientAddr)
 *
 * @param      session pointer to the session structure
 * @param      ctrlSvr pointer to the server control structure that keeps track
 *   of all the active control sessions
 * @param      clientSock socket used to talk with the connection
 * @param      clientAddr structure containing information about the remote
 *   address
 */
void ctrlSvrSessionInit(Session* session, CtrlSvr* ctrlSvr, SOCKET clientSock, sockaddr_in clientAddr)
{
    sessionInit(session, &clientSock, &clientAddr);
    session->usrPtr     = malloc(sizeof(CtrlSvrSession));
    session->onMessage  = onMessage;
    session->onError    = onError;
    session->onClose    = onClose;
    sessionSetBufLen(session, PACKET_LEN_TYPE);

    CtrlSvrSession* ctrlSvrSession = (CtrlSvrSession*) session->usrPtr;
    ctrlSvrSession->ctrlSvr           = ctrlSvr;
    ctrlSvrSession->serverWnds        = ctrlSvr->serverWnds;
    ctrlSvrSession->testServer        = 0;
    ctrlSvrSession->testSession       = 0;
    ctrlSvrSession->ctrlSession       = session;
    ctrlSvrSession->testPort          = 0;
    ctrlSvrSession->testProtocol      = MODE_UNDEFINED;
    ctrlSvrSession->testPacketSize    = 0;
    ctrlSvrSession->byteCount         = 0;
    ctrlSvrSession->dataSink          = MODE_UNDEFINED;
    ctrlSvrSession->dataSource        = MODE_UNDEFINED;
    ctrlSvrSession->lastParsedSection = 0;
    ctrlSvrSession->msgType           = 0;
}

/**
 * invoked when the session receives a message.
 *
 * @function   onMessage
 *
 * @date       2015-02-09
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note
 *
 * since this is a control line session, it follows the control line protocol;
 *   this function is used to parse the control line packet.
 *
 * the first byte is specifies the type of the packet.
 *
 * the second 4 bytes specify the length of the payload.
 *
 * the last part of the packet is the payload. the size of the payload was
 *   specified in the previous segment of the packet.
 *
 * when a packet is successfully parsed, the handleMessage function is called.
 *
 * @signature  static void onMessage(Session* session, char* str, int len)
 *
 * @param      session pointer to the session structure
 * @param      str pointer to the beginning of the payload
 * @param      len specifies the length of the payload in bytes
 */
static void onMessage(Session* session, char* str, int len)
{
    // parse user parameters
    CtrlSvrSession* ctrlSvrSession = (CtrlSvrSession*) session->usrPtr;

    // parse packet
    switch(ctrlSvrSession->lastParsedSection % PACKET_SECTIONS)
    {
        case PACKET_SEQ_TYPE:       // parse packet header
            sessionSetBufLen(session, PACKET_LEN_LENGTH);
            ctrlSvrSession->msgType = str[0];
            break;
        case PACKET_SEQ_LENGTH:     // parse packet payload length
            sessionSetBufLen(session, *((int*) str));
            break;
        case PACKET_SEQ_PAYLOAD:    // parse packet payload & take action
            sessionSetBufLen(session, PACKET_LEN_TYPE);
            handleMessage(session, str, len);
            break;
    }

    if(++(ctrlSvrSession->lastParsedSection) >= PACKET_SECTIONS)
    {
        ctrlSvrSession->lastParsedSection -= PACKET_SECTIONS;
    }
}

/**
 * invoked when an error occurs regarding the session. this function reports the
 *   error to the screen.
 *
 * @function   onError
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
 *
 * @signature  static void onError(Session* session, int errCode, int
 *   winErrCode)
 *
 * @param      session pointer to the session structure.
 * @param      errCode return code indicating the nature of the error.
 * @param      winErrCode window's error code indicating the nature of the
 *   error.
 */
static void onError(Session* session, int errCode, int winErrCode)
{
    char output[MAX_STRING_LEN];        // buffer for output

    // parse user parameters
    CtrlSvrSession* ctrlSvrSession = (CtrlSvrSession*) session->usrPtr;

    // print the error to the screen
    sprintf_s(output, "%s:%d encountered an error: %s - %d\r\n",
        inet_ntoa(sessionGetIP(session)),
        htons(session->_remoteAddress.sin_port), rctoa(errCode), winErrCode);
    appendWindowText(ctrlSvrSession->serverWnds->hOutput, output);
}

/**
 * invoked when the session is being closed.
 *
 * @function   onClose
 *
 * @date       2015-02-09
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note
 *
 * this reports the disconnection to the user, and then removes the session from
 *   the set of control sessions on the server, then frees memory allocated for
 *   the session.
 *
 * @signature  static void onClose(Session* session, int code)
 *
 * @param      session pointer to the session object that's being closed
 * @param      code return code indicating the nature of the close.
 */
static void onClose(Session* session, int code)
{
    char output[MAX_STRING_LEN];        // buffer for output

    // parse user parameters
    CtrlSvrSession* ctrlSvrSession = (CtrlSvrSession*) session->usrPtr;

    // remove the control session from our set of sessions
    linkedListRemoveElement(&ctrlSvrSession->ctrlSvr->ctrlSessions, session);

    // print the close to the screen
    sprintf_s(output, "%s:%d disconnected: %s\r\n",
        inet_ntoa(sessionGetIP(session)),
        htons(session->_remoteAddress.sin_port), rctoa(code));
    appendWindowText(ctrlSvrSession->serverWnds->hOutput, output);

    // forward all other control sessions the same message
    Node* curr;
    for(curr = ctrlSvrSession->ctrlSvr->ctrlSessions.head; curr != 0;
        curr = curr->next)
    {
        sessionSendCtrlMsg((Session*) curr->data, MSG_CHAT, output,
            strlen(output)-2);
    }

    // clean up...
    if(ctrlSvrSession->testSession)
    {
        sessionClose(ctrlSvrSession->testSession);
        WaitForSingleObject(ctrlSvrSession->testSession->_sessionThread, 1000);
    }
    ctrlSvrSession->ctrlSession = 0;
    free(ctrlSvrSession);
    free(session);
}

/**
 * this function is invoked when a control line message has been parsed, and
 *   needs to be handled.
 *
 * @function   handleMessage
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
 *
 * @signature  static void handleMessage(Session* session, char* str, int len)
 *
 * @param      session pointer to the session object
 * @param      str pointer to the beginning of the payload data array
 * @param      len length of the payload data array.
 */
static void handleMessage(Session* session, char* str, int len)
{
    char output[MAX_STRING_LEN];        // buffer for output

    // parse user parameters
    CtrlSvrSession* ctrlSvrSession = (CtrlSvrSession*) session->usrPtr;

    switch(ctrlSvrSession->msgType)
    {
    case MSG_CHAT:
        sprintf_s(output, "%s:%d: %.*s\r\n",
            inet_ntoa(sessionGetIP(session)),
            htons(session->_remoteAddress.sin_port), len, str);
        appendWindowText(ctrlSvrSession->serverWnds->hOutput, output);

        // forward all control sessions the same message
        Node* curr;
        for(curr = ctrlSvrSession->ctrlSvr->ctrlSessions.head; curr != 0;
            curr = curr->next)
        {
            sessionSendCtrlMsg((Session*) curr->data, MSG_CHAT, output,
                strlen(output)-2);
        }
        break;
    case MSG_SET_PROTOCOL:
        ctrlSvrSession->testProtocol = *((int*) str);
        break;
    case MSG_SET_PORT:
        ctrlSvrSession->testPort = *((int*) str);
        break;
    case MSG_SET_PKTSIZE:
        ctrlSvrSession->testPacketSize = *((int*) str);
        break;
    case MSG_SET_PKTCOUNT:
        ctrlSvrSession->testPacketCount = *((double*) str);
        break;
    case MSG_SET_DATASRC:
        ctrlSvrSession->dataSource = *((int*) str);
        break;
    case MSG_START_TEST:
        // validate the current test parameters
        if(ctrlSvrSession->testSession || ctrlSvrSession->testServer)
        {
            sprintf_s(output, "test in progress; failed to begin test");
            sessionSendCtrlMsg(session, MSG_CHAT, output, strlen(output));
            break;
        }
        if(ctrlSvrSession->testPort == 0)
        {
            sprintf_s(output, "test port not defined; failed to begin test");
            sessionSendCtrlMsg(session, MSG_CHAT, output, strlen(output));
            break;
        }
        if(ctrlSvrSession->testProtocol == MODE_UNDEFINED)
        {
            sprintf_s(output, "protocol not defined; failed to begin test");
            sessionSendCtrlMsg(session, MSG_CHAT, output, strlen(output));
            break;
        }
        if(ctrlSvrSession->testPacketSize <= 0)
        {
            sprintf_s(output, "invalid packet size; failed to begin test");
            sessionSendCtrlMsg(session, MSG_CHAT, output, strlen(output));
            break;
        }
        if(ctrlSvrSession->dataSource = MODE_UNDEFINED)
        {
            sprintf_s(output, "data source not specified; failed to begin test");
            sessionSendCtrlMsg(session, MSG_CHAT, output, strlen(output));
            break;
        }
        if(ctrlSvrSession->dataSource == MODE_FROM_GENERATOR
            && ctrlSvrSession->testPacketCount <= 0)
        {
            sprintf_s(output, "invalid packet count; failed to begin test");
            sessionSendCtrlMsg(session, MSG_CHAT, output, strlen(output));
            break;
        }

        // try to begin the test
        ctrlSvrSession->byteCount = 0;
        if(ctrlSvrSession->testProtocol == MODE_TCP)
        {
            int returnCode;

            // create and start the TCP test server
            ctrlSvrSession->testServer = (Server*) malloc(sizeof(Server));
            testSvrInit(ctrlSvrSession->testServer, ctrlSvrSession);
            serverSetPort(ctrlSvrSession->testServer, ctrlSvrSession->testPort);
            returnCode = serverStart(ctrlSvrSession->testServer);

            switch(returnCode)
            {
            case NORMAL_SUCCESS:    // test started
                sprintf_s(output, "TCP test started");
                sessionSendCtrlMsg(session, MSG_CHAT, output, strlen(output));
                sessionSendCtrlMsg(session, MSG_START_TEST, "\0", 1);
                break;
            default:                // failed; tell client
                sprintf_s(output, "Failed to start test: %s",
                    rctoa(returnCode));
                sessionSendCtrlMsg(session, MSG_CHAT, output, strlen(output));
                break;
            }
        }
        if(ctrlSvrSession->testProtocol == MODE_UDP)
        {
            int returnCode;

            // create and start the UDP test server
            ctrlSvrSession->testServer = (Server*) malloc(sizeof(Server));
            testSvrInit(ctrlSvrSession->testServer, ctrlSvrSession);
            returnCode = serverOpenUDPPort(ctrlSvrSession->testServer,
                ctrlSvrSession->testPort);

            switch(returnCode)
            {
            case NORMAL_SUCCESS:    // test started
                sprintf_s(output, "UDP test started");
                sessionSendCtrlMsg(session, MSG_CHAT, output, strlen(output));
                sessionSendCtrlMsg(session, MSG_START_TEST, "\0", 1);
                break;
            default:                // failed; tell client
                sprintf_s(output, "Failed to start test: %s", rctoa(returnCode));
                sessionSendCtrlMsg(session, MSG_CHAT, output, strlen(output));
                break;
            }
        }
        break;
    case MSG_STOP_TEST:
        sprintf_s(output, "received MSG_STOP_TEST");
        sessionSendCtrlMsg(session, MSG_CHAT, output, strlen(output));
        if(ctrlSvrSession->testSession)
        {
            sessionClose(ctrlSvrSession->testSession);
        }
        break;
    default:
        sprintf_s(output, "UNKOWN MSG TYPE%s:%d: %.*s\r\n",
            inet_ntoa(sessionGetIP(session)),
            htons(session->_remoteAddress.sin_port), len, &str[0]);
        appendWindowText(ctrlSvrSession->serverWnds->hOutput, output);
        break;
    }
}
