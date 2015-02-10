/**
 * test server session. the session functions used for the test connection on
 *   the server.
 *
 * @sourceFile TestServerSession.cpp
 *
 * @program    ProtocolTester.exe
 *
 * @function  void testSvrSessionInit(Session* session, CtrlSvrSession*
 *   ctrlSvrSession, SOCKET clientSock, sockaddr_in clientAddr)
 * @function  static void onMessage(Session* session, char* str, int len)
 * @function  static void onError(Session* session, int errCode, int winErrCode)
 * @function  static void onClose(Session* session, int closeCode)
 * @function  static DWORD WINAPI delayStopRoutine(void* params)
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
#include "TestServerSession.h"

static long delay (SYSTEMTIME, SYSTEMTIME);
static void onMessage(Session*, char*, int);
static void onError(Session*, int, int);
static void onClose(Session*, int);
static DWORD WINAPI delayStopRoutine(void*);

/**
 * initializes a session structure on the server used to talk to the test
 *   connection.
 *
 * @function   testSvrSessionInit
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
 * @signature  void testSvrSessionInit(Session* session, CtrlSvrSession* ctrlSvrSession, SOCKET clientSock, sockaddr_in clientAddr)
 *
 * @param      session pointer to the session structure
 * @param      ctrlSvrSession pointer to the control server session that's using
 *   this test session
 * @param      clientSock socket used to talk with the test connection
 * @param      clientAddr remote address.
 */
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
    GetWindowText(testSvrSession->ctrlSvrSession->serverWnds->hFile, filePath,
        MAX_STRING_LEN);
    CloseHandle(testSvrSession->ctrlSvrSession->ctrlSvr->file);
    testSvrSession->ctrlSvrSession->ctrlSvr->file = CreateFile(filePath,
        GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
        (LPSECURITY_ATTRIBUTES) NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
        (HANDLE) NULL);

    // start a thread that will terminate this session if we dont receive
    // another msg for some time
    DWORD useless;
    testSvrSession->delayedStop = CreateThread(NULL, 0, delayStopRoutine,
        testSvrSession, 0, &useless);

    // get test start time
    GetSystemTime(&testSvrSession->startTime);
    GetSystemTime(&testSvrSession->endTime);
}

/**
 * invoked when the test session received a packet.
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
 * this function is invoked whenever we receive a packet from the client's test
 *   session; here, we update the byte count, last received packet time, and
 *   write the received packet to a file. every 100 packets, we would send a
 *   message back to the client, informing them of how many bytes we have
 *   received so far.
 *
 * @signature  static void onMessage(Session* session, char* str, int len)
 *
 * @param      session pointer to the session structure
 * @param      str pointer to the first byte in the array of bytes received from
 *   the session.
 * @param      len this is the length of the byte array from the parameter str.
 */
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
        SetFilePointer(testSvrSession->ctrlSvrSession->ctrlSvr->file, 0, 0,
            FILE_END);
        WriteFile(testSvrSession->ctrlSvrSession->ctrlSvr->file, str, len,
            &useless, NULL);
    }

    // send update about test progress to client every once in a while...
    if(++invocationCount % 100 == 0)
    {
        sprintf_s(output, "Bytes Received: %d",
            testSvrSession->ctrlSvrSession->byteCount);
        sessionSendCtrlMsg(testSvrSession->ctrlSvrSession->ctrlSession,
            MSG_CHAT, output, strlen(output));
    }
}

/**
 * invoked when an error occurs having to do with the test session.
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
 * @note
 *
 * when an error occurs, this function is called.
 *
 * this function reports the error to the client through the control line, and
 *   then closes this session.
 *
 * @signature  static void onError(Session* session, int errCode, int
 *   winErrCode)
 *
 * @param      session pointer to the session structure
 * @param      errCode return code indicating the nature of the error
 * @param      winErrCode error code from windows, indicating the nature of the
 *   error.
 */
static void onError(Session* session, int errCode, int winErrCode)
{
    char output[MAX_STRING_LEN];

    // parse user parameters
    TestSvrSession* testSvrSession = (TestSvrSession*) session->usrPtr;

    // print the error
    sprintf_s(output, "ERROR: %s - %d", rctoa(errCode), winErrCode);
    sessionSendCtrlMsg(testSvrSession->ctrlSvrSession->ctrlSession, MSG_CHAT,
        output, strlen(output));

    // abort
    sessionClose(session);
}

/**
 * invoked when the session is closed. calculates the statistics of the test,
 *   and sends it back to the client through a chat message, and then frees
 *   allocated memory.
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
 * @signature  static void onClose(Session* session, int closeCode)
 *
 * @param      session pointer to the session structure
 * @param      closeCode reason for why the test session ended
 */
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
        testSvrSession->ctrlSvrSession->byteCount / (double)
        testSvrSession->ctrlSvrSession->testPacketSize,
        testSvrSession->ctrlSvrSession->testPacketCount,
        delay(testSvrSession->startTime, testSvrSession->endTime));
    sessionSendCtrlMsg(testSvrSession->ctrlSvrSession->ctrlSession,
        MSG_CHAT, output, strlen(output));
    sessionSendCtrlMsg(testSvrSession->ctrlSvrSession->ctrlSession,
        MSG_STOP_TEST, "a", 1);

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

/**
 * function run on the server that is responsible for stopping the test.
 *
 * @function   delayStopRoutine
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
 * this thread checks the the last time we received a packet constantly, and if
 *   that time is greater then 5 seconds in the passed, then it closes the test
 *   session.
 *
 * this will not skew the test time. the test time is starts from when the test
 *   session is created, until when the last packet is received. so, if no
 *   packets were received, then i guess that means the test's duration would be
 *   zero.
 *
 * @signature  static DWORD WINAPI delayStopRoutine(void* params)
 *
 * @param      params pointer to the server test session's user pointer which
 *   has a TestSvrSession structure.
 *
 * @return     exit code.
 */
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
