#include "TestServerSession.h"

static long delay (SYSTEMTIME, SYSTEMTIME);
static void onMessage(Session*, char*, int);
static void onError(Session*, int, int);
static void onClose(Session*, int);

void testSvrSessionInit(Session* session, CtrlSvrSession* ctrlSvrSession, SOCKET clientSock, sockaddr_in clientAddr)
{
    sessionInit(session, &clientSock, &clientAddr);
    session->usrPtr     = malloc(sizeof(TestSvrSession));
    session->onMessage  = onMessage;
    session->onError    = onError;
    session->onClose    = onClose;

    TestSvrSession* testSvrSession = (TestSvrSession*) session->usrPtr;
    testSvrSession->ctrlSvrSession = ctrlSvrSession;
    sessionSetBufLen(session, testSvrSession->ctrlSvrSession->testPacketSize);

    // get test start time
    GetSystemTime(&testSvrSession->startTime);
}

static void onMessage(Session* session, char* str, int len)
{
    char output[MAX_STRING_LEN];
    static int invocationCount = 0;

    // parse user parameters
    TestSvrSession* testSvrSession = (TestSvrSession*) session->usrPtr;

    // update the byte count...
    testSvrSession->ctrlSvrSession->byteCount += len;

    // update test end time.
    GetSystemTime(&testSvrSession->endTime);

    // send an update to the client every once in a while...
    if(++invocationCount % 100 == 0)
    {
        sprintf_s(output, "bytesReceived: %d", testSvrSession->ctrlSvrSession->byteCount);
        sessionSendCtrlMsg(testSvrSession->ctrlSvrSession->ctrlSession, MSG_CHAT, output, strlen(output));
    }

    // end the test session if we know we got all the packets
    if(testSvrSession->ctrlSvrSession->byteCount == testSvrSession->ctrlSvrSession->testPacketCount * testSvrSession->ctrlSvrSession->testPacketSize)
    {
        sessionClose(session);
    }
}

static void onError(Session* session, int errCode, int winErrCode)
{
    char output[MAX_STRING_LEN];

    // parse user parameters
    TestSvrSession* testSvrSession = (TestSvrSession*) session->usrPtr;

    // print the error
    sprintf_s(output, "ERROR: %s - %d", rctoa(errCode), winErrCode);
    sessionSendCtrlMsg(testSvrSession->ctrlSvrSession->ctrlSession, MSG_CHAT, output, strlen(output));

    // abort
    sessionClose(session);
}

static void onClose(Session* session, int closeCode)
{
    char output[MAX_STRING_LEN];

    // parse user parameters
    TestSvrSession* testSvrSession = (TestSvrSession*) session->usrPtr;

    // send statistics to the client
    sprintf_s(output, "FINISHED!\r\n"
                      "    Bytes Received: %d\r\n"
                      "    Packets Received: %d / %d\r\n"
                      "    Round-trip Delay: %ld ms\r\n",
        testSvrSession->ctrlSvrSession->byteCount,
        testSvrSession->ctrlSvrSession->byteCount / testSvrSession->ctrlSvrSession->testPacketSize,
        testSvrSession->ctrlSvrSession->testPacketCount,
        delay(testSvrSession->startTime, testSvrSession->endTime));
    sessionSendCtrlMsg(testSvrSession->ctrlSvrSession->ctrlSession, MSG_CHAT, output, strlen(output));

    // clean up...
    testSvrSession->ctrlSvrSession->testSession = 0;
    free(testSvrSession);
    free(session);
}

// Compute the delay between tl and t2 in milliseconds
static long delay (SYSTEMTIME t1, SYSTEMTIME t2)
{
    long d;

    d = (t2.wSecond - t1.wSecond) * 1000;
    d += (t2.wMilliseconds - t1.wMilliseconds);
    return(d);
}
