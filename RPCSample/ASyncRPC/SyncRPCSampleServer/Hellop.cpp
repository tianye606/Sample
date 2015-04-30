#include <stdio.h>
#include <windows.h>
#include "hello_h.h"

void HelloProc(IN OUT PRPC_ASYNC_STATE pAsync, wchar_t * pszString)
{
    wprintf(L"%s\n", pszString);
    Sleep(10000);
    int nReply = 100;
    RPC_STATUS status = RpcAsyncCompleteCall(pAsync, &nReply);
    wprintf(L"RpcAsyncCompleteCall 0x%x\n",status);
}

int SyncHelloProc(wchar_t * pszString)
{
    wprintf(L"%s\n", pszString);
    Sleep(10000);
    return 1;
}

void Shutdown()
{
    RPC_STATUS status;

    status = RpcMgmtStopServerListening(NULL);

    if (status)
    {
        exit(status);
    }

    status = RpcServerUnregisterIf(NULL, NULL, FALSE);

    if (status)
    {
        exit(status);
    }
}