#include "ControlServer.h"

static void onConnect(Server*, SOCKET, sockaddr_in);
static void onError(Server*, int, int);
static void onClose(Server*, int);

// good
void testSvrInit(Server* server, ServerWnds* serverWnds)
{
    serverInit(server);
    server->onClose    = onClose;
    server->onConnect  = onConnect;
    server->onError    = onError;
}

// // good
// void testSvrStart(Server* server)
// {
//     char output[MAX_STRING_LEN];        // buffer for output
//     char portString[MAX_STRING_LEN];    // buffer holding remote port
//     unsigned short port;                // remote port in unsigned short form
//     int returnCode;

//     // parse user pointer
//     CtrlSvr* ctrlSvr = (CtrlSvr*) server->usrPtr;

//     // get and parse user input
//     GetWindowText(ctrlSvr->serverWnds->hPort, portString, MAX_STRING_LEN);
//     port = atoi(portString);

//     // set the port and start the server
//     serverSetPort(server, port);
//     returnCode = serverStart(server);
//     switch(returnCode)
//     {
//     case NORMAL_SUCCESS:
//         sprintf_s(output, "Server started; listening on port %d\r\n", port);
//         appendWindowText(ctrlSvr->serverWnds->hOutput, output);
//         break;
//     default:
//         sprintf_s(output, "Failed to start server: %s\r\n",
//             rctoa(returnCode));
//         appendWindowText(ctrlSvr->serverWnds->hOutput, output);
//         break;
//     }
// }

// // good
// void testSvrStop(Server* server)
// {
//     char output[MAX_STRING_LEN];    // temporary buffer for output
//     int returnCode;

//     // parse user pointer
//     CtrlSvr* ctrlSvr = (CtrlSvr*) server->usrPtr;

//     // stop the server
//     returnCode = serverStop(server);
//     switch(returnCode)
//     {
//     case NORMAL_SUCCESS:
//         sprintf_s(output, "Server stopped\r\n");
//         appendWindowText(ctrlSvr->serverWnds->hOutput, output);
//         break;
//     default:
//         sprintf_s(output, "Failed to stop the Server: %s\r\n",
//             rctoa(returnCode));
//         appendWindowText(ctrlSvr->serverWnds->hOutput, output);
//         break;
//     }
// }

// good
static void onConnect(Server* server, SOCKET clientSock, sockaddr_in clientAddr)
{
    char output[MAX_STRING_LEN];        // buffer for output

    // parse user parameters
    CtrlSvr* ctrlSvr = (CtrlSvr*) server->usrPtr;

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
