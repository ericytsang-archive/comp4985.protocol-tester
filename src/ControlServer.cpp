#include "ControlServer.h"

static void onConnect(Server*, SOCKET, sockaddr_in);
static void onError(Server*, int, int);
static void onClose(Server*, int);

// good
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

// good
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

// good
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

// good
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
                strlen(message+1));
        }
    }
    else
    {
        sprintf_s(output, "no existing control sessions; cannot send message.\r\n");
        appendWindowText(ctrlSvr->serverWnds->hOutput, output);
    }
}

// good
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

// good
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

// good
static void onClose(Server* server, int code)
{
    char output[MAX_STRING_LEN];        // buffer for output

    // parse user pointer
    CtrlSvr* ctrlSvr = (CtrlSvr*) server->usrPtr;

    // print stop message
    sprintf_s(output, "Server stopped - %s\r\n", rctoa(code));
    appendWindowText(ctrlSvr->serverWnds->hOutput, output);
}
