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

static char debugString[1000];

struct ClientWnds
{
    HWND hRemoteAddress;
    HWND hProtocolParameters;
    HWND hDataParameters;
    HWND hConnect;
    HWND hDisconnect;
    HWND hTest;
    HWND hOutput;
    HWND hInput;
    HWND hSend;
    HWND hIPHostLabel;
    HWND hTestPortLabel;
    HWND hControlPortLabel;
    HWND hIpHost;
    HWND hTestPort;
    HWND hCtrlPort;
    HWND hTcp;
    HWND hUdp;
    HWND hPacketSizeLabel;
    HWND hPacketSize;
    HWND hSendFile;
    HWND hSendGeneratedData;
    HWND hChooseFile;
    HWND hBrowseFile;
    HWND hPacketsCountLabel;
    HWND hByteCountLabel;
    HWND hFile;
    HWND hPacketCount;
    HWND hByteCount;
};

struct ServerWnds
{
    HWND hPort;
    HWND hFile;
    HWND hBrowseFile;
    HWND hStart;
    HWND hStop;
    HWND hSend;
    HWND hOutput;
    HWND hInput;
    HWND hSvrOptionsBroupBox;
    HWND hCtrlPortLabel;
    HWND hFileLabel;
};

struct CommonWnds
{
    HWND hBackground;
};

struct ClientTestObjects
{
    ClientWnds* clientWnds;
    Session* ctrlSession;
    Session* testSession;
};

typedef struct ClientWnds ClientWnds;
typedef struct ServerWnds ServerWnds;

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

static void serverOnConnect(Server*, SOCKET, sockaddr_in);
static void serverOnError(Server*, int, int);
static void serverOnClose(Server*, int);

static void svrSessionOnMessage(Session*, char*, int);
static void svrSessionOnError(Session*, int, int);
static void svrSessionOnClose(Session*, int);

static void clientOnConnect(Client*, SOCKET, sockaddr_in);
static void clientOnError(Client*, int, int);

static void clntSessionOnMessage(Session*, char*, int);
static void clntSessionOnError(Session*, int, int);
static void clntSessionOnClose(Session*, int);

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

    hWnd = CreateWindow((LPCSTR)Title, (LPCSTR)Title, WS_OVERLAPPEDWINDOW, 0, 0, 760, 480, NULL, NULL, hInst, NULL);

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

    static ClientTestObjects clntTestObjects;

    switch (Message)
    {
        case WM_CREATE:
        {
            makeCommonWindows(hWnd, &commonWnds);
            makeClientWindows(hWnd, &clientWnds);
            makeServerWindows(hWnd, &serverWnds);

            hideServerWindows(&serverWnds);

            serverInit(&server);
            server.usrPtr    = &serverWnds;
            server.onClose   = serverOnClose;
            server.onConnect = serverOnConnect;
            server.onError   = serverOnError;

            clientInit(&client);
            client.usrPtr    = &clntTestObjects;
            client.onConnect = clientOnConnect;
            client.onError   = clientOnError;

            memset(&ctrlSession, 0, sizeof(Session));
            ctrlSession._sessionThread = INVALID_HANDLE_VALUE;

            memset(&testSession, 0, sizeof(Session));
            testSession._sessionThread = INVALID_HANDLE_VALUE;

            clntTestObjects.clientWnds  = &clientWnds;
            clntTestObjects.ctrlSession = &ctrlSession;
            clntTestObjects.testSession = &testSession;
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
                    OutputDebugString("IDC_TCP\r\n");
                    serverOpenUDPPort(&server, 8000);
                    break;
                }
                case IDC_UDP:
                {
                    OutputDebugString("IDC_UDP\r\n");

                    char output[MAX_STRING_LEN];
                    char hostIp[MAX_STRING_LEN];
                    char hostPort[MAX_STRING_LEN];
                    int port;

                    GetWindowText(clientWnds.hIpHost, hostIp, MAX_STRING_LEN);
                    GetWindowText(clientWnds.hCtrlPort, hostPort, MAX_STRING_LEN);

                    sprintf_s(output, "Client Connecting: Connecting to %s:%d...\r\n", hostIp, hostPort);
                    appendWindowText(serverWnds.hOutput, output);

                    port = atoi(hostPort);

                    switch(clientConnectUDP(&client, hostIp, port))
                    {
                        case ALREADY_RUNNING_FAIL:
                            appendWindowText(clientWnds.hOutput, "Client Connecting: ALREADY_RUNNING_FAIL\r\n");
                            break;
                        case THREAD_FAIL:
                            appendWindowText(clientWnds.hOutput, "Client Connecting: THREAD_FAIL\r\n");
                            break;
                        case NORMAL_SUCCESS:
                            appendWindowText(clientWnds.hOutput, "Client Connecting: NORMAL_SUCCESS\r\n");
                            break;
                    }
                    break;
                }
                case IDC_SEND_FILE:
                {
                    OutputDebugString("IDC_SEND_FILE\r\n");
                    break;
                }
                case IDC_SEND_GENERATED_DATA:
                {
                    OutputDebugString("IDC_SEND_GENERATED_DATA\r\n");
                    break;
                }
                case IDC_BROWSE_FILE:
                {
                    OutputDebugString("IDC_BROWSE_FILE\r\n");
                    break;
                }
                case IDC_CONNECT:
                {
                    char output[MAX_STRING_LEN];
                    char hostIp[MAX_STRING_LEN];
                    char hostPort[MAX_STRING_LEN];
                    int port;

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
                    OutputDebugString("IDC_TEST\r\n");
                    break;
                }
                case IDC_SEND_MESSAGE:
                {
                    OutputDebugString("IDC_SEND_MESSAGE\r\n");
                    break;
                }
                case IDC_MODE_SERVER:
                {
                    OutputDebugString("IDC_MODE_SERVER\r\n");
                    hideClientWindows(&clientWnds);
                    showServerWindows(&serverWnds);
                    break;
                }
                case IDC_MODE_CLIENT:
                {
                    OutputDebugString("IDC_MODE_CLIENT\r\n");
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
                    char string[MAX_STRING_LEN];
                    char portString[MAX_STRING_LEN];

                    GetWindowText(serverWnds.hPort, portString, MAX_STRING_LEN);

                    unsigned short port = atoi(portString);

                    sprintf_s(string, "Server Start: Starting on port %d\r\n", port);
                    appendWindowText(serverWnds.hOutput, string);

                    serverSetPort(&server, port);
                    switch(serverStart(&server))
                    {
                        case ALREADY_RUNNING_FAIL:
                        {
                            appendWindowText(serverWnds.hOutput, "Server Start: SERVER_ALREADY_RUNNING_FAIL\r\n");
                            break;
                        }
                        case THREAD_FAIL:
                        {
                            appendWindowText(serverWnds.hOutput, "Server Start: THREAD_FAIL\r\n");
                            break;
                        }
                        case NORMAL_SUCCESS:
                        {
                            appendWindowText(serverWnds.hOutput, "Server Start: NORMAL_SUCCESS\r\n");
                            break;
                        }
                    }
                    break;
                }
                case IDC_STOP_SERVER:
                {
                    OutputDebugString("IDC_STOP_SERVER\r\n");
                    serverStop(&server);
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

static void serverOnConnect(Server* server, SOCKET clientSock, sockaddr_in clientAddr)
{
    ServerWnds* serverWnds = (ServerWnds*) server->usrPtr;

    sprintf_s(debugString, "%s: Connected\n", inet_ntoa(clientAddr.sin_addr));
    appendWindowText(serverWnds->hOutput, debugString);

    Session* session = (Session*) malloc(sizeof(Session));
    sessionInit(session, &clientSock, &clientAddr);
    session->usrPtr     = serverWnds;
    session->onMessage  = svrSessionOnMessage;
    session->onError    = svrSessionOnError;
    session->onClose    = svrSessionOnClose;
    sessionStart(session);
}

static void serverOnError(Server* server, int errCode, int winErrCode)
{
    char output[MAX_STRING_LEN];

    ServerWnds* serverWnds = (ServerWnds*) server->usrPtr;

    switch(errCode)
    {
        case UNKNOWN_FAIL:
            sprintf_s(output, "Server Error: UNKNOWN_FAIL\r\n");
            break;
        case THREAD_FAIL:
            sprintf_s(output, "Server Error: THREAD_FAIL\r\n");
            break;
        case SOCKET_FAIL:
            sprintf_s(output, "Server Error: SOCKET_FAIL\r\n");
            break;
        case BIND_FAIL:
            sprintf_s(output, "Server Error: BIND_FAIL\r\n");
            break;
        case ACCEPT_FAIL:
            sprintf_s(output, "Server Error: ACCEPT_FAIL\r\n");
            break;
        case ALREADY_RUNNING_FAIL:
            sprintf_s(output, "Server Error: ALREADY_RUNNING_FAIL\r\n");
            break;
        case ALREADY_STOPPED_FAIL:
            sprintf_s(output, "Server Error: ALREADY_STOPPED_FAIL\r\n");
            break;
    }

    appendWindowText(serverWnds->hOutput, output);
}

static void serverOnClose(Server* server, int code)
{
    char output[MAX_STRING_LEN];

    ServerWnds* serverWnds = (ServerWnds*) server->usrPtr;

    switch(code)
    {
        case NORMAL_SUCCESS:
            sprintf_s(output, "Server Stopped: NORMAL_SUCCESS\r\n");
            break;
        case UNKNOWN_FAIL:
            sprintf_s(output, "Server Stopped: UNKNOWN_FAIL\r\n");
            break;
        case THREAD_FAIL:
            sprintf_s(output, "Server Stopped: THREAD_FAIL\r\n");
            break;
        case SOCKET_FAIL:
            sprintf_s(output, "Server Stopped: SOCKET_FAIL\r\n");
            break;
        case BIND_FAIL:
            sprintf_s(output, "Server Stopped: BIND_FAIL\r\n");
            break;
        case ACCEPT_FAIL:
            sprintf_s(output, "Server Stopped: ACCEPT_FAIL\r\n");
            break;
        case ALREADY_RUNNING_FAIL:
            sprintf_s(output, "Server Stopped: ALREADY_RUNNING_FAIL\r\n");
            break;
        case ALREADY_STOPPED_FAIL:
            sprintf_s(output, "Server Stopped: ALREADY_STOPPED_FAIL\r\n");
            break;
    }

    appendWindowText(serverWnds->hOutput, output);
}

static void svrSessionOnMessage(Session* session, char* str, int len)
{
    char output[MAX_STRING_LEN];

    HWND* hOutput = (HWND*) session->usrPtr;

    sprintf_s(output, "%s: %.*s\r\n",
        inet_ntoa(sessionGetIP(session)), len, str);
    appendWindowText(*hOutput, output);

    sessionSend(session, str, len);
}

static void svrSessionOnError(Session* session, int errCode, int winErrCode)
{
    char output[MAX_STRING_LEN];

    HWND* hOutput = (HWND*) session->usrPtr;

    sprintf_s(output, "%s: Error %d\r\n",
        inet_ntoa(sessionGetIP(session)), errCode);
    appendWindowText(*hOutput, output);
}

static void svrSessionOnClose(Session* session, int code)
{
    char output[MAX_STRING_LEN];

    HWND* hOutput = (HWND*) session->usrPtr;

    sprintf_s(output, "%s: Disconnected %d\r\n",
        inet_ntoa(sessionGetIP(session)), code);
    appendWindowText(*hOutput, output);
}

static void clientOnConnect(Client* client, SOCKET clientSock, sockaddr_in clientAddr)
{
    ClientTestObjects* testObjs = (ClientTestObjects*) client->usrPtr;

    sprintf_s(debugString, "Connected to \"%s:%d\"\r\n",
        inet_ntoa(clientAddr.sin_addr),
        htons(clientAddr.sin_port));
    appendWindowText(testObjs->clientWnds->hOutput, debugString);

    Session* session = testObjs->ctrlSession;
    sessionInit(session, &clientSock, &clientAddr);
    session->usrPtr     = testObjs;
    session->onMessage  = clntSessionOnMessage;
    session->onError    = clntSessionOnError;
    session->onClose    = clntSessionOnClose;
    sessionStart(session);

    testObjs->ctrlSession = session;
}

static void clientOnError(Client* client, int errCode, int winErrCode)
{
    char output[MAX_STRING_LEN];

    ClientTestObjects* testObjs = (ClientTestObjects*) client->usrPtr;

    switch(errCode)
    {
        case UNKNOWN_FAIL:
            sprintf_s(output, "Failed to connect: UNKNOWN_FAIL\r\n");
            break;
        case THREAD_FAIL:
            sprintf_s(output, "Failed to connect: THREAD_FAIL\r\n");
            break;
        case SOCKET_FAIL:
            sprintf_s(output, "Failed to connect: SOCKET_FAIL\r\n");
            break;
        case BIND_FAIL:
            sprintf_s(output, "Failed to connect: BIND_FAIL\r\n");
            break;
        case ALREADY_RUNNING_FAIL:
            sprintf_s(output, "Failed to connect: ALREADY_RUNNING_FAIL\r\n");
            break;
        case ALREADY_STOPPED_FAIL:
            sprintf_s(output, "Failed to connect: ALREADY_STOPPED_FAIL\r\n");
            break;
        case UNKNOWN_IP_FAIL:
            sprintf_s(output, "Failed to connect: UNKNOWN_IP_FAIL\r\n");
            break;
        case CONNECT_FAIL:
            sprintf_s(output, "Failed to connect: CONNECT_FAIL\r\n");
            break;
    }

    appendWindowText(testObjs->clientWnds->hOutput, output);
}

static void clntSessionOnMessage(Session* session, char* str, int len)
{
    char output[MAX_STRING_LEN];

    ClientTestObjects* testObjs = (ClientTestObjects*) session->usrPtr;

    sprintf_s(output, "Control: %.*s\r\n", len, str);
    appendWindowText(testObjs->clientWnds->hOutput, output);

    sessionSend(session, str, len);
}

static void clntSessionOnError(Session* session, int errCode, int winErrCode)
{
    char output[MAX_STRING_LEN];

    ClientTestObjects* testObjs = (ClientTestObjects*) session->usrPtr;

    sprintf_s(output, "Control: Error %d\r\n", errCode);
    appendWindowText(testObjs->clientWnds->hOutput, output);
}

static void clntSessionOnClose(Session* session, int code)
{
    char output[MAX_STRING_LEN];

    ClientTestObjects* testObjs = (ClientTestObjects*) session->usrPtr;

    sprintf_s(output, "Control: Disconnect %d\r\n", code);
    appendWindowText(testObjs->clientWnds->hOutput, output);
}
