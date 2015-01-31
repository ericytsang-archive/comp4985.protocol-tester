#include "Session.h"

static char debugString[1000];

struct AsyncRecvThreadParams
{
    HANDLE event;
    SOCKET socket;
    char* buffer;
    int bytesToRead;
    int* bytesRead;
};

// thread functions
static DWORD WINAPI sessionThread(void*);
static DWORD WINAPI asyncReadThread(void*);

// other functions...
static HANDLE asyncRead(HANDLE, SOCKET);
static int sessionStart(Session* session);
static HANDLE asyncRecv(HANDLE, SOCKET, char*, int, int*);
static DWORD WINAPI asyncRecvThread(void*);

// initializes the session structure members
void sessionInit(Session* session, SOCKET* clientSocket,
    sockaddr_in* clientAddres)
{
    session->_clientAddress  = *clientAddres;
    session->_clientSocket   = *clientSocket;
    session->_bufLen         = DEFAULT_BUFFER_LEN;
    session->_stopEvent      = CreateEvent(NULL, TRUE, FALSE, NULL);
    session->_sessionThread  = INVALID_HANDLE_VALUE;

    sessionStart(session);
}

// sends a signal to stop the session's thread
int sessionClose(Session* session)
{
    // make sure session is already running
    if(session->_sessionThread == INVALID_HANDLE_VALUE)
    {
        return SESSION_ALREADY_STOPPED_FAIL;
    }

    // signal session thread to stop
    SetEvent(session->_stopEvent);

    // forget about the session thread
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
    return send(session->_clientSocket, (char*) data, len, 0);
}

// starts the session's thread
static int sessionStart(Session* session)
{
    DWORD threadId;     // useless...

    // make sure session isn't already running
    if(session->_sessionThread != INVALID_HANDLE_VALUE)
    {
        return SESSION_ALREADY_RUNNING_FAIL;
    }

    // start the session
    ResetEvent(session->_stopEvent);
    session->_sessionThread =
        CreateThread(NULL, 0, sessionThread, session, 0, &threadId);
    if(session->_sessionThread == INVALID_HANDLE_VALUE)
    {
        session->onError(session, THREAD_FAIL);
        return THREAD_FAIL;
    }

    return NORMAL_SUCCESS;
}

static DWORD WINAPI sessionThread(void* params)
{
    Session* session = (Session*) params;

    // threads, and synchronization
    HANDLE recvThread = CreateEvent(NULL, TRUE, TRUE, NULL); // will be reassigned later;
    HANDLE recvEvent;
    HANDLE handles[2];

    // receive call
    int bytesRead;
    int bytesToRead;
    char* buffer;
    int waitResult;
    BOOL breakLoop;
    int returnValue;

    breakLoop = FALSE;
    recvEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    // do asynchronous receive call
    while(!breakLoop)
    {
        bytesRead = 0;
        bytesToRead = session->_bufLen;
        buffer = (char*) malloc(session->_bufLen);

        ResetEvent(recvEvent);
        recvThread = asyncRecv(recvEvent, session->_clientSocket, buffer, bytesToRead, &bytesRead);

        handles[0] = recvEvent;
        handles[1] = session->_stopEvent;
        waitResult = WaitForMultipleObjects(sizeof(handles) / sizeof(HANDLE), handles, FALSE, INFINITE);
        switch(waitResult)
        {

            // receive event
            case WAIT_OBJECT_0+0:
            switch(bytesRead)
            {

                // socket is now closed
                case 0:
                session->onClose(session, NORMAL_SUCCESS);
                returnValue = NORMAL_SUCCESS;
                breakLoop = TRUE;
                break;

                // handle socket error
                case SOCKET_ERROR:
                session->onError(session, SOCKET_FAIL);
                session->onClose(session, SOCKET_FAIL);
                returnValue = SOCKET_FAIL;
                breakLoop = TRUE;
                break;

                // handle data
                default:
                session->onMessage(session, buffer, bytesRead);
                break;
            }
            free(buffer);
            break;

            // stop event signaled; close the session
            case WAIT_OBJECT_0+1:
            session->onClose(session, NORMAL_SUCCESS);
            returnValue = NORMAL_SUCCESS;
            breakLoop = TRUE;
            break;

            // some sort of something; report error
            default:
            sprintf_s(debugString, "Error @ 1 %d\n", GetLastError());
            OutputDebugString(debugString);
            session->onError(session, UNKNOWN_FAIL);
            session->onClose(session, UNKNOWN_FAIL);
            returnValue = UNKNOWN_FAIL;
            breakLoop = TRUE;
            break;
        }
    }

    // clean up & return
    closesocket(session->_clientSocket);
    WaitForSingleObject(recvThread, INFINITE);
    return returnValue;
}

static HANDLE asyncRecv(HANDLE event, SOCKET socket, char* buffer, int bytesToRead, int* bytesRead)
{
    AsyncRecvThreadParams* threadParams;
    DWORD threadId;
    HANDLE threadHandle;
    int ret;

    // prepare thread parameters
    threadParams = (AsyncRecvThreadParams*)
        malloc(sizeof(AsyncRecvThreadParams));
    threadParams->event        = event;
    threadParams->socket       = socket;
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
    SOCKET socket    = threadParams->socket;
    char* buffer     = threadParams->buffer;
    int bytesToRead  = threadParams->bytesToRead;
    int* bytesRead   = threadParams->bytesRead;
    // int bytesRead    = 0;

    // make the receive call
    *bytesRead = recv(socket, buffer, bytesToRead, 0);
    // while (bytesToRead > 0)
    // {
    //     recv(socket, buffer, bytesToRead, 0);

    //     buffer += bytesRead;
    //     bytesToRead -= bytesRead;
    // }

    // trigger the signal because asynchronous
    SetEvent(event);

    // clean up and return
    free(threadParams);
    return 0;
}