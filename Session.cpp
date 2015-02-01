#include "Session.h"

static char debugString[1000];

/**
 * parameters for the asyncRecvThread routine.
 */
struct AsyncRecvThreadParams
{
    HANDLE event;
    Session* session;
    char* buffer;
    int bytesToRead;
    int* bytesRead;
};

typedef struct AsyncRecvThreadParams AsyncRecvThreadParams;

// thread functions
static DWORD WINAPI sessionThread(void*);
static DWORD WINAPI asyncRecvThread(void*);

// other functions...
static int sessionStart(Session* session);
static HANDLE asyncRecv(HANDLE, Session*, char*, int, int*);

/////////////////////////
// interface functions //
/////////////////////////

// initializes the session structure members
void sessionInit(Session* session, SOCKET* clientSocket,
    sockaddr_in* clientAddres)
{
    session->_remoteAddress    = *clientAddres;
    session->_remoteAddressLen = sizeof(*clientAddres);
    session->_remoteSocket     = *clientSocket;
    session->_bufLen           = DEFAULT_BUFFER_LEN;
    session->_stopEvent        = CreateEvent(NULL, TRUE, FALSE, NULL);
    session->_sessionThread    = INVALID_HANDLE_VALUE;
}

// starts the session's thread
int sessionStart(Session* session)
{
    DWORD threadId;     // useless...

    // make sure session isn't already running
    if(session->_sessionThread != INVALID_HANDLE_VALUE)
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

// sends a signal to stop the session's thread
int sessionClose(Session* session)
{
    // make sure session is already running so we can end it
    if(session->_sessionThread == INVALID_HANDLE_VALUE)
    {
        return ALREADY_STOPPED_FAIL;
    }

    // signal session thread to stop
    SetEvent(session->_stopEvent);

    // forget about the session thread because it's stopping
    WaitForSingleObject(session->_sessionThread, INFINITE);
    CloseHandle(session->_sessionThread);
    session->_sessionThread = INVALID_HANDLE_VALUE;

    return NORMAL_SUCCESS;
}

// sets the length of the buffer that's used to read from the socket
void sessionSetBufLen(Session* session, int newLen)
{
    session->_bufLen = newLen;
}

int sessionSend(Session* session, void* data, int len)
{
    return sendto(session->_remoteSocket, (char*) data, len, 0,
        (sockaddr*) &session->_remoteAddress, session->_remoteAddressLen);
}

void sessionSetUserPtr(Session* session, void* ptr)
{
    session->_usrPtr = ptr;
}

void* sessionGetUserPtr(Session* session)
{
    return session->_usrPtr;
}

IN_ADDR sessionGetIP(Session* session)
{
    return session->_remoteAddress.sin_addr;
}

//////////////////////
// static functions //
//////////////////////

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
                        session->onError(session, SOCKET_FAIL, GetLastError());
                        session->onClose(session, SOCKET_FAIL);
                        returnValue = SOCKET_FAIL;
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

// starts the asynchronous receive thread
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

static DWORD WINAPI asyncRecvThread(void* params)
{
    // parse thread parameters
    AsyncRecvThreadParams* threadParams = (AsyncRecvThreadParams*) params;
    HANDLE event     = threadParams->event;
    Session* session  = threadParams->session;
    char* buffer     = threadParams->buffer;
    int bytesToRead  = threadParams->bytesToRead;
    int* bytesRead   = threadParams->bytesRead;

    // make the receive call
    *bytesRead = recvfrom(session->_remoteSocket, buffer, bytesToRead, 0,
        (sockaddr*) &session->_remoteAddress, &session->_remoteAddressLen);
    if(*bytesRead == SOCKET_ERROR)
    {
        session->onError(session, RECV_FAIL, GetLastError());
        session->onClose(session, RECV_FAIL);
        closesocket(session->_remoteSocket);
    }

    // trigger the signal because asynchronous
    SetEvent(event);

    // clean up and return
    free(threadParams);
    return 0;
}
