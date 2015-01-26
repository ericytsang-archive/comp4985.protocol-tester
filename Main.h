
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <windows.h>

#pragma comment(lib, "Ws2_32.lib")

/** name of the application, used in the window title. */
#define APP_NAME "Protocol Tester"

/** relative positioning. */
#define PADDING_TOP_GROUPBOX 15
#define PADDING 5
#define TEXT_HEIGHT 22

/** absolute positioning. */
#define COLUMN_1_WIDTH 120
#define COLUMN_2_WIDTH 150
#define COLUMN_3_WIDTH 20
#define GROUP_WIDTH COLUMN_1_WIDTH+COLUMN_2_WIDTH+COLUMN_3_WIDTH+PADDING*4

#define BUTTON_WIDTH 100
#define BUTTON_HEIGHT 30

/** window identifiers. */
#define IDC_EDIT_HOST       108
#define IDC_EDIT_IP         109
#define IDC_EDIT_SERVICE    110
#define IDC_EDIT_PORT       111
#define IDC_EDIT_PROTOCOL   112

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void updateDisplay(HWND);

