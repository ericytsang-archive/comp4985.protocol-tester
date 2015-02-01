/**
 * contains functions and thread routines that perform operations on session
 *   structures.
 *
 * @sourceFile Session.cpp
 *
 * @program    ProtocolTester.exe
 *
 * @function   void sessionInit(Session* session, SOCKET* remoteSocket,
 *   sockaddr_in* remoteAddres)
 * @function   int sessionStart(Session* session)
 * @function   int sessionClose(Session* session)
 * @function   void sessionSetBufLen(Session* session, int newLen)
 * @function   int sessionSend(Session* session, void* data, int len)
 * @function   IN_ADDR sessionGetIP(Session* session)
 * @function   static DWORD WINAPI sessionThread(void* params)
 * @function   static HANDLE asyncRecv(HANDLE event, Session* session, char*
 *   buffer, int bytesToRead, int* bytesRead)
 * @function   static DWORD WINAPI asyncRecvThread(void* params)
 *
 * @date       2015-02-01
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 */
#include "Session.h"

/**
 * parameter structure for the asyncRecvThread routine.
 */
struct AsyncRecvThreadParams
{
    HANDLE event;       // event to signal when something is received.
    Session* session;   // session object that represents the connection.
    char* buffer;       // buffer to write the read data into.
    int bytesToRead;    // number of bytes to read from the session's socket.
    int* bytesRead;     // number of bytes actually read from session's socket.
};

typedef struct AsyncRecvThreadParams AsyncRecvThreadParams;

// static function declarations
static DWORD WINAPI sessionThread(void*);
static DWORD WINAPI asyncRecvThread(void*);
static HANDLE asyncRecv(HANDLE, Session*, char*, int, int*);

/////////////////////////
// interface functions //
/////////////////////////

/**
 * initializes the session structure members
 *
 * @function   sessionInit
 *
 * @date       2015-01-31
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 *
 * @signature  void sessionInit(Session* session, SOCKET* remoteSocket,
 *   sockaddr_in* remoteAddres)
 *
 * @param      session pointer to a Session structure; the "this" pointer.
 * @param      remoteSocket pointer to a SOCKET that the session will use for
 *   transmitting and receiving. the socket will be copied into the session
 *   structure.
 * @param      remoteAddres pointer to a sockaddr_in. contains information about
 *   the remote host. contents of the passed structure will be copied into the
 *   session structure.
 */
void sessionInit(Session* session, SOCKET* remoteSocket,
    sockaddr_in* remoteAddres)
{
    session->_remoteAddress    = *remoteAddres;
    session->_remoteAddressLen = sizeof(*remoteAddres);
    session->_remoteSocket     = *remoteSocket;
    session->_bufLen           = DEFAULT_BUFFER_LEN;
    session->_stopEvent        = CreateEvent(NULL, TRUE, FALSE, NULL);
    session->_sessionThread    = INVALID_HANDLE_VALUE;
    session->usrPtr            = 0;
}

/**
 * starts the session's thread so callbacks will be invoked and things.
 *
 * @function   sessionStart
 *
 * @date       2015-01-31
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note
 *
 * returns ALREADY_RUNNING_FAIL if the session is already running.
 *
 * returns THREAD_FAIL if the session thread somehow fails to start.
 *
 * returns NORMAL_SUCCESS if the session thread is started successfully.
 *
 * @signature  int sessionStart(Session* session)
 *
 * @param      session pointer to a Session structure; the "this" pointer.
 *
 * @return     integer indicating the result of the operation.
 */
int sessionStart(Session* session)
{
    DWORD threadId;     // useless...

    // make sure session isn't already running
    if(sessionIsRunning(session))
    {
        return ALREADY_RUNNING_FAIL;
    }

    // start the session
    ResetEvent(session->_stopEvent);
    session->_sessionThread =
        CreateThread(NULL, 0, sessionThread, session, 0, &threadId);
    if(session->_sessionThread == INVALID_HANDLE_VALUE)
    {
        return THREAD_FAIL;
    }

    return NORMAL_SUCCESS;
}

/**
 * sends a signal to stop the session's thread; the session will be closed, and
 *   wont be able to be started again.
 *
 * @function   sessionClose
 *
 * @date       2015-01-31
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note
 *
 * returns ALREADY_STOPPED_FAIL if the session is already stopped and thus can't
 *   be stopped.
 *
 * returns NORMAL_SUCCESS if the session is has been stopped by this function.
 *
 * @signature  int sessionClose(Session* session)
 *
 * @param      session pointer to a Session structure; the "this" pointer.
 *
 * @return     returns a status code indicating the result of the operation.
 */
int sessionClose(Session* session)
{
    // make sure session is already running so we can end it
    if(!sessionIsRunning(session))
    {
        return ALREADY_STOPPED_FAIL;
    }

    // signal session thread to stop
    SetEvent(session->_stopEvent);

    // forget about the session thread because it's stopping
    CloseHandle(session->_sessionThread);
    session->_sessionThread = INVALID_HANDLE_VALUE;

    return NORMAL_SUCCESS;
}

/**
 * non-blocking. returns true if the session is currently running; false otherwise.
 *
 * @function   sessionIsRunning
 *
 * @date       2015-01-31
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 *
 * @signature  BOOL sessionIsRunning(Session* session)
 *
 * @param      session pointer to a Session structure; the "this" pointer.
 *
 * @return     true if the session is currently running; false otherwise.
 */
BOOL sessionIsRunning(Session* session)
{
    return (session->_sessionThread != INVALID_HANDLE_VALUE
        && WaitForSingleObject(session->_sessionThread, 100) == WAIT_TIMEOUT);
}

/**
 * sets the length of the buffer that's used to read from the socket.
 *
 * @function   sessionSetBufLen
 *
 * @date       2015-02-01
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 *
 * @signature  void sessionSetBufLen(Session* session, int newLen)
 *
 * @param      session pointer to a Session structure; the "this" pointer.
 * @param      newLen length of the buffer to use.
 */
void sessionSetBufLen(Session* session, int newLen)
{
    session->_bufLen = newLen;
}

/**
 * sends the specified data out to the remote host through the session's
 *   socket.
 *
 * @function   sessionSend
 *
 * @date       2015-02-01
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 *
 * @signature  int sessionSend(Session* session, void* data, int len)
 *
 * @param      session pointer to a Session structure; the "this" pointer.
 * @param      data pointer to the data array to sent to the remote host.
 * @param      len length of the data array.
 *
 * @return     number of bytes sent out; SOCKET_ERROR if unsuccessful.
 */
int sessionSend(Session* session, void* data, int len)
{
    return sendto(session->_remoteSocket, (char*) data, len, 0,
        (sockaddr*) &session->_remoteAddress, session->_remoteAddressLen);
}

/**
 * returns the session's remote host's Internet address.
 *
 * @function   sessionGetIP
 *
 * @date       2015-02-01
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 *
 * @signature  IN_ADDR sessionGetIP(Session* session)
 *
 * @param      session pointer to a Session structure; the "this" pointer.
 *
 * @return     session's remote host's Internet address.
 */
IN_ADDR sessionGetIP(Session* session)
{
    return session->_remoteAddress.sin_addr;
}

//////////////////////
// static functions //
//////////////////////

/**
 * the routine run for the passed session. continuously reads from the socket,
 *   and invokes the session's callbacks as necessary.
 *
 * @function   sessionThread
 *
 * @date       2015-02-01
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 *
 * @signature  static DWORD WINAPI sessionThread(void* params)
 *
 * @param      params pointer to a session object who's socket will be used, and
 *   callbacks invoked by this thread.
 *
 * @return     return code indicating the result of the operation.
 */
static DWORD WINAPI sessionThread(void* params)
{
    Session* session = (Session*) params;

    // threads, and synchronization
    HANDLE recvThread = CreateEvent(NULL, TRUE, TRUE, NULL);
    HANDLE recvEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    // receive call
    int returnValue;
    BOOL breakLoop = FALSE;

    // do asynchronous receive call continuously
    while(!breakLoop)
    {
        int bytesRead = 0;
        int bytesToRead = session->_bufLen;
        char* buffer = (char*) malloc(session->_bufLen);

        ResetEvent(recvEvent);
        recvThread = asyncRecv(
            recvEvent, session, buffer, bytesToRead, &bytesRead);

        HANDLE handles[] = {recvEvent, session->_stopEvent};
        int waitResult = WaitForMultipleObjects(
            sizeof(handles) / sizeof(HANDLE), handles, FALSE, INFINITE);
        switch(waitResult)
        {
            case WAIT_OBJECT_0+0:   // receive event
                switch(bytesRead)
                {
                    case 0:             // socket is now closed
                        session->onClose(session, NORMAL_SUCCESS);
                        returnValue = NORMAL_SUCCESS;
                        breakLoop = TRUE;
                        break;
                    case SOCKET_ERROR:  // handle socket error
                        session->onError(session, RECV_FAIL, GetLastError());
                        session->onClose(session, RECV_FAIL);
                        returnValue = RECV_FAIL;
                        breakLoop = TRUE;
                        break;
                    default:            // handle data
                        session->onMessage(session, buffer, bytesRead);
                        break;
                }
                free(buffer);
                break;
            case WAIT_OBJECT_0+1:   // stop event signaled; close the session
                session->onClose(session, NORMAL_SUCCESS);
                returnValue = NORMAL_SUCCESS;
                breakLoop = TRUE;
                break;
            default:                // some sort of something; report error
                session->onError(session, UNKNOWN_FAIL, GetLastError());
                session->onClose(session, UNKNOWN_FAIL);
                returnValue = UNKNOWN_FAIL;
                breakLoop = TRUE;
                break;
        }
    }

    // clean up & return
    closesocket(session->_remoteSocket);
    WaitForSingleObject(recvThread, INFINITE);
    return returnValue;
}

/**
 * starts the asynchronous receive thread. when something is received, the
 *   passed event will be signaled.
 *
 * @function   asyncRecv
 *
 * @date       2015-02-01
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 *
 * @signature  static HANDLE asyncRecv(HANDLE event, Session* session, char*
 *   buffer, int bytesToRead, int* bytesRead)
 *
 * @param      event handle to an event that will be signaled when something is
 *   received on the session's socket.
 * @param      session session who's properties will be used.
 * @param      buffer buffer to read the data into.
 * @param      bytesToRead number of bytes to read from the socket.
 * @param      bytesRead number of bytes actually read from the socket.
 *
 * @return     handle to the accept thread.
 */
static HANDLE asyncRecv(HANDLE event, Session* session, char* buffer,
    int bytesToRead, int* bytesRead)
{
    AsyncRecvThreadParams* threadParams;
    DWORD threadId;
    HANDLE threadHandle;

    // prepare thread parameters
    threadParams = (AsyncRecvThreadParams*)
        malloc(sizeof(AsyncRecvThreadParams));
    threadParams->event        = event;
    threadParams->session      = session;
    threadParams->buffer       = buffer;
    threadParams->bytesToRead  = bytesToRead;
    threadParams->bytesRead    = bytesRead;

    // make the thread to make asynchronous call
    threadHandle =
        CreateThread(NULL, 0, asyncRecvThread, threadParams, 0, &threadId);

    return threadHandle;
}

/**
 * routine that should be threaded, and performs the accept call.
 *
 * @function   asyncRecvThread
 *
 * @date       2015-02-01
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 *
 * @signature  static DWORD WINAPI asyncRecvThread(void* params)
 *
 * @param      params pointer to a AsyncRecvThreadParams structure.
 *
 * @return     returns 0.
 */
static DWORD WINAPI asyncRecvThread(void* params)
{
    // parse thread parameters
    AsyncRecvThreadParams* threadParams = (AsyncRecvThreadParams*) params;
    HANDLE event     = threadParams->event;
    Session* session = threadParams->session;
    char* buffer     = threadParams->buffer;
    int bytesToRead  = threadParams->bytesToRead;
    int* bytesRead   = threadParams->bytesRead;

    // make the receive call
    *bytesRead = recvfrom(session->_remoteSocket, buffer, bytesToRead, 0,
        (sockaddr*) &session->_remoteAddress, &session->_remoteAddressLen);

    // trigger the signal because asynchronous
    SetEvent(event);

    // clean up and return
    free(threadParams);
    return 0;
}
