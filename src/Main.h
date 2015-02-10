/**
 * exposes the interface functions of Main.cpp, and defines the structuress
 *   containing all the window handles.
 *
 * @sourceFile Main.h
 *
 * @program    ProtocolTester.exe
 *
 * @functions  int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
 * @functions  LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
 * @functions  char* rctoa(int returnCode);
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
 */
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

/** length of buffer used to store temporary strings and things. */
#define MAX_STRING_LEN 1024

/** relative positioning for GUI layout. */
#define PADDING_TOP_GROUPBOX 15
#define PADDING 5
#define TEXT_HEIGHT 22

/** absolute positioning for GUI layout. */
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

/**
 * structure that contains all the window handles to all the windows related to
 *   the server.
 */
struct ClientWnds
{
    HWND hRemoteAddress;        // input field for remote address
    HWND hProtocolParameters;   // groupbox grouping all the protocol parameters
    HWND hDataParameters;       // groupbox grouping all the data parameters

    HWND hConnect;      // connect button connects to remote server
    HWND hDisconnect;   // disconnect button disconnect from remote server
    HWND hTest;         // test button used to start the network test

    HWND hOutput;       // output area where text is displayed
    HWND hInput;        // input area where the user can enter chat messages
    HWND hSend;         // send button used to send chat messages

    HWND hIPHostLabel;      // label beside the hIpHost input control
    HWND hTestPortLabel;    // label beside the hTestPort input control
    HWND hControlPortLabel; // label beside the hCtrlPort input control

    HWND hIpHost;   // used to specify the IP address of the remote server
    HWND hTestPort; // used to specify the remote test port to connect to
    HWND hCtrlPort; // used to specify the remote control port to connect to

    HWND hTcp;          // TCP radio button used to specify which protocol to test
    HWND hUdp;          // UDP radio button used to specify which protocol to test
    HWND hPacketSize;   // input control indicating the size of sent packets

    HWND hPacketSizeLabel;      // label beside the hPacketSize control
    HWND hPacketsCountLabel;    // label beside the hPacketCount control

    HWND hSendFile;             // radio button indicating to send data from file
    HWND hSendGeneratedData;    // radio button indicating to send generated data


    HWND hChooseFile;   // input control indicating where the file to send is
    HWND hBrowseFile;   // button used to open a file chooser to specify a file
    HWND hFile;         // input control with path to file to read from
    HWND hPacketCount;  // number of packets to generate when sending generated data
};

typedef struct ClientWnds ClientWnds;

/**
 * structure that contains all the window handles to all the windows related to
 *   the client.
 */
struct ServerWnds
{
    HWND hPort;         // field where the local control port is specified
    HWND hFile;         // field specifying where file to open and write to is
    HWND hBrowseFile;   // button clicked opening a file open dialog

    HWND hStart;    // button used to start the server
    HWND hStop;     // button used to stop the server

    HWND hOutput;   // output area where text is displayed
    HWND hInput;    // input area where the user can enter chat messages
    HWND hSend;     // send button used to send chat messages

    HWND hSvrOptionsBroupBox;   // groupbox grouping all the server controls
    HWND hCtrlPortLabel;        // label beside the hPort input
    HWND hFileLabel;            // label beside the hFile input
};

typedef struct ServerWnds ServerWnds;

/**
 * structure that contains all the window handles to all the windows related to
 *   both the server and the client.
 */
struct CommonWnds
{
    HWND hBackground;   // background of the application
};

typedef struct CommonWnds CommonWnds;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
char* rctoa(int returnCode);

#endif
