#include "TestServerSession.h"

static long delay (SYSTEMTIME, SYSTEMTIME);
static void onMessage(Session*, char*, int);
static void onError(Session*, int, int);
static void onClose(Session*, int);
static DWORD WINAPI delayStopRoutine(void*);

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

    // close previous file handle, and repllace it with another if necessary
    char filePath[MAX_STRING_LEN];
    GetWindowText(testSvrSession->ctrlSvrSession->serverWnds->hFile, filePath, MAX_STRING_LEN);
    CloseHandle(testSvrSession->ctrlSvrSession->ctrlSvr->file);
    testSvrSession->ctrlSvrSession->ctrlSvr->file = CreateFile(filePath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, (LPSECURITY_ATTRIBUTES) NULL,
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE) NULL);

    // start a thread that will terminate this session if we dont receive another msg for some time
    DWORD useless;
    testSvrSession->delayedStop = CreateThread(NULL, 0, delayStopRoutine, testSvrSession, 0, &useless);

    // get test start time
    GetSystemTime(&testSvrSession->startTime);
    GetSystemTime(&testSvrSession->endTime);
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

    // write to a file if one is specified
    if(testSvrSession->ctrlSvrSession->ctrlSvr->file != INVALID_HANDLE_VALUE)
    {
        DWORD useless;
        SetFilePointer(testSvrSession->ctrlSvrSession->ctrlSvr->file, 0, 0, FILE_END);
        WriteFile(testSvrSession->ctrlSvrSession->ctrlSvr->file, str, len, &useless, NULL);
    }

    // send update about test progress to client every once in a while...
    if(++invocationCount % 100 == 0)
    {
        sprintf_s(output, "Bytes Received: %d", testSvrSession->ctrlSvrSession->byteCount);
        sessionSendCtrlMsg(testSvrSession->ctrlSvrSession->ctrlSession, MSG_CHAT, output, strlen(output));
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
                      "    Bytes Received: %lu\r\n"
                      "    Packets Received: %.2f / %.2f\r\n"
                      "    Round-trip Delay: %ld ms\r\n",
        testSvrSession->ctrlSvrSession->byteCount,
        testSvrSession->ctrlSvrSession->byteCount / (double) testSvrSession->ctrlSvrSession->testPacketSize,
        testSvrSession->ctrlSvrSession->testPacketCount,
        delay(testSvrSession->startTime, testSvrSession->endTime));
    sessionSendCtrlMsg(testSvrSession->ctrlSvrSession->ctrlSession, MSG_CHAT, output, strlen(output));
    sessionSendCtrlMsg(testSvrSession->ctrlSvrSession->ctrlSession, MSG_STOP_TEST, "a", 1);

    // clean up...
    TerminateThread(testSvrSession->delayedStop, 0);
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

static DWORD WINAPI delayStopRoutine(void* params)
{
    TestSvrSession* testSvrSession = (TestSvrSession*) params;
    SYSTEMTIME now;
    while(true)
    {
        Sleep(500);
        GetSystemTime(&now);
        if(delay(testSvrSession->endTime, now) > 5000)
        break;
    }
    sessionClose(testSvrSession->ctrlSvrSession->testSession);
    return 0;
}