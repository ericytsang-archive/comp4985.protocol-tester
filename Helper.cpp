#include "Helper.h"

void appendWindowText(HWND window, char* newText)
{
    DWORD l,r;
    int textLength = SendMessage(window, WM_GETTEXTLENGTH, 0, 0);
    SendMessage(window, EM_GETSEL, (WPARAM)&l, (LPARAM)&r);

    SendMessage(window, EM_SETSEL, textLength, textLength);
    SendMessage(window, EM_REPLACESEL, 0, (LPARAM)newText);

    if(textLength > MAX_CHARS_IN_TEXTBOX)
    {
        SendMessage(window, EM_SETSEL, 0, textLength-(MAX_CHARS_IN_TEXTBOX/2));
        SendMessage(window, EM_REPLACESEL, 0, (LPARAM)"");
    }

    SendMessage(window, EM_SETSEL, l, r);
    SendMessage(window, WM_VSCROLL, SB_BOTTOM, NULL);
}
