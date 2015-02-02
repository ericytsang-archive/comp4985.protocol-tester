#include "ControlClientSession.h"

void ctrlClntSessionOnMessage(Session* session, char* str, int len)
{
    char output[MAX_STRING_LEN];

    // parse user parameters
    CtrlClntSession* ctrlSession = (CtrlClntSession*) session->usrPtr;

    // parse packet
    switch(ctrlSession->lastParsedSection % PACKET_SECTIONS)
    {
        case PACKET_SEQ_TYPE:       // parse packet header
        {
            ctrlSession->msgType = str[0];
            sessionSetBufLen(session, PACKET_LEN_LENGTH);
            break;
        }
        case PACKET_SEQ_LENGTH:     // parse packet payload length
        {
            ctrlSession->msgLen = *((int*) str);
            sessionSetBufLen(session, ctrlSession->msgLen);
            break;
        }
        case PACKET_SEQ_PAYLOAD:    // parse packet payload & take action
        {
            sessionSetBufLen(session, PACKET_LEN_TYPE);

            switch(ctrlSession->msgType)
            {
                case MSG_CHAT:
                {
                    sprintf_s(output, "Control: %.*s\r\n", len, str);
                    appendWindowText(ctrlSession->clientWnds->hOutput, output);
                    break;
                }
                default:
                {
                    sprintf_s(output, "UNKNOWN MSG TYPE: %.*s\r\n", len, str);
                    appendWindowText(ctrlSession->clientWnds->hOutput, output);
                    break;
                }
            }
            break;
        }
    }

    if(++(ctrlSession->lastParsedSection) >= PACKET_SECTIONS)
    {
        ctrlSession->lastParsedSection -= PACKET_SECTIONS;
    }
}

void ctrlClntSessionOnError(Session* session, int errCode, int winErrCode)
{
    char output[MAX_STRING_LEN];
    CtrlClntSession* ctrlSession = (CtrlClntSession*) session->usrPtr;
    sprintf_s(output, "Control encountered an error: %s - %d\r\n",
        rctoa(errCode), winErrCode);
    appendWindowText(ctrlSession->clientWnds->hOutput, output);
}

void ctrlClntSessionOnClose(Session* session, int code)
{
    char output[MAX_STRING_LEN];
    CtrlClntSession* ctrlSession = (CtrlClntSession*) session->usrPtr;
    sprintf_s(output, "Control disconnected: %s\r\n", rctoa(code));
    appendWindowText(ctrlSession->clientWnds->hOutput, output);
}
