/**
 * [c description]
 *
 * @sourceFile [function_header] [class_header] [method_header] source_file_name
 *
 * @program    [function_header] [class_header] [method_header] executable_file_name
 *
 * @class      [function_header]
 *
 * @function   [class_header] [method_header]
 *
 * @date       2015-01-26
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 *
 * @return     [description]
 */
#define STRICT
#include "Main.h"

static void makeClientWindows(HWND, ClientWnds*);
static void updateClientWindows(HWND, ClientWnds*);
static void showClientWindows(ClientWnds*);
static void hideClientWindows(ClientWnds*);

static void makeServerWindows(HWND, ServerWnds*);
static void updateServerWindows(HWND, ServerWnds*);
static void showServerWindows(ServerWnds*);
static void hideServerWindows(ServerWnds*);

static void makeCommonWindows(HWND, CommonWnds*);
static void updateCommonWindows(HWND, CommonWnds*);

/**
 * [WinMain description]
 *
 * @function   [class_header] [method_header]
 *
 * @date       2015-01-26
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 *
 * @signature  [some_headers_only] [class_header] [file_header]
 *
 * @param      hInst [description]
 * @param      hPrevInst [description]
 * @param      lspszCmdParam [description]
 * @param      nCmdShow [description]
 *
 * @return     [description]
 */
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lspszCmdParam, int nCmdShow)
{
    TCHAR Title[] = TEXT(APP_NAME);
    MSG Msg;
    WNDCLASSEX Wcl;
    WSADATA wsaData;
    HWND hWnd;

    WSAStartup(MAKEWORD(2, 2), &wsaData);

    Wcl.cbSize = sizeof(WNDCLASSEX);
    Wcl.style = 0;
    Wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    Wcl.hIconSm = NULL;
    Wcl.hCursor = LoadCursor(NULL, IDC_ARROW);
    Wcl.lpfnWndProc = WndProc;
    Wcl.hInstance = hInst;
    Wcl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    Wcl.lpszClassName = Title;
    Wcl.lpszMenuName = TEXT("MENUBAR");
    Wcl.cbClsExtra = 0;
    Wcl.cbWndExtra = 0;

    if (!RegisterClassEx(&Wcl))
        return 0;

    hWnd = CreateWindow((LPCSTR)Title, (LPCSTR)Title, WS_OVERLAPPEDWINDOW, 0, 0, 850, 500, NULL, NULL, hInst, NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&Msg, NULL, 0, 0))
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    return Msg.wParam;
}
/**
 * [WndProc description]
 *
 * @function   [class_header] [method_header]
 *
 * @date       2015-01-26
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 *
 * @signature  [some_headers_only] [class_header] [file_header]
 *
 * @param      hWnd [description]
 * @param      Message [description]
 * @param      wParam [description]
 * @param      lParam [description]
 *
 * @return     [description]
 */
LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    static ClientWnds clientWnds;
    static ServerWnds serverWnds;
    static CommonWnds commonWnds;

    static Server server;
    static Client client;
    static Session ctrlSession;
    static Session testSession;

    static LinkedList ctrlSessions;

    static CtrlClnt ctrlClnt;
    static CtrlSvr ctrlSvr;

    static int currMode = MODE_CLIENT;

    switch (Message)
    {
        case WM_CREATE:
        {
            makeCommonWindows(hWnd, &commonWnds);
            makeClientWindows(hWnd, &clientWnds);
            makeServerWindows(hWnd, &serverWnds);

            hideServerWindows(&serverWnds);

            serverInit(&server);
            server.usrPtr    = &ctrlSvr;
            server.onClose   = ctrlSvrOnClose;
            server.onConnect = ctrlSvrOnConnect;
            server.onError   = ctrlSvrOnError;

            clientInit(&client);
            client.usrPtr    = &ctrlClnt;
            client.onConnect = ctrlClntOnConnect;
            client.onError   = ctrlClntOnError;

            memset(&ctrlSession, 0, sizeof(Session));
            ctrlSession._sessionThread = INVALID_HANDLE_VALUE;

            memset(&testSession, 0, sizeof(Session));
            testSession._sessionThread = INVALID_HANDLE_VALUE;

            linkedListInit(&ctrlSessions);

            ctrlClnt.clientWnds   = &clientWnds;
            ctrlClnt.ctrlSession  = &ctrlSession;
            ctrlClnt.testSession  = &testSession;
            ctrlClnt.testProtocol = MODE_UNDEFINED;
            ctrlClnt.testPort     = 0;

            ctrlSvr.serverWnds   = &serverWnds;
            ctrlSvr.ctrlSessions = &ctrlSessions;
            break;
        }
        case WM_DESTROY:
        {
            WSACleanup();
            PostQuitMessage(0);
            break;
        }
        case WM_SIZE:
        {
            updateCommonWindows(hWnd, &commonWnds);
            updateClientWindows(hWnd, &clientWnds);
            updateServerWindows(hWnd, &serverWnds);
            break;
        }
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case IDC_TCP:
                {
                    ctrlClnt.testProtocol = MODE_TCP;
                    break;
                }
                case IDC_UDP:
                {
                    ctrlClnt.testProtocol = MODE_UDP;
                    break;
                }
                case IDC_SEND_FILE:
                {
                    //////////
                    // TODO //
                    //////////
                    OutputDebugString("IDC_SEND_FILE\r\n");
                    break;
                }
                case IDC_SEND_GENERATED_DATA:
                {
                    //////////
                    // TODO //
                    //////////
                    OutputDebugString("IDC_SEND_GENERATED_DATA\r\n");
                    break;
                }
                case IDC_BROWSE_FILE:
                {
                    //////////
                    // TODO //
                    //////////
                    OutputDebugString("IDC_BROWSE_FILE\r\n");
                    break;
                }
                case IDC_CONNECT:
                {
                    char output[MAX_STRING_LEN];
                    char hostIp[MAX_STRING_LEN];
                    char hostPort[MAX_STRING_LEN];
                    unsigned int port;

                    if(sessionIsRunning(&ctrlSession))
                    {
                        appendWindowText(clientWnds.hOutput, "Control session already running\r\n");
                        break;
                    }

                    GetWindowText(clientWnds.hIpHost, hostIp, MAX_STRING_LEN);
                    GetWindowText(clientWnds.hCtrlPort, hostPort, MAX_STRING_LEN);

                    port = atoi(hostPort);

                    switch(clientConnectTCP(&client, hostIp, port))
                    {
                        case ALREADY_RUNNING_FAIL:
                        {
                            appendWindowText(clientWnds.hOutput, "Failed to connect: ALREADY_RUNNING_FAIL\r\n");
                            break;
                        }
                        case THREAD_FAIL:
                        {
                            appendWindowText(clientWnds.hOutput, "Failed to connect: THREAD_FAIL\r\n");
                            break;
                        }
                        case NORMAL_SUCCESS:
                        {
                            sprintf_s(output, "Connecting to \"%s:%s\" . . .\r\n", hostIp, hostPort);
                            appendWindowText(clientWnds.hOutput, output);
                            break;
                        }
                    }
                    break;
                }
                case IDC_DISCONNECT:
                {
                    char output[MAX_STRING_LEN];

                    switch(sessionClose(&ctrlSession))
                    {
                        case ALREADY_STOPPED_FAIL:
                            sprintf_s(output, "Failed to stop the Control Session: ALREADY_STOPPED_FAIL\r\n");
                            appendWindowText(clientWnds.hOutput, output);
                            break;
                        case NORMAL_SUCCESS:
                            sprintf_s(output, "Control session stopped\r\n");
                            appendWindowText(clientWnds.hOutput, output);
                            break;
                    }
                    switch(sessionClose(&testSession))
                    {
                        case ALREADY_STOPPED_FAIL:
                            sprintf_s(output, "Failed to stop the Test Session: ALREADY_STOPPED_FAIL\r\n");
                            appendWindowText(clientWnds.hOutput, output);
                            break;
                        case NORMAL_SUCCESS:
                            sprintf_s(output, "Test session stopped\r\n");
                            appendWindowText(clientWnds.hOutput, output);
                            break;
                    }
                    break;
                }
                case IDC_TEST:
                {
                    char msgType;
                    int msgLen;

                    char port[MAX_STRING_LEN];
                    GetWindowText(clientWnds.hTestPort, port, MAX_STRING_LEN);
                    ctrlClnt.testPort = atoi(port);

                    msgType = MSG_SET_PORT;
                    msgLen = sizeof(ctrlClnt.testPort);
                    sessionSend(&ctrlSession, &msgType, PACKET_LEN_TYPE);
                    sessionSend(&ctrlSession, &msgLen, PACKET_LEN_LENGTH);
                    sessionSend(&ctrlSession, &ctrlClnt.testPort, msgLen);

                    msgType = MSG_SET_PROTOCOL;
                    msgLen = sizeof(ctrlClnt.testProtocol);
                    sessionSend(&ctrlSession, &msgType, PACKET_LEN_TYPE);
                    sessionSend(&ctrlSession, &msgLen, PACKET_LEN_LENGTH);
                    sessionSend(&ctrlSession, &ctrlClnt.testProtocol, msgLen);

                    msgType = MSG_START_TEST;
                    msgLen = 1;
                    sessionSend(&ctrlSession, &msgType, PACKET_LEN_TYPE);
                    sessionSend(&ctrlSession, &msgLen, PACKET_LEN_LENGTH);
                    sessionSend(&ctrlSession, &msgLen, msgLen);
                    break;
                }
                case IDC_SEND_MESSAGE:
                {
                    char message[MAX_STRING_LEN];
                    char output[MAX_STRING_LEN];

                    switch(currMode)
                    {
                        case MODE_CLIENT:
                        {
                            GetWindowText(clientWnds.hInput, message, MAX_STRING_LEN);
                            sprintf_s(output, "Sending: %s\r\n", message);
                            appendWindowText(clientWnds.hOutput, output);

                            char msgType = MSG_CHAT;
                            int msgLen = strlen(message);
                            sessionSend(&ctrlSession, &msgType, PACKET_LEN_TYPE);
                            sessionSend(&ctrlSession, &msgLen, PACKET_LEN_LENGTH);
                            sessionSend(&ctrlSession, message, strlen(message));
                            break;
                        }
                        case MODE_SERVER:
                        {
                            GetWindowText(serverWnds.hInput, message, MAX_STRING_LEN);
                            sprintf_s(output, "Sending: %s\r\n", message);
                            appendWindowText(serverWnds.hOutput, output);

                            Node* curr;
                            for(curr = ctrlSessions.head; curr != 0; curr = curr->next)
                            {
                                char msgType = MSG_CHAT;
                                int msgLen = strlen(message);
                                sessionSend((Session*) curr->data, &msgType, PACKET_LEN_TYPE);
                                sessionSend((Session*) curr->data, &msgLen, PACKET_LEN_LENGTH);
                                sessionSend((Session*) curr->data, message, strlen(message));
                            }
                            break;
                        }
                    }
                    break;
                }
                case IDC_MODE_SERVER:
                {
                    currMode = MODE_SERVER;
                    hideClientWindows(&clientWnds);
                    showServerWindows(&serverWnds);
                    break;
                }
                case IDC_MODE_CLIENT:
                {
                    currMode = MODE_CLIENT;
                    hideServerWindows(&serverWnds);
                    showClientWindows(&clientWnds);
                    break;
                }
                case IDC_HELP:
                {
                    OutputDebugString("IDC_HELP\r\n");
                    break;
                }
                case IDC_START_SERVER:
                {
                    char output[MAX_STRING_LEN];
                    char portString[MAX_STRING_LEN];

                    GetWindowText(serverWnds.hPort, portString, MAX_STRING_LEN);
                    unsigned short port = atoi(portString);
                    serverSetPort(&server, port);

                    switch(serverStart(&server))
                    {
                        case ALREADY_RUNNING_FAIL:
                        {
                            appendWindowText(serverWnds.hOutput, "Failed to start server: SERVER_ALREADY_RUNNING_FAIL\r\n");
                            break;
                        }
                        case THREAD_FAIL:
                        {
                            appendWindowText(serverWnds.hOutput, "Failed to start server: THREAD_FAIL\r\n");
                            break;
                        }
                        case NORMAL_SUCCESS:
                        {
                            sprintf_s(output, "Server started; listening on port %d\r\n", port);
                            appendWindowText(serverWnds.hOutput, output);
                            break;
                        }
                    }
                    break;
                }
                case IDC_STOP_SERVER:
                {
                    char output[MAX_STRING_LEN];

                    switch(serverStop(&server))
                    {
                        case ALREADY_STOPPED_FAIL:
                            sprintf_s(output, "Failed to stop the Server: ALREADY_STOPPED_FAIL\r\n");
                            appendWindowText(clientWnds.hOutput, output);
                            break;
                        case NORMAL_SUCCESS:
                            sprintf_s(output, "Server stopped\r\n");
                            appendWindowText(clientWnds.hOutput, output);
                            break;
                    }
                    break;
                }
                break;
            }
        default:
        {
            return DefWindowProc(hWnd, Message, wParam, lParam);
        }
    }
    return 0;
}

char* rctoa(int returnCode)
{
    static char string[MAX_STRING_LEN];

    switch(returnCode)
    {
        case NORMAL_SUCCESS:
            sprintf_s(string, "NORMAL_SUCCESS");
            break;
        case UNKNOWN_FAIL:
            sprintf_s(string, "UNKNOWN_FAIL");
            break;
        case THREAD_FAIL:
            sprintf_s(string, "THREAD_FAIL");
            break;
        case SOCKET_FAIL:
            sprintf_s(string, "SOCKET_FAIL");
            break;
        case BIND_FAIL:
            sprintf_s(string, "BIND_FAIL");
            break;
        case ACCEPT_FAIL:
            sprintf_s(string, "ACCEPT_FAIL");
            break;
        case ALREADY_RUNNING_FAIL:
            sprintf_s(string, "ALREADY_RUNNING_FAIL");
            break;
        case ALREADY_STOPPED_FAIL:
            sprintf_s(string, "ALREADY_STOPPED_FAIL");
            break;
        case UNKNOWN_IP_FAIL:
            sprintf_s(string, "UNKNOWN_IP_FAIL");
            break;
        case CONNECT_FAIL:
            sprintf_s(string, "CONNECT_FAIL");
            break;
        case RECV_FAIL:
            sprintf_s(string, "RECV_FAIL");
            break;
        default:
            sprintf_s(string, "UNKNOWN_RETURN_CODE");
            break;
    }

    return string;
}

static void updateCommonWindows(HWND hwnd, CommonWnds* commonWnds)
{
    RECT windowRect;
    GetClientRect(hwnd, &windowRect);

    MoveWindow(commonWnds->hBackground, 0, 0, windowRect.right, windowRect.bottom, TRUE);
}
/**
 * [updateDisplay description]
 *
 * @function   [class_header] [method_header]
 *
 * @date       2015-01-26
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 *
 * @signature  [some_headers_only] [class_header] [file_header]
 *
 * @param      hwnd [description]
 * @param      clientWnds [description]
 */
static void updateServerWindows(HWND hwnd, ServerWnds* serverWnds)
{
    RECT windowRect;
    GetClientRect(hwnd, &windowRect);

    MoveWindow(serverWnds->hStart,
        PADDING, windowRect.bottom-PADDING-BUTTON_HEIGHT,
        BUTTON_WIDTH, BUTTON_HEIGHT, TRUE);
    MoveWindow(serverWnds->hStop,
        BUTTON_WIDTH+PADDING*2, windowRect.bottom-PADDING-BUTTON_HEIGHT,
        BUTTON_WIDTH, BUTTON_HEIGHT, TRUE);
    MoveWindow(serverWnds->hOutput,
        GROUP_WIDTH+PADDING*2, PADDING,
        windowRect.right-(GROUP_WIDTH+PADDING*3), windowRect.bottom-BUTTON_HEIGHT-PADDING*3, TRUE);
    MoveWindow(serverWnds->hInput,
        GROUP_WIDTH+PADDING*2, windowRect.bottom-PADDING-BUTTON_HEIGHT,
        windowRect.right-(BUTTON_WIDTH+GROUP_WIDTH+PADDING*3), BUTTON_HEIGHT, TRUE);
    MoveWindow(serverWnds->hSend,
        windowRect.right-BUTTON_WIDTH-PADDING, windowRect.bottom-BUTTON_HEIGHT-PADDING,
        BUTTON_WIDTH, BUTTON_HEIGHT, TRUE);

    RedrawWindow(serverWnds->hOutput, NULL, NULL, RDW_INVALIDATE);
    RedrawWindow(serverWnds->hStart, NULL, NULL, RDW_INVALIDATE);
    RedrawWindow(serverWnds->hStop, NULL, NULL, RDW_INVALIDATE);
    RedrawWindow(serverWnds->hInput, NULL, NULL, RDW_INVALIDATE);
    RedrawWindow(serverWnds->hSend, NULL, NULL, RDW_INVALIDATE);
}
/**
 * [updateDisplay description]
 *
 * @function   [class_header] [method_header]
 *
 * @date       2015-01-26
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 *
 * @signature  [some_headers_only] [class_header] [file_header]
 *
 * @param      hwnd [description]
 * @param      clientWnds [description]
 */
static void updateClientWindows(HWND hwnd, ClientWnds* clientWnds)
{
    RECT windowRect;
    GetClientRect(hwnd, &windowRect);

    MoveWindow(clientWnds->hConnect,
        PADDING, windowRect.bottom-PADDING-BUTTON_HEIGHT,
        BUTTON_WIDTH, BUTTON_HEIGHT, TRUE);
    MoveWindow(clientWnds->hDisconnect,
        BUTTON_WIDTH+PADDING*2, windowRect.bottom-PADDING-BUTTON_HEIGHT,
        BUTTON_WIDTH, BUTTON_HEIGHT, TRUE);
    MoveWindow(clientWnds->hTest,
        BUTTON_WIDTH*2+PADDING*3, windowRect.bottom-PADDING-BUTTON_HEIGHT,
        BUTTON_WIDTH, BUTTON_HEIGHT, TRUE);
    MoveWindow(clientWnds->hOutput,
        GROUP_WIDTH+PADDING*2, PADDING,
        windowRect.right-(GROUP_WIDTH+PADDING*3), windowRect.bottom-BUTTON_HEIGHT-PADDING*3, TRUE);
    MoveWindow(clientWnds->hInput,
        GROUP_WIDTH+PADDING*2, windowRect.bottom-PADDING-BUTTON_HEIGHT,
        windowRect.right-(BUTTON_WIDTH+GROUP_WIDTH+PADDING*3), BUTTON_HEIGHT, TRUE);
    MoveWindow(clientWnds->hSend,
        windowRect.right-BUTTON_WIDTH-PADDING, windowRect.bottom-BUTTON_HEIGHT-PADDING,
        BUTTON_WIDTH, BUTTON_HEIGHT, TRUE);

    RedrawWindow(clientWnds->hOutput, NULL, NULL, RDW_INVALIDATE);
    RedrawWindow(clientWnds->hTest, NULL, NULL, RDW_INVALIDATE);
    RedrawWindow(clientWnds->hConnect, NULL, NULL, RDW_INVALIDATE);
    RedrawWindow(clientWnds->hDisconnect, NULL, NULL, RDW_INVALIDATE);
    RedrawWindow(clientWnds->hInput, NULL, NULL, RDW_INVALIDATE);
    RedrawWindow(clientWnds->hSend, NULL, NULL, RDW_INVALIDATE);
}

static void makeCommonWindows(HWND hWnd, CommonWnds* commonWnds)
{
    commonWnds->hBackground = CreateWindowEx(WS_EX_CLIENTEDGE,
        "Static", "", WS_CHILD | WS_VISIBLE | WS_DISABLED,
        0, 0, 0, 0,
        hWnd, NULL,
        GetModuleHandle(NULL), NULL);
}
/**
 * [makeServerWindows description]
 *
 * @function   [class_header] [method_header]
 *
 * @date       2015-01-26
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 *
 * @signature  [some_headers_only] [class_header] [file_header]
 *
 * @param      serverWnds [description]
 */
static void makeServerWindows(HWND hWnd, ServerWnds* serverWnds)
{
    int ServerOptionsGroupX = PADDING;
    int ServerOptionsGroupY = PADDING;
    int ServerOptionsGroupW = GROUP_WIDTH;
    int ServerOptionsGroupH = PADDING_TOP_GROUPBOX+PADDING*3+TEXT_HEIGHT*2;

    RECT clientRect;
    GetClientRect(hWnd, &clientRect);

    // group boxes
    serverWnds->hSvrOptionsBroupBox = CreateWindowEx(NULL,
        "Button", "Server Options",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        ServerOptionsGroupX, ServerOptionsGroupY,
        ServerOptionsGroupW, ServerOptionsGroupH,
        hWnd, NULL,
        GetModuleHandle(NULL), NULL);
    serverWnds->hStart = CreateWindowEx(WS_EX_CLIENTEDGE,
        "Button", "Start Server", WS_CHILD | WS_VISIBLE,
        0, 0, 0, 0,
        hWnd, (HMENU)IDC_START_SERVER,
        GetModuleHandle(NULL), NULL);
    serverWnds->hStop = CreateWindowEx(WS_EX_CLIENTEDGE,
        "Button", "Stop Server", WS_CHILD | WS_VISIBLE,
        0, 0, 0, 0,
        hWnd, (HMENU)IDC_STOP_SERVER,
        GetModuleHandle(NULL), NULL);
    serverWnds->hOutput = CreateWindowEx(WS_EX_CLIENTEDGE,
        "Edit", "",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY,
        0, 0, 0, 0, hWnd, NULL,
        GetModuleHandle(NULL), NULL);
    serverWnds->hInput = CreateWindowEx(WS_EX_CLIENTEDGE,
        "Edit", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        0, 0, 0, 0, hWnd, NULL,
        GetModuleHandle(NULL), NULL);
    serverWnds->hSend = CreateWindowEx(WS_EX_CLIENTEDGE,
        "Button", "Send", WS_CHILD | WS_VISIBLE,
        0, 0, 0, 0,
        hWnd, (HMENU)IDC_SEND_MESSAGE,
        GetModuleHandle(NULL), NULL);

    // remote address controls
    serverWnds->hCtrlPortLabel = CreateWindowEx(NULL,
        "Static", "Control Port:", WS_CHILD | WS_VISIBLE,
        ServerOptionsGroupX+PADDING, ServerOptionsGroupY+PADDING_TOP_GROUPBOX+PADDING,
        COLUMN_1_WIDTH, TEXT_HEIGHT,
        hWnd, NULL,
        GetModuleHandle(NULL), NULL);
    serverWnds->hFileLabel = CreateWindowEx(NULL,
        "Static", "Choose File:",
        WS_CHILD | WS_VISIBLE,
        ServerOptionsGroupX+PADDING, ServerOptionsGroupY+PADDING_TOP_GROUPBOX+PADDING*2+TEXT_HEIGHT,
        COLUMN_1_WIDTH, TEXT_HEIGHT,
        hWnd, NULL,
        GetModuleHandle(NULL), NULL);
    serverWnds->hPort = CreateWindowEx(WS_EX_CLIENTEDGE,
        "Edit", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        ServerOptionsGroupX+COLUMN_1_WIDTH+PADDING*2, ServerOptionsGroupY+PADDING_TOP_GROUPBOX+PADDING,
        COLUMN_2_WIDTH+COLUMN_3_WIDTH+PADDING, TEXT_HEIGHT,
        hWnd, NULL,
        GetModuleHandle(NULL), NULL);
    serverWnds->hFile = CreateWindowEx(WS_EX_CLIENTEDGE,
        "Edit", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        ServerOptionsGroupX+COLUMN_1_WIDTH+PADDING*2, ServerOptionsGroupY+PADDING_TOP_GROUPBOX+PADDING*2+TEXT_HEIGHT,
        COLUMN_2_WIDTH, TEXT_HEIGHT,
        hWnd, NULL,
        GetModuleHandle(NULL), NULL);
    serverWnds->hBrowseFile = CreateWindowEx(WS_EX_CLIENTEDGE,
        "Button", "...", WS_CHILD | WS_VISIBLE,
        ServerOptionsGroupX+COLUMN_1_WIDTH+COLUMN_2_WIDTH+PADDING*3, ServerOptionsGroupY+PADDING_TOP_GROUPBOX+PADDING*2+TEXT_HEIGHT,
        COLUMN_3_WIDTH, TEXT_HEIGHT,
        hWnd, (HMENU)IDC_BROWSE_FILE,
        GetModuleHandle(NULL), NULL);
}
/**
 * [makeClientWindows description]
 *
 * @function   [class_header] [method_header]
 *
 * @date       2015-01-26
 *
 * @revision   none
 *
 * @designer   Eric Tsang
 *
 * @programmer Eric Tsang
 *
 * @note       none
 *
 * @signature  [some_headers_only] [class_header] [file_header]
 *
 * @param      clientWnds [description]
 */
static void makeClientWindows(HWND hWnd, ClientWnds* clientWnds)
{
    int RemoteAddrGroupX = PADDING;
    int RemoteAddrGroupY = PADDING;
    int RemoteAddrGroupW = GROUP_WIDTH;
    int RemoteAddrGroupH = PADDING_TOP_GROUPBOX+PADDING*4+TEXT_HEIGHT*3;

    int ProtocolParamsGroupX = PADDING;
    int ProtocolParamsGroupY = RemoteAddrGroupY+RemoteAddrGroupH+PADDING;
    int ProtocolParamsGroupW = GROUP_WIDTH;
    int ProtocolParamsGroupH = PADDING_TOP_GROUPBOX+PADDING*3+TEXT_HEIGHT*2;

    int DataParamsGroupX = PADDING;
    int DataParamsGroupY = ProtocolParamsGroupY+ProtocolParamsGroupH+PADDING;;
    int DataParamsGroupW = GROUP_WIDTH;
    int DataParamsGroupH = PADDING_TOP_GROUPBOX+PADDING*6+TEXT_HEIGHT*5;

    RECT clientRect;
    GetClientRect(hWnd, &clientRect);

    // group boxes
    clientWnds->hRemoteAddress = CreateWindowEx(NULL,
        "Button", "Remote Address",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        RemoteAddrGroupX, RemoteAddrGroupY,
        RemoteAddrGroupW, RemoteAddrGroupH,
        hWnd, NULL,
        GetModuleHandle(NULL), NULL);
    clientWnds->hProtocolParameters = CreateWindowEx(NULL,
        "Button", "Protocol Parameters",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        ProtocolParamsGroupX, ProtocolParamsGroupY,
        ProtocolParamsGroupW, ProtocolParamsGroupH,
        hWnd, NULL,
        GetModuleHandle(NULL), NULL);
    clientWnds->hDataParameters = CreateWindowEx(NULL,
        "Button", "Data Parameters",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        DataParamsGroupX, DataParamsGroupY,
        DataParamsGroupW, DataParamsGroupH,
        hWnd, NULL,
        GetModuleHandle(NULL), NULL);
    clientWnds->hConnect = CreateWindowEx(WS_EX_CLIENTEDGE,
        "Button", "Connect", WS_CHILD | WS_VISIBLE,
        0, 0, 0, 0,
        hWnd, (HMENU)IDC_CONNECT,
        GetModuleHandle(NULL), NULL);
    clientWnds->hDisconnect = CreateWindowEx(WS_EX_CLIENTEDGE,
        "Button", "Disconnect", WS_CHILD | WS_VISIBLE,
        0, 0, 0, 0,
        hWnd, (HMENU)IDC_DISCONNECT,
        GetModuleHandle(NULL), NULL);
    clientWnds->hTest = CreateWindowEx(WS_EX_CLIENTEDGE,
        "Button", "Begin Test", WS_CHILD | WS_VISIBLE,
        0, 0, 0, 0,
        hWnd, (HMENU)IDC_TEST,
        GetModuleHandle(NULL), NULL);
    clientWnds->hOutput = CreateWindowEx(WS_EX_CLIENTEDGE,
        "Edit", "",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY,
        0, 0, 0, 0, hWnd, NULL,
        GetModuleHandle(NULL), NULL);
    clientWnds->hInput = CreateWindowEx(WS_EX_CLIENTEDGE,
        "Edit", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        0, 0, 0, 0, hWnd, NULL,
        GetModuleHandle(NULL), NULL);
    clientWnds->hSend = CreateWindowEx(WS_EX_CLIENTEDGE,
        "Button", "Send", WS_CHILD | WS_VISIBLE,
        0, 0, 0, 0,
        hWnd, (HMENU)IDC_SEND_MESSAGE,
        GetModuleHandle(NULL), NULL);

    // remote address controls
    clientWnds->hIPHostLabel = CreateWindowEx(NULL,
        "Static", "IP / Host Name:",
        WS_CHILD | WS_VISIBLE,
        RemoteAddrGroupX+PADDING, RemoteAddrGroupY+PADDING_TOP_GROUPBOX+PADDING,
        COLUMN_1_WIDTH, TEXT_HEIGHT,
        hWnd, NULL,
        GetModuleHandle(NULL), NULL);
    clientWnds->hTestPortLabel = CreateWindowEx(NULL,
        "Static", "Test Port:",
        WS_CHILD | WS_VISIBLE,
        RemoteAddrGroupX+PADDING, RemoteAddrGroupY+PADDING_TOP_GROUPBOX+PADDING*2+TEXT_HEIGHT,
        COLUMN_1_WIDTH, TEXT_HEIGHT,
        hWnd, NULL,
        GetModuleHandle(NULL), NULL);
    clientWnds->hControlPortLabel = CreateWindowEx(NULL,
        "Static", "Control Port:",
        WS_CHILD | WS_VISIBLE,
        RemoteAddrGroupX+PADDING, RemoteAddrGroupY+PADDING_TOP_GROUPBOX+PADDING*3+TEXT_HEIGHT*2,
        COLUMN_1_WIDTH, TEXT_HEIGHT,
        hWnd, NULL,
        GetModuleHandle(NULL), NULL);
    clientWnds->hIpHost = CreateWindowEx(WS_EX_CLIENTEDGE,
        "Edit", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        RemoteAddrGroupX+COLUMN_1_WIDTH+PADDING*2, RemoteAddrGroupY+PADDING_TOP_GROUPBOX+PADDING,
        COLUMN_2_WIDTH+COLUMN_3_WIDTH+PADDING, TEXT_HEIGHT,
        hWnd, NULL,
        GetModuleHandle(NULL), NULL);
    clientWnds->hTestPort = CreateWindowEx(WS_EX_CLIENTEDGE,
        "Edit", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        RemoteAddrGroupX+COLUMN_1_WIDTH+PADDING*2, RemoteAddrGroupY+PADDING_TOP_GROUPBOX+PADDING*2+TEXT_HEIGHT,
        COLUMN_2_WIDTH+COLUMN_3_WIDTH+PADDING, TEXT_HEIGHT,
        hWnd, NULL,
        GetModuleHandle(NULL), NULL);
    clientWnds->hCtrlPort = CreateWindowEx(WS_EX_CLIENTEDGE,
        "Edit", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        RemoteAddrGroupX+COLUMN_1_WIDTH+PADDING*2, RemoteAddrGroupY+PADDING_TOP_GROUPBOX+PADDING*3+TEXT_HEIGHT*2,
        COLUMN_2_WIDTH+COLUMN_3_WIDTH+PADDING, TEXT_HEIGHT,
        hWnd, NULL,
        GetModuleHandle(NULL), NULL);

    // protocol parameters controls
    clientWnds->hTcp = CreateWindowEx(NULL,
        "Button", "TCP",
        WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
        ProtocolParamsGroupX+PADDING, ProtocolParamsGroupY+PADDING_TOP_GROUPBOX+PADDING,
        COLUMN_1_WIDTH, TEXT_HEIGHT,
        hWnd, (HMENU)IDC_TCP,
        GetModuleHandle(NULL), NULL);
    clientWnds->hUdp = CreateWindowEx(NULL,
        "Button", "UDP",
        WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
        ProtocolParamsGroupX+PADDING*2+COLUMN_1_WIDTH, ProtocolParamsGroupY+PADDING_TOP_GROUPBOX+PADDING,
        COLUMN_2_WIDTH, TEXT_HEIGHT,
        hWnd, (HMENU)IDC_UDP,
        GetModuleHandle(NULL), NULL);
    clientWnds->hPacketSizeLabel = CreateWindowEx(NULL,
        "Static", "Packet Size:",
        WS_CHILD | WS_VISIBLE,
        ProtocolParamsGroupX+PADDING, ProtocolParamsGroupY+PADDING_TOP_GROUPBOX+PADDING*2+TEXT_HEIGHT,
        COLUMN_1_WIDTH, TEXT_HEIGHT,
        hWnd, NULL,
        GetModuleHandle(NULL), NULL);
    clientWnds->hPacketSize = CreateWindowEx(WS_EX_CLIENTEDGE,
        "Edit", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        ProtocolParamsGroupX+COLUMN_1_WIDTH+PADDING*2, ProtocolParamsGroupY+PADDING_TOP_GROUPBOX+PADDING*2+TEXT_HEIGHT,
        COLUMN_2_WIDTH+COLUMN_3_WIDTH+PADDING, TEXT_HEIGHT,
        hWnd, NULL,
        GetModuleHandle(NULL), NULL);

    // data parameters controls
    clientWnds->hSendFile = CreateWindowEx(NULL,
        "Button", "Send File",
        WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
        DataParamsGroupX+PADDING, DataParamsGroupY+PADDING_TOP_GROUPBOX+PADDING,
        COLUMN_1_WIDTH+COLUMN_2_WIDTH+COLUMN_3_WIDTH+PADDING*2, TEXT_HEIGHT,
        hWnd, (HMENU)IDC_SEND_FILE,
        GetModuleHandle(NULL), NULL);
    clientWnds->hSendGeneratedData = CreateWindowEx(NULL,
        "Button", "Send Generated Data",
        WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
        DataParamsGroupX+PADDING, DataParamsGroupY+PADDING_TOP_GROUPBOX+PADDING*2+TEXT_HEIGHT,
        COLUMN_1_WIDTH+COLUMN_2_WIDTH+COLUMN_3_WIDTH+PADDING*2, TEXT_HEIGHT,
        hWnd, (HMENU)IDC_SEND_GENERATED_DATA,
        GetModuleHandle(NULL), NULL);
    clientWnds->hChooseFile = CreateWindowEx(NULL,
        "Static", "Choose File:",
        WS_CHILD | WS_VISIBLE,
        DataParamsGroupX+PADDING, DataParamsGroupY+PADDING_TOP_GROUPBOX+PADDING*3+TEXT_HEIGHT*2,
        COLUMN_1_WIDTH, TEXT_HEIGHT,
        hWnd, NULL,
        GetModuleHandle(NULL), NULL);
    clientWnds->hBrowseFile = CreateWindowEx(NULL,
        "Button", "...",
        WS_CHILD | WS_VISIBLE,
        DataParamsGroupX+COLUMN_1_WIDTH+COLUMN_2_WIDTH+PADDING*3, DataParamsGroupY+PADDING_TOP_GROUPBOX+PADDING*3+TEXT_HEIGHT*2,
        COLUMN_3_WIDTH, TEXT_HEIGHT,
        hWnd, (HMENU)IDC_BROWSE_FILE,
        GetModuleHandle(NULL), NULL);
    clientWnds->hPacketsCountLabel = CreateWindowEx(NULL,
        "Static", "Packets to Send:",
        WS_CHILD | WS_VISIBLE,
        DataParamsGroupX+PADDING, DataParamsGroupY+PADDING_TOP_GROUPBOX+PADDING*4+TEXT_HEIGHT*3,
        COLUMN_1_WIDTH, TEXT_HEIGHT,
        hWnd, NULL,
        GetModuleHandle(NULL), NULL);
    clientWnds->hByteCountLabel = CreateWindowEx(NULL,
        "Static", "Bytes to Send:",
        WS_CHILD | WS_VISIBLE,
        DataParamsGroupX+PADDING, DataParamsGroupY+PADDING_TOP_GROUPBOX+PADDING*5+TEXT_HEIGHT*4,
        COLUMN_1_WIDTH, TEXT_HEIGHT,
        hWnd, NULL,
        GetModuleHandle(NULL), NULL);
    clientWnds->hFile = CreateWindowEx(WS_EX_CLIENTEDGE,
        "Edit", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        DataParamsGroupX+COLUMN_1_WIDTH+PADDING*2, DataParamsGroupY+PADDING_TOP_GROUPBOX+PADDING*3+TEXT_HEIGHT*2,
        COLUMN_2_WIDTH, TEXT_HEIGHT,
        hWnd, NULL,
        GetModuleHandle(NULL), NULL);
    clientWnds->hPacketCount = CreateWindowEx(WS_EX_CLIENTEDGE,
        "Edit", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        DataParamsGroupX+COLUMN_1_WIDTH+PADDING*2, DataParamsGroupY+PADDING_TOP_GROUPBOX+PADDING*4+TEXT_HEIGHT*3,
        COLUMN_2_WIDTH+COLUMN_3_WIDTH+PADDING, TEXT_HEIGHT,
        hWnd, NULL,
        GetModuleHandle(NULL), NULL);
    clientWnds->hByteCount = CreateWindowEx(WS_EX_CLIENTEDGE,
        "Edit", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        DataParamsGroupX+COLUMN_1_WIDTH+PADDING*2, DataParamsGroupY+PADDING_TOP_GROUPBOX+PADDING*5+TEXT_HEIGHT*4,
        COLUMN_2_WIDTH+COLUMN_3_WIDTH+PADDING, TEXT_HEIGHT,
        hWnd, NULL,
        GetModuleHandle(NULL), NULL);
}

static void hideClientWindows(ClientWnds* clientWnds)
{
    ShowWindow(clientWnds->hRemoteAddress,      SW_HIDE);
    ShowWindow(clientWnds->hProtocolParameters, SW_HIDE);
    ShowWindow(clientWnds->hDataParameters,     SW_HIDE);
    ShowWindow(clientWnds->hConnect,            SW_HIDE);
    ShowWindow(clientWnds->hDisconnect,         SW_HIDE);
    ShowWindow(clientWnds->hTest,               SW_HIDE);
    ShowWindow(clientWnds->hOutput,             SW_HIDE);
    ShowWindow(clientWnds->hInput,              SW_HIDE);
    ShowWindow(clientWnds->hSend,               SW_HIDE);
    ShowWindow(clientWnds->hIPHostLabel,        SW_HIDE);
    ShowWindow(clientWnds->hTestPortLabel,      SW_HIDE);
    ShowWindow(clientWnds->hControlPortLabel,   SW_HIDE);
    ShowWindow(clientWnds->hIpHost,             SW_HIDE);
    ShowWindow(clientWnds->hTestPort,           SW_HIDE);
    ShowWindow(clientWnds->hCtrlPort,           SW_HIDE);
    ShowWindow(clientWnds->hTcp,                SW_HIDE);
    ShowWindow(clientWnds->hUdp,                SW_HIDE);
    ShowWindow(clientWnds->hPacketSizeLabel,    SW_HIDE);
    ShowWindow(clientWnds->hPacketSize,         SW_HIDE);
    ShowWindow(clientWnds->hSendFile,           SW_HIDE);
    ShowWindow(clientWnds->hSendGeneratedData,  SW_HIDE);
    ShowWindow(clientWnds->hChooseFile,         SW_HIDE);
    ShowWindow(clientWnds->hBrowseFile,         SW_HIDE);
    ShowWindow(clientWnds->hPacketsCountLabel,  SW_HIDE);
    ShowWindow(clientWnds->hByteCountLabel,     SW_HIDE);
    ShowWindow(clientWnds->hFile,               SW_HIDE);
    ShowWindow(clientWnds->hPacketCount,        SW_HIDE);
    ShowWindow(clientWnds->hByteCount,          SW_HIDE);
}

static void showClientWindows(ClientWnds* clientWnds)
{
    ShowWindow(clientWnds->hRemoteAddress,      SW_SHOW);
    ShowWindow(clientWnds->hProtocolParameters, SW_SHOW);
    ShowWindow(clientWnds->hDataParameters,     SW_SHOW);
    ShowWindow(clientWnds->hConnect,            SW_SHOW);
    ShowWindow(clientWnds->hDisconnect,         SW_SHOW);
    ShowWindow(clientWnds->hTest,               SW_SHOW);
    ShowWindow(clientWnds->hOutput,             SW_SHOW);
    ShowWindow(clientWnds->hInput,              SW_SHOW);
    ShowWindow(clientWnds->hSend,               SW_SHOW);
    ShowWindow(clientWnds->hIPHostLabel,        SW_SHOW);
    ShowWindow(clientWnds->hTestPortLabel,      SW_SHOW);
    ShowWindow(clientWnds->hControlPortLabel,   SW_SHOW);
    ShowWindow(clientWnds->hIpHost,             SW_SHOW);
    ShowWindow(clientWnds->hTestPort,           SW_SHOW);
    ShowWindow(clientWnds->hCtrlPort,           SW_SHOW);
    ShowWindow(clientWnds->hTcp,                SW_SHOW);
    ShowWindow(clientWnds->hUdp,                SW_SHOW);
    ShowWindow(clientWnds->hPacketSizeLabel,    SW_SHOW);
    ShowWindow(clientWnds->hPacketSize,         SW_SHOW);
    ShowWindow(clientWnds->hSendFile,           SW_SHOW);
    ShowWindow(clientWnds->hSendGeneratedData,  SW_SHOW);
    ShowWindow(clientWnds->hChooseFile,         SW_SHOW);
    ShowWindow(clientWnds->hBrowseFile,         SW_SHOW);
    ShowWindow(clientWnds->hPacketsCountLabel,  SW_SHOW);
    ShowWindow(clientWnds->hByteCountLabel,     SW_SHOW);
    ShowWindow(clientWnds->hFile,               SW_SHOW);
    ShowWindow(clientWnds->hPacketCount,        SW_SHOW);
    ShowWindow(clientWnds->hByteCount,          SW_SHOW);
}

static void hideServerWindows(ServerWnds* serverWnds)
{
    ShowWindow(serverWnds->hPort,               SW_HIDE);
    ShowWindow(serverWnds->hFile,               SW_HIDE);
    ShowWindow(serverWnds->hBrowseFile,         SW_HIDE);
    ShowWindow(serverWnds->hStart,              SW_HIDE);
    ShowWindow(serverWnds->hStop,               SW_HIDE);
    ShowWindow(serverWnds->hSend,               SW_HIDE);
    ShowWindow(serverWnds->hOutput,             SW_HIDE);
    ShowWindow(serverWnds->hInput,              SW_HIDE);
    ShowWindow(serverWnds->hSvrOptionsBroupBox, SW_HIDE);
    ShowWindow(serverWnds->hCtrlPortLabel,      SW_HIDE);
    ShowWindow(serverWnds->hFileLabel,          SW_HIDE);
}

static void showServerWindows(ServerWnds* serverWnds)
{
    ShowWindow(serverWnds->hPort,               SW_SHOW);
    ShowWindow(serverWnds->hFile,               SW_SHOW);
    ShowWindow(serverWnds->hBrowseFile,         SW_SHOW);
    ShowWindow(serverWnds->hStart,              SW_SHOW);
    ShowWindow(serverWnds->hStop,               SW_SHOW);
    ShowWindow(serverWnds->hSend,               SW_SHOW);
    ShowWindow(serverWnds->hOutput,             SW_SHOW);
    ShowWindow(serverWnds->hInput,              SW_SHOW);
    ShowWindow(serverWnds->hSvrOptionsBroupBox, SW_SHOW);
    ShowWindow(serverWnds->hCtrlPortLabel,      SW_SHOW);
    ShowWindow(serverWnds->hFileLabel,          SW_SHOW);
}
