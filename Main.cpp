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

struct ClientWnds
{
    HWND hIpHost;
    HWND hTestPort;
    HWND hCtrlPort;
    HWND hTcp;
    HWND hUdp;
    HWND hPacketSize;
    HWND hPacketCount;
    HWND hByteCount;
    HWND hSendFile;
    HWND hSendGeneratedData;
    HWND hFile;                 // file input
    HWND hBrowseFile;           // button to click to browse for the file
    HWND hConnect;
    HWND hTest;
    HWND hOutput;
    HWND hInput;
    HWND hSend;
};

struct ServerWnds
{
    HWND hPort;
    HWND hFile;                 // file input
    HWND hBrowseFile;           // button to click to browse for the file
    HWND hStart;
    HWND hStop;
    HWND hSend;
    HWND hOutput;
    HWND hInput;
};

typedef struct ClientWnds ClientWnds;
typedef struct ServerWnds ServerWnds;

static void updateClientDisplays(HWND, ClientWnds*);
static void updateServerDisplays(HWND, ServerWnds*);
static void makeClientWindows(HWND, ClientWnds*);
static void makeServerWindows(HWND, ServerWnds*);

void serverOnConnect(Server*, SOCKET);
void serverOnError(Server*, int, void*, int);
void serverOnClose(Server*, int, void*, int);

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

    hWnd = CreateWindow((LPCSTR)Title, (LPCSTR)Title, WS_OVERLAPPEDWINDOW, 300, 300, 740, 480, NULL, NULL, hInst, NULL);

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
    static Server server;

    switch (Message)
    {
    case WM_CREATE:
        {
            //makeClientWindows(hWnd, &clientWnds);
            makeServerWindows(hWnd, &serverWnds);

            serverInit(&server, AF_INET, 7001, INADDR_ANY);
            serverSetOnClose(&server, serverOnClose);
            serverSetOnConnect(&server, serverOnConnect);
            serverSetOnError(&server, serverOnError);
        }
        break;
    case WM_DESTROY:
        WSACleanup();
        PostQuitMessage(0);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
            case IDC_TCP:
                OutputDebugString("IDC_TCP\n");
                break;
            case IDC_UDP:
                OutputDebugString("IDC_UDP\n");
                break;
            case IDC_SEND_FILE:
                OutputDebugString("IDC_SEND_FILE\n");
                break;
            case IDC_SEND_GENERATED_DATA:
                OutputDebugString("IDC_SEND_GENERATED_DATA\n");
                break;
            case IDC_BROWSE_FILE:
                OutputDebugString("IDC_BROWSE_FILE\n");
                break;
            case IDC_CONNECT:
                OutputDebugString("IDC_CONNECT\n");
                break;
            case IDC_TEST:
                OutputDebugString("IDC_TEST\n");
                break;
            case IDC_SEND_MESSAGE:
                OutputDebugString("IDC_SEND_MESSAGE\n");
                break;
            case IDC_MODE_SERVER:
                OutputDebugString("IDC_MODE_SERVER\n");
                break;
            case IDC_MODE_CLIENT:
                OutputDebugString("IDC_MODE_CLIENT\n");
                break;
            case IDC_HELP:
                OutputDebugString("IDC_HELP\n");
                break;
            case IDC_START_SERVER:
                OutputDebugString("IDC_START_SERVER\n");
                serverStart(&server);
                break;
            case IDC_STOP_SERVER:
                OutputDebugString("IDC_STOP_SERVER\n");
                serverStop(&server);
                break;
        }
        break;
    case WM_SIZE:
        updateClientDisplays(hWnd, &clientWnds);
        updateServerDisplays(hWnd, &serverWnds);
        break;
    default:
        return DefWindowProc(hWnd, Message, wParam, lParam);
    }
    return 0;
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
static void updateServerDisplays(HWND hwnd, ServerWnds* serverWnds)
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
static void updateClientDisplays(HWND hwnd, ClientWnds* clientWnds)
{
    RECT windowRect;
    GetClientRect(hwnd, &windowRect);

    MoveWindow(clientWnds->hConnect,
        PADDING, windowRect.bottom-PADDING-BUTTON_HEIGHT,
        BUTTON_WIDTH, BUTTON_HEIGHT, TRUE);
    MoveWindow(clientWnds->hTest,
        BUTTON_WIDTH+PADDING*2, windowRect.bottom-PADDING-BUTTON_HEIGHT,
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
    CreateWindowEx(NULL,
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
    CreateWindowEx(NULL,
        "Static", "Control Port:", WS_CHILD | WS_VISIBLE,
        ServerOptionsGroupX+PADDING, ServerOptionsGroupY+PADDING_TOP_GROUPBOX+PADDING,
        COLUMN_1_WIDTH, TEXT_HEIGHT,
        hWnd, NULL,
        GetModuleHandle(NULL), NULL);
    CreateWindowEx(NULL,
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
    CreateWindowEx(NULL,
        "Button", "Remote Address",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        RemoteAddrGroupX, RemoteAddrGroupY,
        RemoteAddrGroupW, RemoteAddrGroupH,
        hWnd, NULL,
        GetModuleHandle(NULL), NULL);
    CreateWindowEx(NULL,
        "Button", "Protocol Parameters",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        ProtocolParamsGroupX, ProtocolParamsGroupY,
        ProtocolParamsGroupW, ProtocolParamsGroupH,
        hWnd, NULL,
        GetModuleHandle(NULL), NULL);
    CreateWindowEx(NULL,
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
    CreateWindowEx(NULL,
        "Static", "IP / Host Name:",
        WS_CHILD | WS_VISIBLE,
        RemoteAddrGroupX+PADDING, RemoteAddrGroupY+PADDING_TOP_GROUPBOX+PADDING,
        COLUMN_1_WIDTH, TEXT_HEIGHT,
        hWnd, NULL,
        GetModuleHandle(NULL), NULL);
    CreateWindowEx(NULL,
        "Static", "Test Port:",
        WS_CHILD | WS_VISIBLE,
        RemoteAddrGroupX+PADDING, RemoteAddrGroupY+PADDING_TOP_GROUPBOX+PADDING*2+TEXT_HEIGHT,
        COLUMN_1_WIDTH, TEXT_HEIGHT,
        hWnd, NULL,
        GetModuleHandle(NULL), NULL);
    CreateWindowEx(NULL,
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
    CreateWindowEx(NULL,
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
    CreateWindowEx(NULL,
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
    CreateWindowEx(NULL,
        "Static", "Packets to Send:",
        WS_CHILD | WS_VISIBLE,
        DataParamsGroupX+PADDING, DataParamsGroupY+PADDING_TOP_GROUPBOX+PADDING*4+TEXT_HEIGHT*3,
        COLUMN_1_WIDTH, TEXT_HEIGHT,
        hWnd, NULL,
        GetModuleHandle(NULL), NULL);
    CreateWindowEx(NULL,
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

void serverOnConnect(Server*, SOCKET)
{
}

void serverOnError(Server*, int, void*, int)
{
}

void serverOnClose(Server*, int, void*, int)
{
}
