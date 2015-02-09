/**
 * functions here manage the GUI, initialize controllers, and forward events and
 *   messages to controllers which do all the work.
 *
 * @sourceFile Main.cpp
 *
 * @program    ProtocolTester.exe
 *
 * @function   int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
 * @function   LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM)
 * @function   char* rctoa(int)
 * @function   static char* getFilePath(HWND)
 * @function   static void updateCommonWindows(HWND hwnd, CommonWnds*)
 * @function   static void updateServerWindows(HWND hwnd, ServerWnds*)
 * @function   static void updateClientWindows(HWND hwnd, ClientWnds*)
 * @function   static void makeCommonWindows(HWND hWnd, CommonWnds*)
 * @function   static void makeServerWindows(HWND hWnd, ServerWnds*)
 * @function   static void makeClientWindows(HWND hWnd, ClientWnds*)
 * @function   static void hideClientWindows(ClientWnds*)
 * @function   static void showClientWindows(ClientWnds*)
 * @function   static void hideServerWindows(ServerWnds*)
 * @function   static void showServerWindows(ServerWnds*)
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

static char* getFilePath(HWND);

/**
 * entry point to the windows application.
 *
 * @function   WinMain
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
 * @signature  int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lspszCmdParam, int nCmdShow)
 *
 * @param      hInst handle to the application instance
 * @param      hPrevInst handle to a previous application's instance
 * @param      lspszCmdParam the command line for the application
 * @param      nCmdShow controls how the window is to be shown
 *
 * @return     returns WM_QUIT on normal exit, or 0 if WndProc was never run.
 */
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lspszCmdParam,
    int nCmdShow)
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

    while (GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    return Msg.wParam;
}

/**
 * message handler for this windows application.
 *
 * @function   WndProc
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
 * @signature  LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam,
 *   LPARAM lParam)
 *
 * @param      hWnd window handle that this message handler is handling messages
 *   for
 * @param      Message message type
 * @param      wParam a parameter for the message
 * @param      lParam another parameter for the message
 *
 * @return     i don't know
 */
LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    static ClientWnds clientWnds;
    static ServerWnds serverWnds;
    static CommonWnds commonWnds;

    static Server server;
    static Client client;

    static int currMode = MODE_UNDEFINED;

    static char buffer[MAX_STRING_LEN];

    switch (Message)
    {
    case WM_CREATE:
        // make all the gui windows
        makeCommonWindows(hWnd, &commonWnds);
        makeClientWindows(hWnd, &clientWnds);
        makeServerWindows(hWnd, &serverWnds);

        // set initial mode to client mode
        PostMessage(hWnd, WM_COMMAND, LOWORD(IDC_MODE_CLIENT), 0);

        // set up control server & client
        ctrlSvrInit(&server, &serverWnds);
        ctrlClntInit(&client, &clientWnds);
        break;
    case WM_DESTROY:
        WSACleanup();
        PostQuitMessage(0);
        break;
    case WM_SIZE:
        updateCommonWindows(hWnd, &commonWnds);
        updateClientWindows(hWnd, &clientWnds);
        updateServerWindows(hWnd, &serverWnds);
        break;
    case WM_GETMINMAXINFO:
    {
        MINMAXINFO* mmi = (MINMAXINFO*)lParam;
        mmi->ptMinTrackSize.x = 850;
        mmi->ptMinTrackSize.y = 500;
        break;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_TCP:
            ctrlClntSetTestProtocol(&client, MODE_TCP);
            break;
        case IDC_UDP:
            ctrlClntSetTestProtocol(&client, MODE_UDP);
            break;
        case IDC_SEND_FILE:
            ctrlClntSetDataSource(&client, MODE_FROM_FILE);
            break;
        case IDC_SEND_GENERATED_DATA:
            ctrlClntSetDataSource(&client, MODE_FROM_GENERATOR);
            break;
        case IDC_BROWSE_FILE:
            switch(currMode)
            {
            case MODE_CLIENT:
                SetWindowText(clientWnds.hFile, getFilePath(hWnd));
                break;
            case MODE_SERVER:
                SetWindowText(serverWnds.hFile, getFilePath(hWnd));
                break;
            }
            break;
        case IDC_CONNECT:
            ctrlClntConnectCtrl(&client);
            break;
        case IDC_DISCONNECT:
            ctrlClntDisonnect(&client);
            break;
        case IDC_TEST:
            ctrlClntStartTest(&client);
            break;
        case IDC_SEND_MESSAGE:
            switch(currMode)
            {
            case MODE_CLIENT:
                ctrlClientSendChat(&client);
                break;
            case MODE_SERVER:
                ctrlSvrSendChat(&server);
                break;
            }
            break;
        case IDC_MODE_SERVER:
            currMode = MODE_SERVER;
            hideClientWindows(&clientWnds);
            showServerWindows(&serverWnds);
            break;
        case IDC_MODE_CLIENT:
            currMode = MODE_CLIENT;
            hideServerWindows(&serverWnds);
            showClientWindows(&clientWnds);
            break;
        case IDC_HELP:
            OutputDebugString("IDC_HELP\r\n");
            break;
        case IDC_START_SERVER:
            ctrlSvrStart(&server);
            break;
        case IDC_STOP_SERVER:
            ctrlSvrStop(&server);
            break;
        break;
        }
    default:
        return DefWindowProc(hWnd, Message, wParam, lParam);
    }
    return 0;
}

/**
 * takes a return code defined in ReturnCodes.h, and returns the name of the
 *   error as a pointer to a c style, null terminated string.
 *
 * @function   rctoa
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
 * @signature  char* rctoa(int returnCode)
 *
 * @param      returnCode return code number to get the name of
 *
 * @return     pointer to a null terminated c style string that is the name of
 *   the return code.
 */
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

/**
 * opens the file chooser dialog, and returns a pointer to a string that is the
 *   path to the chosen file.
 *
 * @function   getFilePath
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
 * @signature  static char* getFilePath(HWND parentWindowHandle)
 *
 * @param      parentWindowHandle window handle to the parent window
 *
 * @return     pointer to numm terminated c style string that contains the file
 *   path of the chosen file.
 */
static char* getFilePath(HWND parentWindowHandle)
{
    OPENFILENAME ofn;       // common dialog box structure
    static char szFile[260];       // buffer for file name

    // initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = parentWindowHandle;
    ofn.lpstrFile = szFile;
    // Set lpstrFile[0] to '\0' so that GetOpenFileName does not
    // use the contents of szFile to initialize itself.
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // display the Open dialog box.
    GetOpenFileName(&ofn);

    // return the result...
    return szFile;
}

/**
 * resizes all the common windows appropriately to fit the passed parent window
 *   (hWnd)
 *
 * @function   updateCommonWindows
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
 * @signature  static void updateCommonWindows(HWND hwnd, CommonWnds*
 *   commonWnds)
 *
 * @param      hwnd window handle to parent window
 * @param      commonWnds pointer to common window structure
 */
static void updateCommonWindows(HWND hwnd, CommonWnds* commonWnds)
{
    RECT windowRect;
    GetClientRect(hwnd, &windowRect);

    MoveWindow(commonWnds->hBackground, 0, 0, windowRect.right,
        windowRect.bottom, TRUE);
}
/**
 * resizes all the server windows as needed to make it fit the passed parent
 *   window appropriately.
 *
 * @function   updateServerWindows
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
 * @signature  static void updateServerWindows(HWND hwnd, ServerWnds*
 *   serverWnds)
 *
 * @param      hwnd handle to parent window.
 * @param      clientWnds handle to structure containing all the window handles
 *   to server related windows
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
 * resizes all the client windows as needed to make it fit the passed parent
 *   window appropriately.
 *
 * @function   updateClientWindows
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
 * @signature  static void updateClientWindows(HWND hwnd, ClientWnds* clientWnds)
 *
 * @param      hwnd handle to parent window of all the client windows.
 * @param      clientWnds pointer to structure of client window handles.
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

/**
 * creates the "common windows" for the program. common windows are the windows
 *   that are shown in both client, and server mode.
 *
 * @function   makeCommonWindows
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
 * @signature  static void makeCommonWindows(HWND hWnd, CommonWnds* commonWnds)
 *
 * @param      hWnd handle to the window that will be the parent of the "common
 *   windows"
 * @param      commonWnds pointer to the structure that contains all the window
 *   handles to the common windows.
 */
static void makeCommonWindows(HWND hWnd, CommonWnds* commonWnds)
{
    commonWnds->hBackground = CreateWindowEx(WS_EX_CLIENTEDGE,
        "Static", "", WS_CHILD | WS_VISIBLE | WS_DISABLED,
        0, 0, 0, 0,
        hWnd, NULL,
        GetModuleHandle(NULL), NULL);
}

/**
 * creates all the windows related to the server.
 *
 * @function   makeServerWindows
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
 * @signature  static void makeServerWindows(HWND hWnd, ServerWnds* serverWnds)
 *
 * @param      hWnd handle to the window that will be the parent of all server
 *   windows.
 * @param      serverWnds pointer to the ServerWnds structure that contains all
 *   window handles to the server windows.
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
 * creates all the windows related to the client
 *
 * @function   makeClientWindows
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
 * @signature  static void makeClientWindows(HWND hWnd, ClientWnds* clientWnds)
 *
 * @param      hWnd handle to the window that will be the parent of all client
 *   windows.
 * @param      clientWnds pointer to the ClientWnds structure that contains
 *   window handles to all the client windows.
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
    int DataParamsGroupH = PADDING_TOP_GROUPBOX+PADDING*5+TEXT_HEIGHT*4;

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
}

/**
 * hides all the windows and controls related to the client.
 *
 * @function   hideClientWindows
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
 * @signature  static void hideClientWindows(ClientWnds* clientWnds)
 *
 * @param      clientWnds pointer to the ClientWnds structure that contains
 *   window handles to all the client windows.
 */
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
    ShowWindow(clientWnds->hFile,               SW_HIDE);
    ShowWindow(clientWnds->hPacketCount,        SW_HIDE);
}

/**
 * shows all the windows and controls related to the client.
 *
 * @function   showClientWindows
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
 * @signature  static void showClientWindows(ClientWnds* clientWnds)
 *
 * @param      clientWnds pointer to the ClientWnds structure that contains
 *   window handles to all the client windows.
 */
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
    ShowWindow(clientWnds->hFile,               SW_SHOW);
    ShowWindow(clientWnds->hPacketCount,        SW_SHOW);
}

/**
 * hides all the windows and controls related to the server.
 *
 * @function   hideServerWindows
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
 * @signature  static void hideServerWindows(ServerWnds* serverWnds)
 *
 * @param      serverWnds pointer to the ServerWnds structure that contains
 *   window handles to all the server windows.
 */
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

/**
 * shows all the windows and controls related to the server.
 *
 * @function   showServerWindows
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
 * @signature  static void showServerWindows(ServerWnds* serverWnds)
 *
 * @param      serverWnds pointer to the ServerWnds structure that contains
 *   window handles to all the server windows.
 */
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
