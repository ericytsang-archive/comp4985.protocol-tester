#ifndef CONTROLCLIENT_H
#define CONTROLCLIENT_H

#include "ControlClientSession.h"
#include "Session.h"
#include "Main.h"

#define MAX_STRING_LEN 1024

struct CtrlClnt
{
    struct ClientWnds* clientWnds;

    struct Client* client;

    struct Session* ctrlSession;
    struct Session* testSession;

    char remoteAddress[MAX_STRING_LEN];
    unsigned short testPort;
    unsigned short ctrlPort;

    int testProtocol;
    int testPacketSize;

    int dataSource;
    char filePath[MAX_STRING_LEN];
    int packetsToSend;
    int bytesToSend;
};

typedef struct CtrlClnt CtrlClnt;

void ctrlClntInit(Client*, struct ClientWnds*);

void ctrlClntConnectCtrl(Client*);
void ctrlClntConnectTest(Client*);
void ctrlClntDisonnect(Client*);
void ctrlClntStartTest(Client*);
void ctrlClientSendChat(Client*);

void ctrlClntSetDataSource(Client*, int);
void ctrlClntSetTestProtocol(Client*, int);

#endif
