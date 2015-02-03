#include "ControlClient.h"

// good
void ctrlClntInit(Client* client, ClientWnds* clientWnds)
{
    clientInit(client);
    client->usrPtr    = malloc(sizeof(CtrlSvr));
    client->onConnect = ctrlClntOnConnect;
    client->onError   = ctrlClntOnError;

    CtrlClnt* ctrlClnt     = (CtrlClnt*) client->usrPtr;
    ctrlClnt->clientWnds   = clientWnds;
    ctrlClnt->testProtocol = MODE_UNDEFINED;
    ctrlClnt->testPort     = 0;
    memset(&ctrlClnt->ctrlSession, 0, sizeof(Session));
    ctrlClnt->ctrlSession._sessionThread = INVALID_HANDLE_VALUE;
    memset(&ctrlClnt->testSession, 0, sizeof(Session));
    ctrlClnt->testSession._sessionThread = INVALID_HANDLE_VALUE;
}

void ctrlClntSetTestProtocol(Client* client, int protocol)
{
    // parse user pointer
    CtrlClnt* ctrlClnt = (CtrlClnt*) client->usrPtr;
    ctrlClnt->testProtocol = protocol;
}

// good
void ctrlClntOnConnect(Client* client, SOCKET clientSock, sockaddr_in clientAddr)
{
    char output[MAX_STRING_LEN];    // temporary output buffer

    // parse user pointer
    CtrlClnt* ctrlClnt = (CtrlClnt*) client->usrPtr;

    // print connected message
    sprintf_s(output, "Control connected to \"%s:%d\"\r\n",
        inet_ntoa(clientAddr.sin_addr),
        htons(clientAddr.sin_port));
    appendWindowText(ctrlClnt->clientWnds->hOutput, output);

    // create and start the client control session
    ctrlClntSessionInit(&ctrlClnt->ctrlSession, ctrlClnt, clientSock, clientAddr);
    sessionStart(&ctrlClnt->ctrlSession);
}

// good
void ctrlClntOnError(Client* client, int errCode, int winErrCode)
{
    char output[MAX_STRING_LEN];    // temporary output buffer

    // parse user pointer
    CtrlClntSession* ctrlClntSession = (CtrlClntSession*) client->usrPtr;

    // print failure message to the screen
    sprintf_s(output, "Failed to connect: %s - %d\r\n",
        rctoa(errCode), winErrCode);
    appendWindowText(ctrlClntSession->clientWnds->hOutput, output);
}

// good
void ctrlClntConnect(Client* client)
{
    char output[MAX_STRING_LEN];    // temporary buffer for output
    char hostIp[MAX_STRING_LEN];    // holds remote IP specified by user
    char hostPort[MAX_STRING_LEN];  // holds the remote port specified by user
    unsigned int port;              // the actual remote port...as an integer

    // parse user pointer
    CtrlClnt* ctrlClnt = (CtrlClnt*) client->usrPtr;

    // get and parse user input
    GetWindowText(ctrlClnt->clientWnds->hIpHost, hostIp, MAX_STRING_LEN);
    GetWindowText(ctrlClnt->clientWnds->hCtrlPort, hostPort, MAX_STRING_LEN);
    port = atoi(hostPort);

    // abort connection if we're still connecting from a previous call
    if(sessionIsRunning(&ctrlClnt->ctrlSession))
    {
        appendWindowText(ctrlClnt->clientWnds->hOutput,
            "Control session already running\r\n");
        return;
    }

    // try to connect, and print outcome
    int returnCode = clientConnectTCP(client, hostIp, port);
    switch(returnCode)
    {
        case NORMAL_SUCCESS:
            sprintf_s(output, "Connecting to \"%s:%s\"...\r\n",
                hostIp, hostPort);
            appendWindowText(ctrlClnt->clientWnds->hOutput, output);
            break;
        default:
            sprintf_s(output, "Failed to connect: %s\r\n", rctoa(returnCode));
            appendWindowText(ctrlClnt->clientWnds->hOutput, output);
            break;
    }
}

// good
void ctrlClntStartTest(Client* client)
{
    char output[MAX_STRING_LEN];    // temporary buffer for output

    // parse user pointer
    CtrlClnt* ctrlClnt = (CtrlClnt*) client->usrPtr;

    // start the test if the ctrlSession is initialized
    if(ctrlClnt->ctrlSession.usrPtr != 0)
    {
        sprintf_s(output, "starting test...\r\n");
        appendWindowText(ctrlClnt->clientWnds->hOutput, output);
        ctrlClntSessionStartTest(&ctrlClnt->ctrlSession);
    }
    else
    {
        sprintf_s(output, "no existing control session; cannot start test.\r\n");
        appendWindowText(ctrlClnt->clientWnds->hOutput, output);
    }
}

// good
void ctrlClientSendChat(Client* client)
{
    char message[MAX_STRING_LEN];   // contains the user's chat message
    char output[MAX_STRING_LEN];    // temporary buffer for output

    // parse user pointer
    CtrlClnt* ctrlClnt = (CtrlClnt*) client->usrPtr;

    // only send the message if the control session is initialized
    if(ctrlClnt->ctrlSession.usrPtr != 0)
    {
        // get user's chat message
        GetWindowText(ctrlClnt->clientWnds->hInput, message, MAX_STRING_LEN);
        sprintf_s(output, "Sending: %s\r\n", message);
        appendWindowText(ctrlClnt->clientWnds->hOutput, output);

        // send the chat message
        sessionSendCtrlMsg(&ctrlClnt->ctrlSession, MSG_CHAT, message,
            strlen(message));
    }
    else
    {
        sprintf_s(output, "no existing control session; cannot send message.\r\n");
        appendWindowText(ctrlClnt->clientWnds->hOutput, output);
    }
}

// good
void ctrlClntDisonnect(Client* client)
{
    char output[MAX_STRING_LEN];    // temporary buffer for output
    int returnCode;                 // return code of sessionClose calls

    // parse user pointer
    CtrlClnt* ctrlClnt = (CtrlClnt*) client->usrPtr;

    // close the control session
    returnCode = sessionClose(&ctrlClnt->ctrlSession);
    switch(returnCode)
    {
        case NORMAL_SUCCESS:
            sprintf_s(output, "Control session stopped\r\n");
            appendWindowText(ctrlClnt->clientWnds->hOutput, output);
            break;
        default:
            sprintf_s(output, "Failed to stop the Control Session: %s\r\n",
                rctoa(returnCode));
            appendWindowText(ctrlClnt->clientWnds->hOutput, output);
            break;
    }

    // close the test session
    returnCode = sessionClose(&ctrlClnt->testSession);
    switch(returnCode)
    {
        case NORMAL_SUCCESS:
            sprintf_s(output, "Test session stopped\r\n");
            appendWindowText(ctrlClnt->clientWnds->hOutput, output);
            break;
        default:
            sprintf_s(output, "Failed to stop the Test Session: %s\r\n",
                rctoa(returnCode));
            appendWindowText(ctrlClnt->clientWnds->hOutput, output);
            break;
    }
}
