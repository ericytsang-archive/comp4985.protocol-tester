/**
 * control session on the client side.
 *
 * @sourceFile ControlClientSession.cpp
 *
 * @program    ProtocolTester.exe
 *
 * @signature  void ctrlClntSessionInit(Session* session, CtrlClnt* ctrlClnt,
 *   SOCKET clientSock, sockaddr_in clientAddr)
 * @signature  void ctrlClntSessionStartTest(Session* session)
 * @signature  static void handleMessage(Session* session, char* str, int len)
 * @signature  static void onMessage(Session* session, char* str, int len)
 * @signature  static void onError(Session* session, int errCode, int
 *   winErrCode)
 * @signature  static void onClose(Session* session, int code)
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
#include "ControlClientSession.h"

static void handleMessage(Session* session, char* str, int len);
static void onMessage(Session*, char*, int);
static void onError(Session*, int, int);
static void onClose(Session*, int);

/**
 * initializes the session structure
 *
 * @function   ctrlClntSessionInit
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
 * @signature  void ctrlClntSessionInit(Session* session, CtrlClnt* ctrlClnt,
 *   SOCKET clientSock, sockaddr_in clientAddr)
 *
 * @param      session pointer to the session structure to initialize
 * @param      ctrlClnt pointer to the control
 * @param      clientSock socket used to interface with the connection
 * @param      clientAddr structure containing the address of the remote host
 */
void ctrlClntSessionInit(Session* session, CtrlClnt* ctrlClnt,
    SOCKET clientSock, sockaddr_in clientAddr)
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

/**
 * starts the test session and performs the test
 *
 * @function   ctrlClntSessionStartTest
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
 * @signature  void ctrlClntSessionStartTest(Session* session)
 *
 * @param      session pointer to the session object that should start the test.
 */
void ctrlClntSessionStartTest(Session* session)
{
    char output[MAX_STRING_LEN];

    // parse user pointer
    CtrlClntSession* ctrlClntSession = (CtrlClntSession*) session->usrPtr;

    // do local validation
    if(ctrlClntSession->ctrlClnt->dataSource == MODE_UNDEFINED)
    {
        sprintf_s(output, "invalid data source; failed to start test\r\n");
        appendWindowText(ctrlClntSession->ctrlClnt->clientWnds->hOutput,
            output);
        return;
    }

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
    sessionSendCtrlMsg(session, MSG_SET_DATASRC,
        &ctrlClntSession->ctrlClnt->dataSource,
        sizeof(ctrlClntSession->ctrlClnt->dataSource));
    sessionSendCtrlMsg(session, MSG_START_TEST, "\0", 1);
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
        if(ctrlClntSession->ctrlClnt->testSession)
        {
            sessionClose(ctrlClntSession->ctrlClnt->testSession);
        }
        break;
    default:
        sprintf_s(output, "UNKNOWN MSG TYPE: %.*s\r\n", len, str);
        appendWindowText(ctrlClntSession->ctrlClnt->clientWnds->hOutput,
            output);
        break;
    }
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
 * @param      session pointer to the session object
 * @param      str pointer to the beginning of the payload
 * @param      len length of the payload
 */
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

/**
 * invoked when an error occurs regarding the session
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
 * @param      session pointer to the session structure that has the error
 * @param      errCode error code indicating the nature of the error
 * @param      winErrCode window's error code indicating the nature of the error
 */
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

/**
 * closes the control session and deallocates all the memory used by the
 *   session.
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
 * @note       none
 *
 * @signature  static void onClose(Session* session, int code)
 *
 * @param      session pointer to the session to close
 * @param      code return code indicating the nature of the close.
 */
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
