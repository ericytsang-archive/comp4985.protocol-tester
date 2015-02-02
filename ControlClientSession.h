#ifndef CONTROLCLIENTSESSION_H
#define CONTROLCLIENTSESSION_H

#include "Session.h"
#include "Main.h"

struct CtrlClntSession
{
    struct ClientWnds* clientWnds;
    struct Session* ctrlSession;
    struct Session* testSession;

    int lastParsedSection;
    char msgType;
    int msgLen;
};

typedef struct CtrlClntSession CtrlClntSession;

void ctrlClntSessionOnMessage(Session*, char*, int);
void ctrlClntSessionOnError(Session*, int, int);
void ctrlClntSessionOnClose(Session*, int);

#endif
