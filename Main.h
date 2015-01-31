#ifndef MAIN_H
#define MAIN_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <windows.h>
#include "Server.h"
#include "Session.h"
#include "Helper.h"
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
#define IDC_TEST                114
#define IDC_SEND_MESSAGE        115
#define IDC_MODE_SERVER         116
#define IDC_MODE_CLIENT         117
#define IDC_HELP                118
#define IDC_START_SERVER        119
#define IDC_STOP_SERVER         120

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

#endif
