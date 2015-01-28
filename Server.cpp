#include "Server.h"

static struct Server
{
    short controlPort;
    char* logFilePath;
    SOCKET acceptSocket;
};

typedef struct Server Server;

int serverInit(Server*);
int serverStart(Server*);
int serverStop(Server*);
int serverSendMessage(Server*, char*);
int serverSetOnMessage(Server*, char*);

int serverInit(Server* server)
{
    server->controlPort = 0;
    server->logFilePath = 0;
    server->acceptSocket = 0;
    return 0;
}

int serverStart(Server* server)
{
    SOCKET sd;  // assigned to server->accrptSocket

    if(server->acceptSocket != 0)
    {
        perror("socket already opened\n");
        return -1;
    }

    // opening a socket for listening
    server->acceptSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (server->acceptSocket == -1)
    {
        perror("couldn't create a socket\n");
        return -2;
    }

    // accept connections until the server ends
    /*sessionsocket = accept(server->acceptSocket, NULL, NULL);

    // receive from the socket
    if (WSARecv(sessionsocket, &DataBuf, 1, &RecvBytes, &Flags, &Overlapped, 
	WorkerRoutine) == SOCKET-ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			printf("WSARecv() failed with error %d\n", WSAGetLastError()); 
			return;
		}
	}*/
}
