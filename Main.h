#ifndef MAIN_H
#define MAIN_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <windows.h>
#include "Server.h"
#include "Client.h"
#include "Session.h"
#include "Helper.h"
#include "LinkedList.h"
#include "Packet.h"
#include "ControlServer.h"
#include "ControlClient.h"
#include "ReturnCodes.h"

#pragma comment(lib, "Ws2_32.lib")

/** name of the application, used in the window title. */
#define APP_NAME "Protocol Tester"

/** length of buffer used to store strings and things. */
#define MAX_STRING_LEN 1024

/** relative positioning. */
#define PADDING_TOP_GROUPBOX 15
#define PADDING 5
#define TEXT_HEIGHT 22

/** absolute positioning. */
#define COLUMN_1_WIDTH 120
#define COLUMN_2_WIDTH 150
#define COLUMN_3_WIDTH 30
#define GROUP_WIDTH COLUMN_1_WIDTH+COLUMN_2_WIDTH+COLUMN_3_WIDTH+PADDING*4
#define BUTTON_WIDTH 100
#define BUTTON_HEIGHT 30

/** control identifiers. */
#define IDC_TCP                 108
#define IDC_UDP                 109
#define IDC_SEND_FILE           110
#define IDC_SEND_GENERATED_DATA 111
#define IDC_BROWSE_FILE         112
#define IDC_CONNECT             113
#define IDC_DISCONNECT          114
#define IDC_TEST                115
#define IDC_SEND_MESSAGE        116
#define IDC_MODE_SERVER         117
#define IDC_MODE_CLIENT         118
#define IDC_HELP                119
#define IDC_START_SERVER        120
#define IDC_STOP_SERVER         121

/** control message types. */
#define MSG_CHAT         'A'
#define MSG_SET_PROTOCOL 'B'
#define MSG_SET_PORT     'C'
#define MSG_START_TEST   'D'

/** modes. */
#define MODE_UNDEFINED      0
#define MODE_CLIENT         1
#define MODE_SERVER         2
#define MODE_TCP            3
#define MODE_UDP            4
#define MODE_FROM_FILE      5
#define MODE_FROM_GENERATOR 6
#define MODE_TO_FILE        7
#define MODE_TO_NOTHING     8

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

typedef struct ClientWnds ClientWnds;

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

typedef struct ServerWnds ServerWnds;

struct CommonWnds
{
    HWND hBackground;
};

typedef struct CommonWnds CommonWnds;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
char* rctoa(int returnCode);

#endif
