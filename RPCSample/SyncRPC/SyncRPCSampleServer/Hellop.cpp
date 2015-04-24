#include <stdio.h>
#include <windows.h>
#include "hello_h.h"

void HelloProc(wchar_t * pszString)
{
    wprintf(L"%s\n", pszString);
}

void HelloProc1(wchar_t * pszString)
{
    wprintf(L"%s1\n", pszString);
}

void Shutdown(void)
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