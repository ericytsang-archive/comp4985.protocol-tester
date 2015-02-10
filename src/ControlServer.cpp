#include "ControlServer.h"

static void onConnect(Server*, SOCKET, sockaddr_in);
static void onError(Server*, int, int);
static void onClose(Server*, int);

/**
 * initializes the control server structure, and sets its callbacks.
 *
 * @function   ctrlSvrInit
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
 * @signature  void ctrlSvrInit(Server* server, ServerWnds* serverWnds)
 *
 * @param      server pointer to the control server structure
 * @param      serverWnds pointer to the structure that has all the server
 *   window handles.
 */
void ctrlSvrInit(Server* server, ServerWnds* serverWnds)
{
    serverInit(server);
    server->usrPtr     = malloc(sizeof(CtrlSvr));
    server->onClose    = onClose;
    server->onConnect  = onConnect;
    server->onError    = onError;

    CtrlSvr* ctrlSvr    = (CtrlSvr*) server->usrPtr;
    ctrlSvr->serverWnds = serverWnds;
    ctrlSvr->file       = INVALID_HANDLE_VALUE;
    linkedListInit(&ctrlSvr->ctrlSessions);
}

/**
 * starts the control server's listening port; prints the outcome of the
 *   operation.
 *
 * @function   ctrlSvrStart
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
 * @signature  void ctrlSvrStart(Server* server)
 *
 * @param      server pointer to server structure
 */
void ctrlSvrStart(Server* server)
{
    char output[MAX_STRING_LEN];        // buffer for output
    char portString[MAX_STRING_LEN];    // buffer holding remote port
    unsigned short port;                // remote port in unsigned short form
    int returnCode;

    // parse user pointer
    CtrlSvr* ctrlSvr = (CtrlSvr*) server->usrPtr;

    // get and parse user input
    GetWindowText(ctrlSvr->serverWnds->hPort, portString, MAX_STRING_LEN);
    port = atoi(portString);

    // set the port and start the server
    serverSetPort(server, port);
    returnCode = serverStart(server);
    switch(returnCode)
    {
    case NORMAL_SUCCESS:
        sprintf_s(output, "Server started; listening on port %d\r\n", port);
        appendWindowText(ctrlSvr->serverWnds->hOutput, output);
        break;
    default:
        sprintf_s(output, "Failed to start server: %s\r\n",
            rctoa(returnCode));
        appendWindowText(ctrlSvr->serverWnds->hOutput, output);
        break;
    }
}

/**
 * closes the server's listening port.
 *
 * @function   ctrlSvrStop
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
 * @signature  void ctrlSvrStop(Server* server)
 *
 * @param      server pointer to the server structure
 */
void ctrlSvrStop(Server* server)
{
    char output[MAX_STRING_LEN];    // temporary buffer for output
    int returnCode;

    // parse user pointer
    CtrlSvr* ctrlSvr = (CtrlSvr*) server->usrPtr;

    // stop the server
    returnCode = serverStop(server);
    switch(returnCode)
    {
    case NORMAL_SUCCESS:
        sprintf_s(output, "Server stopped\r\n");
        appendWindowText(ctrlSvr->serverWnds->hOutput, output);
        break;
    default:
        sprintf_s(output, "Failed to stop the Server: %s\r\n",
            rctoa(returnCode));
        appendWindowText(ctrlSvr->serverWnds->hOutput, output);
        break;
    }
}

/**
 * convenience function used to send chat messages through the all the control
 *   lines associated with this server. really, this just sends a chat type
 *   control message.
 *
 * @function   ctrlSvrSendChat
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
 * @signature  void ctrlSvrSendChat(Server* server)
 *
 * @param      server pointer to the control server structure
 */
void ctrlSvrSendChat(Server* server)
{
    char message[MAX_STRING_LEN];   // contains the user's chat message
    char output[MAX_STRING_LEN];    // temporary buffer for output

    // parse user pointer
    CtrlSvr* ctrlSvr = (CtrlSvr*) server->usrPtr;

    // only send the message if there are sessions to send to
    if(linkedListSize(&ctrlSvr->ctrlSessions) > 0)
    {
        Node* curr;

        // get user's chat message
        GetWindowText(ctrlSvr->serverWnds->hInput, message, MAX_STRING_LEN);
        sprintf_s(output, "Sending: %s\r\n", message);
        appendWindowText(ctrlSvr->serverWnds->hOutput, output);

        // send the chat message
        for(curr = ctrlSvr->ctrlSessions.head; curr != 0; curr = curr->next)
        {
            sessionSendCtrlMsg((Session*) curr->data, MSG_CHAT, message,
                strlen(message));
        }
    }
    else
    {
        sprintf_s(output,
            "no existing control sessions; cannot send message.\r\n");
        appendWindowText(ctrlSvr->serverWnds->hOutput, output);
    }
}

/**
 * invoked when a new connection is made. this function returns the socket that
 *   should be used to interact with the connection from this point in time
 *   onwards.
 *
 * @function   onConnect
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
 * @signature  static void onConnect(Server* server, SOCKET clientSock,
 *   sockaddr_in clientAddr)
 *
 * @param      server pointer to the server object that this new connection is
 *   for
 * @param      clientSock socket used to communicate with the remote host
 *   through the connection
 * @param      clientAddr structure with information about the remote host's
 *   address
 */
static void onConnect(Server* server, SOCKET clientSock, sockaddr_in clientAddr)
{
    char output[MAX_STRING_LEN];        // buffer for output

    // parse user parameters
    CtrlSvr* ctrlSvr = (CtrlSvr*) server->usrPtr;

    // create and start the control server session
    Session* session = (Session*) malloc(sizeof(Session));
    ctrlSvrSessionInit(session, ctrlSvr, clientSock, clientAddr);
    sessionStart(session);

    // add the session to out list of control server sessions
    linkedListPrepend(&ctrlSvr->ctrlSessions, session);

    // print connected message
    sprintf_s(output, "%s:%d connected\r\n", inet_ntoa(clientAddr.sin_addr),
        htons(clientAddr.sin_port));
    appendWindowText(ctrlSvr->serverWnds->hOutput, output);

    // forward all other control sessions the same message
    Node* curr;
    for(curr = ctrlSvr->ctrlSessions.head; curr != 0;
        curr = curr->next)
    {
        sessionSendCtrlMsg((Session*) curr->data, MSG_CHAT, output,
            strlen(output)-2);
    }
}

/**
 * invoked when an error occurs regarding the passed server. this function just
 *   reports the error.
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
 * static void onError(Server* server, int errCode, int winErrCode)
 *
 * @param      server pointer to the server object that the error is for.
 * @param      errCode return code indicating the nature of the error
 * @param      winErrCode window's error code indicating the nature of the
 *   error.
 */
static void onError(Server* server, int errCode, int winErrCode)
{
    char output[MAX_STRING_LEN];        // buffer for output

    // parse user parameters
    CtrlSvr* ctrlSvr = (CtrlSvr*) server->usrPtr;

    // print error message
    sprintf_s(output, "Server encountered an error: %s - %d\r\n",
        rctoa(errCode), winErrCode);
    appendWindowText(ctrlSvr->serverWnds->hOutput, output);
}

/**
 * invoked when the control server's TCP listener is shut down
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
 * @note
 *
 * this function just reports that the server has been stopped, and for what
 *   reason.
 *
 * @signature  static void onClose(Server* server, int code)
 *
 * @param      server pointer to the server structure
 * @param      code code indicating the reason why the server was shut down
 */
static void onClose(Server* server, int code)
{
    char output[MAX_STRING_LEN];        // buffer for output

    // parse user pointer
    CtrlSvr* ctrlSvr = (CtrlSvr*) server->usrPtr;

    // print stop message
    sprintf_s(output, "Server stopped - %s\r\n", rctoa(code));
    appendWindowText(ctrlSvr->serverWnds->hOutput, output);
}
