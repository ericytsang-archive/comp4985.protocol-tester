#ifndef TESTSESSION_H
#define TESTSESSION_H

void testSvrInit(Server*, ServerWnds*);

struct TestSvrSession
{
    struct CtrlSvrSession* ctrlSvrSession;
};

#endif
