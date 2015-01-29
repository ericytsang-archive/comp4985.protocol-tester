#include "Session.h"

struct AsyncReadThreadParams
{

};

// thread functions
static DWORD WINAPI sessionThread(void*);
static DWORD WINAPI asyncReadThread(void*);

// other functions...
static HANDLE asyncRead(HANDLE, SOCKET);
static int _sessionStart(Session* session);

// initializes the session structure members
void sessionInit(Session* session, SOCKET*, sockaddr_in*)
{
    _sessionStart(session);
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
    CloseHandle(session->_sessionThread);
    session->_sessionThread = INVALID_HANDLE_VALUE;

    return NORMAL_SUCCESS;
}

// starts the session's thread
static int _sessionStart(Session* session)
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
        session->onError(session, THREAD_FAIL, 0, 0);
        return THREAD_FAIL;
    }

    return NORMAL_SUCCESS;
}

static DWORD WINAPI sessionThread(void* params)
{
    Session* session = (Session*) params;

    session->_clientSocket.
}
