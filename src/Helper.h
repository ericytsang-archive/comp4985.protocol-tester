/**
 * exposes the interface of the Helper.cpp
 *
 * @sourceFile Helper.h
 *
 * @program    ProtocolTester.exe
 *
 * @function   void appendWindowText(HWND window, char* newText);
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
#ifndef HELPER_H
#define HELPER_H

#include <windows.h>

#define MAX_CHARS_IN_TEXTBOX 10000

void appendWindowText(HWND window, char* newText);

#endif
