// SyncRPCSampleServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "hello_h.h"
void HelloProc1(wchar_t * pszString);

int _tmain(int argc, _TCHAR* argv[])
{
    RPC_STATUS status;
    TCHAR * pszProtocolSequence = _TEXT("ncacn_np");
    TCHAR * pszSecurity = NULL;
    TCHAR * pszEndpoint = _TEXT("\\pipe\\hello");
    unsigned int    cMinCalls = 1;
    unsigned int    fDontWait = FALSE;

    status = RpcServerUseProtseqEp(reinterpret_cast<RPC_WSTR>(pszProtocolSequence),
        RPC_C_LISTEN_MAX_CALLS_DEFAULT,
        reinterpret_cast<RPC_WSTR>(pszEndpoint),
        pszSecurity);

    if (status) exit(status);

    status = RpcServerRegisterIf(hello_v1_0_s_ifspec,
        NULL,
        NULL);

    if (status) exit(status);

    status = RpcServerListen(cMinCalls,
        RPC_C_LISTEN_MAX_CALLS_DEFAULT,
        fDontWait);

    if (status) exit(status);
}

