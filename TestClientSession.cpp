#include "TestClientSession.h"

static void onMessage(Session*, char*, int);
static void onError(Session*, int, int);
static void onClose(Session*, int);

void testClntSessionInit(Session* session, CtrlClnt* CtrlClnt, SOCKET clientSock, sockaddr_in clientAddr)
{
    sessionInit(session, &clientSock, &clientAddr);
    session->usrPtr     = malloc(sizeof(TestClntSession));
    session->onMessage  = onMessage;
    session->onError    = onError;
    session->onClose    = onClose;

    TestClntSession* testClntSession = (TestClntSession*) session->usrPtr;
    testClntSession->ctrlClnt = CtrlClnt;
    sessionSetBufLen(session, testClntSession->ctrlClnt->testPacketSize);
}

static void onMessage(Session* session, char* str, int len)
{

}

static void onError(Session* session, int errCode, int winErrCode)
{
    char output[MAX_STRING_LEN];        // buffer for output

    // parse user parameters
    TestClntSession* testClntSession = (TestClntSession*) session->usrPtr;

    // print error message...
    sprintf_s(output, "ERROR: %s\r\n", rctoa(errCode));
    appendWindowText(testClntSession->ctrlClnt->clientWnds->hOutput, output);

    // abort
    sessionClose(session);
}

static void onClose(Session* session, int closeCode)
{
    char output[MAX_STRING_LEN];        // buffer for output

    // parse user parameters
    TestClntSession* testClntSession = (TestClntSession*) session->usrPtr;

    // print stopped test message...
    sprintf_s(output, "Test session stopped: %s\r\n", rctoa(closeCode));
    appendWindowText(testClntSession->ctrlClnt->clientWnds->hOutput, output);

    // clean up...
    testClntSession->ctrlClnt->testSession = 0;
    free(testClntSession);
    free(session);
}
