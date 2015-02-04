#include "TestServer.h"

static void onConnectTCP(Server*, SOCKET, sockaddr_in);
static void onConnectUDP(Server*, SOCKET, sockaddr_in);
static void onError(Server*, int, int);
static void onClose(Server*, int);

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

// good
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

// good
static void onClose(Server* server, int code)
{
    char output[MAX_STRING_LEN];        // buffer for output

    // parse user pointer
    TestSvr* testSvr = (TestSvr*) server->usrPtr;

    // clean up...
    testSvr->ctrlSvrSession->testServer = 0;
    free(testSvr);
    free(server);
}
