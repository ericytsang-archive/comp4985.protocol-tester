#include "Helper.h"

void appendWindowText(HWND window, char* newText)
{
    DWORD l,r;
    SendMessage(window, EM_GETSEL,(WPARAM)&l,(LPARAM)&r);
    SendMessage(window, EM_SETSEL, -1, -1);
    SendMessage(window, EM_REPLACESEL, 0, (LPARAM)newText);
    SendMessage(window, EM_SETSEL,l,r);
}
