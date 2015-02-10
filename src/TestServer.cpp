/**
 * test server. this server is started to let the client's test client connect
 *   so that the test session can begin, and statistics can be gathered.
 *
 * @sourceFile TestServer.cpp
 *
 * @program    ProtocolTester.exe
 *
 * @function   void testSvrInit(Server* server, CtrlSvrSession* ctrlSvrSession)
 * @function   static void onConnectTCP(Server* server, SOCKET clientSock,
 *   sockaddr_in clientAddr)
 * @function   static void onConnectUDP(Server* server, SOCKET clientSock,
 *   sockaddr_in clientAddr)
 * @function   static void onError(Server* server, int errCode, int winErrCode)
 * @function   static void onClose(Server* server, int code)
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
#include "TestServer.h"

static void onConnectTCP(Server*, SOCKET, sockaddr_in);
static void onConnectUDP(Server*, SOCKET, sockaddr_in);
static void onError(Server*, int, int);
static void onClose(Server*, int);

/**
 * initializes and assigns the callback functions for the server expecting the
 *   test connection.
 *
 * @function   testSvrInit
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
 * @signature  void testSvrInit(Server* server, CtrlSvrSession* ctrlSvrSession)
 *
 * @param      server pointer to the server structure to initialize.
 * @param      ctrlSvrSession pointer to the control server structure that this
 *   test server is testing for.
 */
void testSvrInit(Server* server, CtrlSvrSession* ctrlSvrSession)
{
    serverInit(server);
    server->usrPtr    = malloc(sizeof(TestSvr));
    server->onConnect = (ctrlSvrSession->testProtocol == MODE_TCP)
        ? onConnectTCP : onConnectUDP;
    server->onError   = onError;
    server->onClose   = onClose;

    TestSvr* testSvr = (TestSvr*) server->usrPtr;
    testSvr->ctrlSvrSession = ctrlSvrSession;
    testSvr->packetCount    = 0;
}

/**
 * invoked when the client's test client connects with the server for TCP
 *   testing.
 *
 * @function   onConnectTCP
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
 * the difference between onConnectTCP and onConnectUDP is the last statement.
 *   when we clean up after making a test connection, we want to stop the
 *   server, and close the listening socket, because we only want one test
 *   connection.
 *
 * this function does the following:
 *
 * 1. informs the client through the control line that a connection with the
 *   test server has been established
 *
 * 2. on the server side, creates a session object used to receive the packets
 *   from the new connection.
 *
 * 3. stops the test server, so no new connections can be established.
 *
 * @signature  static void onConnectTCP(Server* server, SOCKET clientSock,
 *   sockaddr_in clientAddr)
 *
 * @param      server pointer to the server structure that is the test server
 * @param      clientSock socket to interface with the new connection
 * @param      clientAddr structure containing remote address information
 */
static void onConnectTCP(Server* server, SOCKET clientSock, sockaddr_in clientAddr)
{
    char output[MAX_STRING_LEN];        // buffer for output

    // parse user parameters
    TestSvr* testSvr = (TestSvr*) server->usrPtr;

    // print connected message
    sprintf_s(output, "Test started!",
        inet_ntoa(clientAddr.sin_addr), htons(clientAddr.sin_port));
    sessionSendCtrlMsg(testSvr->ctrlSvrSession->ctrlSession,
        MSG_CHAT, output, strlen(output));

    // create a session, and send it to the ControlServerSession
    testSvr->ctrlSvrSession->testSession = (Session*) malloc(sizeof(Session));
    testSvrSessionInit(testSvr->ctrlSvrSession->testSession,
        testSvr->ctrlSvrSession, clientSock, clientAddr);
    sessionStart(testSvr->ctrlSvrSession->testSession);

    // close the server
    serverStop(server);
}

/**
 * invoked when the client's test client connects with the server for UDP
 *   testing.
 *
 * @function   onConnectUDP
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
 * the difference between onConnectTCP and onConnectUDP is the last statement.
 *   on UDP, the server was never started for listening, but we still want to
 *   clean up the server structures, since we don't need it anymore, that's why
 *   we just invoke the onClose at the end of the function here.
 *
 * this function does the following:
 *
 * 1. informs the client through the control line that a connection with the
 *   test server has been established
 *
 * 2. on the server side, creates a session object used to receive the packets
 *   from the new connection.
 *
 * 3. stops the test server, so no new connections can be established.
 *
 * @signature  static void onConnectUDP(Server* server, SOCKET clientSock,
 *   sockaddr_in clientAddr)
 *
 * @param      server pointer to server structure
 * @param      clientSock socket used to communicate with the new connection
 * @param      clientAddr structure containing information about the remote host
 */
static void onConnectUDP(Server* server, SOCKET clientSock, sockaddr_in clientAddr)
{
    char output[MAX_STRING_LEN];        // buffer for output

    // parse user parameters
    TestSvr* testSvr = (TestSvr*) server->usrPtr;

    // print connected message
    sprintf_s(output, "Test started!",
        inet_ntoa(clientAddr.sin_addr), htons(clientAddr.sin_port));
    sessionSendCtrlMsg(testSvr->ctrlSvrSession->ctrlSession,
        MSG_CHAT, output, strlen(output));

    // create a session, and send it to the ControlServerSession
    testSvr->ctrlSvrSession->testSession = (Session*) malloc(sizeof(Session));
    testSvrSessionInit(testSvr->ctrlSvrSession->testSession,
        testSvr->ctrlSvrSession, clientSock, clientAddr);
    sessionStart(testSvr->ctrlSvrSession->testSession);

    // close the server
    onClose(server, NORMAL_SUCCESS);
}

/**
 * if the server encounters an error, report it.
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
 * @signature  static void onError(Server* server, int errCode, int winErrCode)
 *
 * @param      server pointer to the server structure.
 * @param      errCode return code indicating the nature of the error.
 * @param      winErrCode window's return code indicating the nature of the
 *   error.
 */
static void onError(Server* server, int errCode, int winErrCode)
{
    char output[MAX_STRING_LEN];        // buffer for output

    // parse user parameters
    TestSvr* testSvr = (TestSvr*) server->usrPtr;

    // print error message
    sprintf_s(output, "Server encountered an error: %s - %d; test failed!",
        rctoa(errCode), winErrCode);
    sessionSendCtrlMsg(testSvr->ctrlSvrSession->ctrlSession,
        MSG_CHAT, output, strlen(output));
    sessionSendCtrlMsg(testSvr->ctrlSvrSession->ctrlSession,
        MSG_STOP_TEST, "a", 1);
}

/**
 * this function is invoked when the server is closing. this function cleans up
 *   for the server, and frees used memory.
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
 * @signature  static void onClose(Server* server, int code)
 *
 * @param      server pointer to the server structure that's closing
 * @param      code return code indicating the reason for the close.
 */
static void onClose(Server* server, int code)
{
    // parse user pointer
    TestSvr* testSvr = (TestSvr*) server->usrPtr;

    // clean up...
    testSvr->ctrlSvrSession->testServer = 0;
    free(testSvr);
    free(server);
}
