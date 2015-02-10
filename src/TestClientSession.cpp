/**
 * the test session on the client side.
 *
 * @sourceFile TestClientSession.cpp
 *
 * @program    ProtocolTester.exe
 *
 * @function   void testClntSessionInit(Session*, CtrlClnt*, SOCKET,
 *   sockaddr_in)
 * @function   static void onMessage(Session*, char*, int)
 * @function   static void onError(Session*, int, int)
 * @function   static void onClose(Session*, int)
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
 * this session, since it is on the client side, is used to send messages to the
 *   remote host; there isn't much to do in this file.
 *
 * this session should not receive any messages; the onMessage function is left
 *   empty.
 *
 * onClose, this session cleans up after itself.
 *
 * onError, the session reports the error, and then closes itself.
 */
#include "TestClientSession.h"

static void onMessage(Session*, char*, int);
static void onError(Session*, int, int);
static void onClose(Session*, int);

/**
 * initializes the test client session structure.
 *
 * @function   testClntSessionInit
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
 * assigns the user pointer of the session, and the callbacks to the appropriate
 *   callbacks of the client side test session.
 *
 * @signature  void testClntSessionInit(Session* session, CtrlClnt* CtrlClnt,
 *   SOCKET clientSock, sockaddr_in clientAddr)
 *
 * @param      session pointer to the session structure
 * @param      CtrlClnt pointer to the control client structure that this test
 *   is for
 * @param      clientSock socket representing the connection
 * @param      clientAddr address of the remote
 */
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

/**
 * don't do anything when we receive a message, we are not supposed to receive a
 *   message anyway.
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
 * @note       none
 *
 * @signature  static void onMessage(Session* session, char* str, int len)
 *
 * @param      session session structure pointer
 * @param      str pointer to the beginning of an array of the received data.
 * @param      len length of the data received.
 */
static void onMessage(Session* session, char* str, int len)
{
}

/**
 * when an error occurs, report the error, and close the session.
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
 * @param      session session structure pointer
 * @param      errCode error return code indicating the nature of the error.
 * @param      winErrCode the windows error return code, indicating the nature
 *   of the error.
 */
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

/**
 * invoked when the session is being closed. here, we free all the memory used
 *   by this session, and print to the screen, reporting that the session has
 *   stopped.
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
 * @param      session pointer to the session structure this operation is for.
 * @param      closeCode return code indicating the reason why the session is
 *   being closed.
 */
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
