/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: Application.cpp - The gui frontend for network resolving
--
-- PROGRAM: Network Resolver
--
-- FUNCTIONS:
-- LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
-- int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
-- void updateDisplay(HWND)
--
-- DATE: January 13th, 2015
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Lewis Scott
--
-- PROGRAMMER: Lewis Scott
--
-- NOTES:
------------------------------------------------------------------------------------------------------------------*/
#define STRICT
#include "Main.h"

static HWND hWnd;
static HWND hIpHost;
static HWND hTestPort;
static HWND hCtrlPort;
static HWND hTcp;
static HWND hUdp;
static HWND hPacketSize;
static HWND hPacketCount;
static HWND hByteCount;
static HWND hSendFile;
static HWND hBrowseFile;
static HWND hSendGeneratedData;
static HWND hConnect;
static HWND hTest;
static HWND hOutput;
static HWND hInput;
static HWND hSend;

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: WinMain
--
-- DATE: January 13th, 2015
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Lewis Scott
--
-- PROGRAMMER: Lewis Scott
--
-- INTERFACE: int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lspszCmdParam, int nCmdShow)
--
-- RETURNS: int: Status
--
-- NOTES:
-- Entry point into win32.
----------------------------------------------------------------------------------------------------------------------*/
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lspszCmdParam, int nCmdShow)
{
    TCHAR Title[] = TEXT(APP_NAME);
    MSG Msg;
    WNDCLASSEX Wcl;
    WSADATA wsaData;
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
    hWnd = CreateWindow((LPCSTR)Title, (LPCSTR)Title, WS_OVERLAPPEDWINDOW, 300, 300, 500, 500, NULL, NULL, hInst, NULL);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    while (GetMessage(&Msg, NULL, 0, 0))
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: WndProc
--
-- DATE: January 13th, 2015
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Lewis Scott
--
-- PROGRAMMER: Lewis Scott
--
-- INTERFACE: LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
--
-- RETURNS: LRESULT: was the message processed?
--
-- NOTES:
-- Message handler for the application
----------------------------------------------------------------------------------------------------------------------*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{

    switch (Message)
    {
    case WM_CREATE:
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
                hWnd, (HMENU)IDC_EDIT_HOST,
                GetModuleHandle(NULL), NULL);
            CreateWindowEx(NULL,
                "Button", "Protocol Parameters",
                WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
                ProtocolParamsGroupX, ProtocolParamsGroupY,
                ProtocolParamsGroupW, ProtocolParamsGroupH,
                hWnd, (HMENU)IDC_EDIT_HOST,
                GetModuleHandle(NULL), NULL);
            CreateWindowEx(NULL,
                "Button", "Data Parameters",
                WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
                DataParamsGroupX, DataParamsGroupY,
                DataParamsGroupW, DataParamsGroupH,
                hWnd, (HMENU)IDC_EDIT_HOST,
                GetModuleHandle(NULL), NULL);
            hConnect = CreateWindowEx(WS_EX_CLIENTEDGE,
                "Button", "Connect", WS_CHILD | WS_VISIBLE,
                0, 0, 0, 0,
                hWnd, (HMENU)IDC_EDIT_PROTOCOL,
                GetModuleHandle(NULL), NULL);
            hTest = CreateWindowEx(WS_EX_CLIENTEDGE,
                "Button", "Begin Test", WS_CHILD | WS_VISIBLE,
                0, 0, 0, 0,
                hWnd, (HMENU)IDC_EDIT_PROTOCOL,
                GetModuleHandle(NULL), NULL);
            hOutput = CreateWindowEx(WS_EX_CLIENTEDGE,
                "Edit", "bjads hfbjsk hd\njhbjhb fjgb\nkjsd fngksdfjg nkjn\nasda sdasdsdfjg nkjn\nasda sdasdsdfjg nkjn\nasda sdasdsdfjg nkjn\nasda sdasdsdfjg nkjn\nasda sdasdsdfjg nkjn\nasda sdasdsdfjg nkjn\nasda sdasdsdfjg nkjn\nasda sdasdsdfjg nkjn\nasda sdasdsdfjg nkjn\nasda sdasdsdfjg nkjn\nasda sdasdsdfjg nkjn\nasda sdasdasdj khlabdsfjah bsdkfjh baskdjfhb akjshdb fkjahbs",
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY,
                0, 0, 0, 0, hWnd, (HMENU)IDC_EDIT_PROTOCOL,
                GetModuleHandle(NULL), NULL);
            hInput = CreateWindowEx(WS_EX_CLIENTEDGE,
                "Edit", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                0, 0, 0, 0, hWnd, (HMENU)IDC_EDIT_PROTOCOL,
                GetModuleHandle(NULL), NULL);
            hSend = CreateWindowEx(WS_EX_CLIENTEDGE,
                "Button", "Send", WS_CHILD | WS_VISIBLE,
                0, 0, 0, 0,
                hWnd, (HMENU)IDC_EDIT_PROTOCOL,
                GetModuleHandle(NULL), NULL);

            // remote address controls
            CreateWindowEx(NULL,
                "Static", "IP / Host Name:",
                WS_CHILD | WS_VISIBLE,
                RemoteAddrGroupX+PADDING, RemoteAddrGroupY+PADDING_TOP_GROUPBOX+PADDING,
                COLUMN_1_WIDTH, TEXT_HEIGHT,
                hWnd, (HMENU)IDC_EDIT_HOST,
                GetModuleHandle(NULL), NULL);
            CreateWindowEx(NULL,
                "Static", "Test Port:",
                WS_CHILD | WS_VISIBLE,
                RemoteAddrGroupX+PADDING, RemoteAddrGroupY+PADDING_TOP_GROUPBOX+PADDING*2+TEXT_HEIGHT,
                COLUMN_1_WIDTH, TEXT_HEIGHT,
                hWnd, (HMENU)IDC_EDIT_HOST,
                GetModuleHandle(NULL), NULL);
            CreateWindowEx(NULL,
                "Static", "Control Port:",
                WS_CHILD | WS_VISIBLE,
                RemoteAddrGroupX+PADDING, RemoteAddrGroupY+PADDING_TOP_GROUPBOX+PADDING*3+TEXT_HEIGHT*2,
                COLUMN_1_WIDTH, TEXT_HEIGHT,
                hWnd, (HMENU)IDC_EDIT_HOST,
                GetModuleHandle(NULL), NULL);
            hIpHost = CreateWindowEx(WS_EX_CLIENTEDGE,
                "Edit", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                RemoteAddrGroupX+COLUMN_1_WIDTH+PADDING*2, RemoteAddrGroupY+PADDING_TOP_GROUPBOX+PADDING,
                COLUMN_2_WIDTH+COLUMN_3_WIDTH+PADDING, TEXT_HEIGHT,
                hWnd, (HMENU)IDC_EDIT_HOST,
                GetModuleHandle(NULL), NULL);
            hTestPort = CreateWindowEx(WS_EX_CLIENTEDGE,
                "Edit", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                RemoteAddrGroupX+COLUMN_1_WIDTH+PADDING*2, RemoteAddrGroupY+PADDING_TOP_GROUPBOX+PADDING*2+TEXT_HEIGHT,
                COLUMN_2_WIDTH+COLUMN_3_WIDTH+PADDING, TEXT_HEIGHT,
                hWnd, (HMENU)IDC_EDIT_IP,
                GetModuleHandle(NULL), NULL);
            hCtrlPort = CreateWindowEx(WS_EX_CLIENTEDGE,
                "Edit", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                RemoteAddrGroupX+COLUMN_1_WIDTH+PADDING*2, RemoteAddrGroupY+PADDING_TOP_GROUPBOX+PADDING*3+TEXT_HEIGHT*2,
                COLUMN_2_WIDTH+COLUMN_3_WIDTH+PADDING, TEXT_HEIGHT,
                hWnd, (HMENU)IDC_EDIT_IP,
                GetModuleHandle(NULL), NULL);

            // protocol parameters controls
            hTcp = CreateWindowEx(NULL,
                "Button", "TCP",
                WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
                ProtocolParamsGroupX+PADDING, ProtocolParamsGroupY+PADDING_TOP_GROUPBOX+PADDING,
                COLUMN_1_WIDTH, TEXT_HEIGHT,
                hWnd, (HMENU)IDC_EDIT_HOST,
                GetModuleHandle(NULL), NULL);
            hUdp = CreateWindowEx(NULL,
                "Button", "UDP",
                WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
                ProtocolParamsGroupX+PADDING*2+COLUMN_1_WIDTH, ProtocolParamsGroupY+PADDING_TOP_GROUPBOX+PADDING,
                COLUMN_2_WIDTH, TEXT_HEIGHT,
                hWnd, (HMENU)IDC_EDIT_HOST,
                GetModuleHandle(NULL), NULL);
            CreateWindowEx(NULL,
                "Static", "Packet Size:",
                WS_CHILD | WS_VISIBLE,
                ProtocolParamsGroupX+PADDING, ProtocolParamsGroupY+PADDING_TOP_GROUPBOX+PADDING*2+TEXT_HEIGHT,
                COLUMN_1_WIDTH, TEXT_HEIGHT,
                hWnd, (HMENU)IDC_EDIT_HOST,
                GetModuleHandle(NULL), NULL);
            hPacketSize = CreateWindowEx(WS_EX_CLIENTEDGE,
                "Edit", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                ProtocolParamsGroupX+COLUMN_1_WIDTH+PADDING*2, ProtocolParamsGroupY+PADDING_TOP_GROUPBOX+PADDING*2+TEXT_HEIGHT,
                COLUMN_2_WIDTH+COLUMN_3_WIDTH+PADDING, TEXT_HEIGHT,
                hWnd, (HMENU)IDC_EDIT_PROTOCOL,
                GetModuleHandle(NULL), NULL);

            // data parameters controls
            hSendFile = CreateWindowEx(NULL,
                "Button", "Send File",
                WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
                DataParamsGroupX+PADDING, DataParamsGroupY+PADDING_TOP_GROUPBOX+PADDING,
                COLUMN_1_WIDTH+COLUMN_2_WIDTH+COLUMN_3_WIDTH+PADDING*2, TEXT_HEIGHT,
                hWnd, (HMENU)IDC_EDIT_HOST,
                GetModuleHandle(NULL), NULL);
            hSendGeneratedData = CreateWindowEx(NULL,
                "Button", "Send Generated Data",
                WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
                DataParamsGroupX+PADDING, DataParamsGroupY+PADDING_TOP_GROUPBOX+PADDING*2+TEXT_HEIGHT,
                COLUMN_1_WIDTH+COLUMN_2_WIDTH+COLUMN_3_WIDTH+PADDING*2, TEXT_HEIGHT,
                hWnd, (HMENU)IDC_EDIT_HOST,
                GetModuleHandle(NULL), NULL);
            CreateWindowEx(NULL,
                "Static", "Choose File:",
                WS_CHILD | WS_VISIBLE,
                DataParamsGroupX+PADDING, DataParamsGroupY+PADDING_TOP_GROUPBOX+PADDING*3+TEXT_HEIGHT*2,
                COLUMN_1_WIDTH, TEXT_HEIGHT,
                hWnd, (HMENU)IDC_EDIT_HOST,
                GetModuleHandle(NULL), NULL);
            hBrowseFile = CreateWindowEx(NULL,
                "Button", "...",
                WS_CHILD | WS_VISIBLE,
                DataParamsGroupX+COLUMN_1_WIDTH+COLUMN_2_WIDTH+PADDING*3, DataParamsGroupY+PADDING_TOP_GROUPBOX+PADDING*3+TEXT_HEIGHT*2,
                COLUMN_3_WIDTH, TEXT_HEIGHT,
                hWnd, (HMENU)IDC_EDIT_HOST,
                GetModuleHandle(NULL), NULL);
            CreateWindowEx(NULL,
                "Static", "Packets to Send:",
                WS_CHILD | WS_VISIBLE,
                DataParamsGroupX+PADDING, DataParamsGroupY+PADDING_TOP_GROUPBOX+PADDING*4+TEXT_HEIGHT*3,
                COLUMN_1_WIDTH, TEXT_HEIGHT,
                hWnd, (HMENU)IDC_EDIT_HOST,
                GetModuleHandle(NULL), NULL);
            CreateWindowEx(NULL,
                "Static", "Bytes to Send:",
                WS_CHILD | WS_VISIBLE,
                DataParamsGroupX+PADDING, DataParamsGroupY+PADDING_TOP_GROUPBOX+PADDING*5+TEXT_HEIGHT*4,
                COLUMN_1_WIDTH, TEXT_HEIGHT,
                hWnd, (HMENU)IDC_EDIT_HOST,
                GetModuleHandle(NULL), NULL);
            hBrowseFile = CreateWindowEx(WS_EX_CLIENTEDGE,
                "Edit", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                DataParamsGroupX+COLUMN_1_WIDTH+PADDING*2, DataParamsGroupY+PADDING_TOP_GROUPBOX+PADDING*3+TEXT_HEIGHT*2,
                COLUMN_2_WIDTH, TEXT_HEIGHT,
                hWnd, (HMENU)IDC_EDIT_PROTOCOL,
                GetModuleHandle(NULL), NULL);
            hPacketCount = CreateWindowEx(WS_EX_CLIENTEDGE,
                "Edit", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                DataParamsGroupX+COLUMN_1_WIDTH+PADDING*2, DataParamsGroupY+PADDING_TOP_GROUPBOX+PADDING*4+TEXT_HEIGHT*3,
                COLUMN_2_WIDTH+COLUMN_3_WIDTH+PADDING, TEXT_HEIGHT,
                hWnd, (HMENU)IDC_EDIT_PROTOCOL,
                GetModuleHandle(NULL), NULL);
            hByteCount = CreateWindowEx(WS_EX_CLIENTEDGE,
                "Edit", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                DataParamsGroupX+COLUMN_1_WIDTH+PADDING*2, DataParamsGroupY+PADDING_TOP_GROUPBOX+PADDING*5+TEXT_HEIGHT*4,
                COLUMN_2_WIDTH+COLUMN_3_WIDTH+PADDING, TEXT_HEIGHT,
                hWnd, (HMENU)IDC_EDIT_PROTOCOL,
                GetModuleHandle(NULL), NULL);
        }
        break;
    case WM_DESTROY:
        WSACleanup();
        PostQuitMessage(0);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
            // case IDM_EXIT:
            //     WSACleanup();
            //     PostQuitMessage(0);
            //     break;
            // case IDM_CLEAR:
            //     SendMessage(hHost, WM_SETTEXT, (WPARAM)"", '\0');
            //     SendMessage(hIp, WM_SETTEXT, (WPARAM)"", '\0');
            //     SendMessage(hService, WM_SETTEXT, (WPARAM)"", '\0');
            //     SendMessage(hPort, WM_SETTEXT, (WPARAM)"", '\0');
            //     SendMessage(hProtocol, WM_SETTEXT, (WPARAM)"", '\0');
            //     break;
            // case IDM_IP:
            //     findip(hHost, hIp);
            //     break;
            // case IDM_HOST:
            //     findhost(hIp, hHost);
            //     break;
            // case IDM_PORT:
            //     findport(hService, hProtocol, hPort);
            //     break;
            // case IDM_SERVICE:
            //     findserv(hPort, hProtocol, hService);
            //     break;
        }
        break;
    case WM_SIZE:
    //case WM_PAINT:
        updateDisplay(hWnd);
        break;
    default:
        return DefWindowProc(hWnd, Message, wParam, lParam);
    }
    return 0;
}
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: updateDisplay
--
-- DATE: January 13th, 2015
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Lewis Scott
--
-- PROGRAMMER: Lewis Scott
--
-- INTERFACE: void updateDisplay(HWND hwnd)
--
-- RETURNS: void
--
-- NOTES:
-- Redraws the window
----------------------------------------------------------------------------------------------------------------------*/
void updateDisplay(HWND hwnd)
{
    PAINTSTRUCT paintStruct;
    TEXTMETRIC tm;
    HDC hdc;
    RECT windowRect;

    GetClientRect(hwnd, &windowRect);

    MoveWindow(hConnect,
        PADDING, windowRect.bottom-PADDING-BUTTON_HEIGHT,
        BUTTON_WIDTH, BUTTON_HEIGHT, TRUE);
    MoveWindow(hTest,
        BUTTON_WIDTH+PADDING*2, windowRect.bottom-PADDING-BUTTON_HEIGHT,
        BUTTON_WIDTH, BUTTON_HEIGHT, TRUE);
    MoveWindow(hOutput,
        GROUP_WIDTH+PADDING*2, PADDING,
        windowRect.right-(GROUP_WIDTH+PADDING*3), windowRect.bottom-BUTTON_HEIGHT-PADDING*3, TRUE);
    MoveWindow(hInput,
        GROUP_WIDTH+PADDING*2, windowRect.bottom-PADDING-BUTTON_HEIGHT,
        windowRect.right-(BUTTON_WIDTH+GROUP_WIDTH+PADDING*3), BUTTON_HEIGHT, TRUE);
    MoveWindow(hSend,
        windowRect.right-BUTTON_WIDTH-PADDING, windowRect.bottom-BUTTON_HEIGHT-PADDING,
        BUTTON_WIDTH, BUTTON_HEIGHT, TRUE);

    RedrawWindow(hOutput, NULL, NULL, RDW_INVALIDATE);
    RedrawWindow(hTest, NULL, NULL, RDW_INVALIDATE);
    RedrawWindow(hConnect, NULL, NULL, RDW_INVALIDATE);
    RedrawWindow(hInput, NULL, NULL, RDW_INVALIDATE);
    RedrawWindow(hSend, NULL, NULL, RDW_INVALIDATE);
}
