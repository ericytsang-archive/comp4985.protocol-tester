#include "ControlClient.h"

static void onConnectCtrl(Client*, SOCKET, sockaddr_in);
static void onConnectTest(Client*, SOCKET, sockaddr_in);
static void onError(Client*, int, int);

static void ctrlClntGetRemoteAddress(Client*);
static void ctrlClntGetCtrlPort(Client*);
static void ctrlClntGetTestPort(Client*);
static void ctrlClntGetTestPacketSize(Client*);
static void ctrlClntGetFilePath(Client*);
static void ctrlClntGetPacketsToSend(Client*);

// good
void ctrlClntInit(Client* client, ClientWnds* clientWnds)
{
    clientInit(client);
    client->usrPtr    = malloc(sizeof(CtrlClnt));
    client->onConnect = 0;
    client->onError   = onError;

    CtrlClnt* ctrlClnt       = (CtrlClnt*) client->usrPtr;
    ctrlClnt->clientWnds     = clientWnds;
    ctrlClnt->client         = client;
    ctrlClnt->ctrlSession    = 0;
    ctrlClnt->testSession    = 0;
    ctrlClnt->testPort       = 0;
    ctrlClnt->testProtocol   = MODE_UNDEFINED;
    ctrlClnt->testPacketSize = 0;
    ctrlClnt->dataSource     = 0;
    ctrlClnt->filePath[0]    = 0;
    ctrlClnt->packetsToSend  = 0;
    ctrlClnt->bytesToSend    = 0;
}

// good
void ctrlClntSetTestProtocol(Client* client, int testProtocol)
{
    // parse user pointer
    CtrlClnt* ctrlClnt = (CtrlClnt*) client->usrPtr;

    // set test protocol
    ctrlClnt->testProtocol = testProtocol;
}

// good
void ctrlClntSetDataSource(Client* client, int dataSource)
{
    // parse user pointer
    CtrlClnt* ctrlClnt = (CtrlClnt*) client->usrPtr;

    // set data source
    ctrlClnt->dataSource = dataSource;
}

// todo: get information about the test, and then begin the test
void ctrlClntStartTest(Client* client)
{
    char output[MAX_STRING_LEN];    // temporary buffer for output

    // parse user pointer
    CtrlClnt* ctrlClnt = (CtrlClnt*) client->usrPtr;

    // abort the test if there is no control session
    if(!ctrlClnt->ctrlSession)
    {
        sprintf_s(output, "No existing control session; cannot start test.\r\n");
        appendWindowText(ctrlClnt->clientWnds->hOutput, output);
        return;
    }

    // abort connection if there is currently a control session in progress
    if(ctrlClnt->testSession)
    {
        sprintf_s(output, "Test session already in progress...\r\n");
        appendWindowText(ctrlClnt->clientWnds->hOutput, output);
        return;
    }

    // get and parse user input
    ctrlClntGetTestPort(client);
    ctrlClntGetTestPacketSize(client);
    ctrlClntGetFilePath(client);
    ctrlClntGetPacketsToSend(client);

    // start the test
    sprintf_s(output, "Starting test...\r\n");
    appendWindowText(ctrlClnt->clientWnds->hOutput, output);
    ctrlClntSessionStartTest(ctrlClnt->ctrlSession);
}

// get all the information needed to connect to the remote host, and connect to them
void ctrlClntConnectCtrl(Client* client)
{
    char output[MAX_STRING_LEN];    // temporary buffer for output

    // parse user pointer
    CtrlClnt* ctrlClnt = (CtrlClnt*) client->usrPtr;

    // abort connection if we're still connecting from a previous call
    if(clientIsConnecting(client))
    {
        sprintf_s(output, "Connection still in progress...\r\n");
        appendWindowText(ctrlClnt->clientWnds->hOutput, output);
        return;
    }

    // abort connection if there is currently a control session in progress
    if(ctrlClnt->ctrlSession)
    {
        sprintf_s(output, "Control session already in progress...\r\n");
        appendWindowText(ctrlClnt->clientWnds->hOutput, output);
        return;
    }

    // get and parse user input
    ctrlClntGetRemoteAddress(client);
    ctrlClntGetCtrlPort(client);

    // try to connect, and print outcome
    client->onConnect = onConnectCtrl;
    int returnCode = clientConnectTCP(client,
        ctrlClnt->remoteAddress, ctrlClnt->ctrlPort);
    switch(returnCode)
    {
    case NORMAL_SUCCESS:
        sprintf_s(output, "Connecting to \"%s:%d\"...\r\n",
            ctrlClnt->remoteAddress, ctrlClnt->ctrlPort);
        appendWindowText(ctrlClnt->clientWnds->hOutput, output);
        break;
    default:
        sprintf_s(output, "Failed to connect: %s\r\n", rctoa(returnCode));
        appendWindowText(ctrlClnt->clientWnds->hOutput, output);
        break;
    }
}

// get all the information needed to connect to the remote host, and connect to them
void ctrlClntConnectTest(Client* client)
{
    char output[MAX_STRING_LEN];    // temporary buffer for output
    int returnCode;

    // parse user pointer
    CtrlClnt* ctrlClnt = (CtrlClnt*) client->usrPtr;

    // abort connection if we're still connecting from a previous call
    if(clientIsConnecting(client))
    {
        sprintf_s(output, "Connection still in progress...\r\n");
        appendWindowText(ctrlClnt->clientWnds->hOutput, output);
        return;
    }

    // try to connect, and print outcome
    client->onConnect = onConnectTest;

    switch(ctrlClnt->testProtocol)
    {
    case MODE_TCP:
        returnCode = clientConnectTCP(client, ctrlClnt->remoteAddress,
            ctrlClnt->testPort);
        break;
    case MODE_UDP:
        returnCode = clientConnectUDP(client, ctrlClnt->remoteAddress,
            ctrlClnt->testPort);
        break;
    }

    switch(returnCode)
    {
    case NORMAL_SUCCESS:
        sprintf_s(output, "Connecting to \"%s:%d\" for test...\r\n",
            ctrlClnt->remoteAddress, htons(ctrlClnt->testPort));
        appendWindowText(ctrlClnt->clientWnds->hOutput, output);
        break;
    default:
        sprintf_s(output, "Failed to connect: %s\r\n", rctoa(returnCode));
        appendWindowText(ctrlClnt->clientWnds->hOutput, output);
        break;
    }
}

// send chat message through control line
void ctrlClientSendChat(Client* client)
{
    char message[MAX_STRING_LEN];   // contains the user's chat message
    char output[MAX_STRING_LEN];    // temporary buffer for output

    // parse user pointer
    CtrlClnt* ctrlClnt = (CtrlClnt*) client->usrPtr;

    // only send the message if the control session is initialized
    if(ctrlClnt->ctrlSession)
    {
        // get user's chat message
        GetWindowText(ctrlClnt->clientWnds->hInput, message, MAX_STRING_LEN);
        sprintf_s(output, "Sending: %s\r\n", message);
        appendWindowText(ctrlClnt->clientWnds->hOutput, output);

        // send the chat message
        sessionSendCtrlMsg(ctrlClnt->ctrlSession, MSG_CHAT, message,
            strlen(message));
    }
    else
    {
        sprintf_s(output, "No existing control session; cannot send message.\r\n");
        appendWindowText(ctrlClnt->clientWnds->hOutput, output);
    }
}

// disconnects the control and test sessions, and resets the pointers to null
void ctrlClntDisonnect(Client* client)
{
    char output[MAX_STRING_LEN];    // temporary buffer for output
    int returnCode;                 // return code of sessionClose calls

    // parse user pointer
    CtrlClnt* ctrlClnt = (CtrlClnt*) client->usrPtr;
    
    // close the test session
    if(ctrlClnt->testSession)
    {
        returnCode = sessionClose(ctrlClnt->testSession);
        switch(returnCode)
        {
        case NORMAL_SUCCESS:
            sprintf_s(output, "Test session stopped\r\n");
            appendWindowText(ctrlClnt->clientWnds->hOutput, output);
        
            // tell the server to shup off their udp session as well if it's udp
            //if(ctrlClnt->testProtocol == MODE_UDP)
            //{
            //    sessionSendCtrlMsg(ctrlClnt->ctrlSession, MSG_STOP_TEST, "a", 1);
            //}
            break;
        default:
            sprintf_s(output, "Failed to stop the test session: %s\r\n",
                rctoa(returnCode));
            appendWindowText(ctrlClnt->clientWnds->hOutput, output);
            break;
        }
    }
    else
    {
        sprintf_s(output, "No existing test session; can't stop test session\r\n");
        appendWindowText(ctrlClnt->clientWnds->hOutput, output);
    }

    // close the control session
    if(ctrlClnt->ctrlSession)
    {
        returnCode = sessionClose(ctrlClnt->ctrlSession);
        switch(returnCode)
        {
        case NORMAL_SUCCESS:
            sprintf_s(output, "Control session stopped\r\n");
            appendWindowText(ctrlClnt->clientWnds->hOutput, output);
            break;
        default:
            sprintf_s(output, "Failed to stop the control session: %s\r\n",
                rctoa(returnCode));
            appendWindowText(ctrlClnt->clientWnds->hOutput, output);
            break;
        }
    }
    else
    {
        sprintf_s(output, "No existing control session; can't stop control session\r\n");
        appendWindowText(ctrlClnt->clientWnds->hOutput, output);
    }
}

// prints a message, creates a control session, and points our pointer at it.
static void onConnectCtrl(Client* client, SOCKET clientSock, sockaddr_in clientAddr)
{
    char output[MAX_STRING_LEN];    // temporary output buffer

    // parse user pointer
    CtrlClnt* ctrlClnt = (CtrlClnt*) client->usrPtr;

    // print connected message
    sprintf_s(output, "Control connected to \"%s:%d\"\r\n",
        inet_ntoa(clientAddr.sin_addr),
        htons(clientAddr.sin_port));
    appendWindowText(ctrlClnt->clientWnds->hOutput, output);

    // create and start the client control or test session depending on port
    ctrlClnt->ctrlSession = (Session*) malloc(sizeof(Session));
    ctrlClntSessionInit(ctrlClnt->ctrlSession, ctrlClnt, clientSock, clientAddr);
    sessionStart(ctrlClnt->ctrlSession);
}

// prints a message, creates a test session, and points our pointer at it.
static void onConnectTest(Client* client, SOCKET clientSock, sockaddr_in clientAddr)
{
    char output[MAX_STRING_LEN];    // temporary output buffer
    char packet[65536];

    // parse user pointer
    CtrlClnt* ctrlClnt = (CtrlClnt*) client->usrPtr;

    // print connected message
    sprintf_s(output, "Test connected to \"%s:%d\"\r\n",
        inet_ntoa(clientAddr.sin_addr),
        htons(clientAddr.sin_port));
    appendWindowText(ctrlClnt->clientWnds->hOutput, output);

    // create and start the client control or test session depending on port
    ctrlClnt->testSession = (Session*) malloc(sizeof(Session));
    testClntSessionInit(ctrlClnt->testSession, ctrlClnt, clientSock,
        clientAddr);
    sessionStart(ctrlClnt->testSession);

    Sleep(100);

    for (int i = 0; i < ctrlClnt->packetsToSend
        && sessionIsRunning(ctrlClnt->testSession); ++i)
    {
        if(i % 100 == 0)
        {
            sprintf_s(output, "sent %d of %d packets\r\n", i, ctrlClnt->packetsToSend);
            appendWindowText(ctrlClnt->clientWnds->hOutput, output);
        }
        sessionSend(ctrlClnt->testSession, packet, ctrlClnt->testPacketSize);
    }

    Sleep(2000);

    if(ctrlClnt->testSession)
    {
        sessionClose(ctrlClnt->testSession);
    }
    if(ctrlClnt->ctrlSession && ctrlClnt->testProtocol == MODE_UDP)
    {
        sessionSendCtrlMsg(ctrlClnt->ctrlSession, MSG_STOP_TEST, "a", 1);
    }
}

// report the error to the screen
static void onError(Client* client, int errCode, int winErrCode)
{
    char output[MAX_STRING_LEN];    // temporary output buffer

    // parse user pointer
    CtrlClnt* ctrlClnt = (CtrlClnt*) client->usrPtr;

    // print failure message to the screen
    sprintf_s(output, "Failed to connect: %s - %d\r\n",
        rctoa(errCode), winErrCode);
    appendWindowText(ctrlClnt->clientWnds->hOutput, output);
}

static void ctrlClntGetRemoteAddress(Client* client)
{
    char input[MAX_STRING_LEN];     // holds user's input

    // parse user pointer
    CtrlClnt* ctrlClnt = (CtrlClnt*) client->usrPtr;

    GetWindowText(ctrlClnt->clientWnds->hIpHost, input, MAX_STRING_LEN);
    strncpy_s(ctrlClnt->remoteAddress, input, MAX_STRING_LEN);
}

static void ctrlClntGetCtrlPort(Client* client)
{
    char input[MAX_STRING_LEN];     // holds user's input

    // parse user pointer
    CtrlClnt* ctrlClnt = (CtrlClnt*) client->usrPtr;

    GetWindowText(ctrlClnt->clientWnds->hCtrlPort, input, MAX_STRING_LEN);
    ctrlClnt->ctrlPort = atoi(input);
}

static void ctrlClntGetTestPort(Client* client)
{
    char input[MAX_STRING_LEN];     // holds user's input

    // parse user pointer
    CtrlClnt* ctrlClnt = (CtrlClnt*) client->usrPtr;

    GetWindowText(ctrlClnt->clientWnds->hTestPort, input, MAX_STRING_LEN);
    ctrlClnt->testPort = atoi(input);
}

static void ctrlClntGetTestPacketSize(Client* client)
{
    char input[MAX_STRING_LEN];     // holds user's input

    // parse user pointer
    CtrlClnt* ctrlClnt = (CtrlClnt*) client->usrPtr;

    GetWindowText(ctrlClnt->clientWnds->hPacketSize, input, MAX_STRING_LEN);
    ctrlClnt->testPacketSize = atoi(input);
}

static void ctrlClntGetFilePath(Client* client)
{
    char input[MAX_STRING_LEN];     // holds user's input

    // parse user pointer
    CtrlClnt* ctrlClnt = (CtrlClnt*) client->usrPtr;

    GetWindowText(ctrlClnt->clientWnds->hFile, input, MAX_STRING_LEN);
    strncpy_s(ctrlClnt->filePath, input, MAX_STRING_LEN);
}

static void ctrlClntGetPacketsToSend(Client* client)
{
    char input[MAX_STRING_LEN];     // holds user's input

    // parse user pointer
    CtrlClnt* ctrlClnt = (CtrlClnt*) client->usrPtr;

    GetWindowText(ctrlClnt->clientWnds->hPacketCount, input, MAX_STRING_LEN);
    ctrlClnt->packetsToSend = atoi(input);
}
