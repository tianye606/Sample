// SyncRPCSampleServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "hello_h.h"
void HelloProc1(wchar_t * pszString);

// Naive security callback.
RPC_STATUS CALLBACK SecurityCallback(RPC_IF_HANDLE /*hInterface*/, void* /*pBindingHandle*/)
{
    return RPC_S_OK; // Always allow anyone.
}

int _tmain(int argc, _TCHAR* argv[])
{
    RPC_STATUS status;
    TCHAR * pszProtocolSequence = _TEXT("ncacn_ip_tcp");
    TCHAR * pszSecurity = NULL;
    TCHAR * pszEndpoint = _TEXT("4747");
    unsigned int    cMinCalls = 1;
    unsigned int    fDontWait = FALSE;

    status = RpcServerUseProtseqEp(reinterpret_cast<RPC_WSTR>(pszProtocolSequence),
        RPC_C_LISTEN_MAX_CALLS_DEFAULT,
        reinterpret_cast<RPC_WSTR>(pszEndpoint),
        pszSecurity);

    if (status) exit(status);

    //status = RpcServerRegisterIf(hello_v1_0_s_ifspec,
    //    NULL,
    //    NULL);

    //Answer::Server will handle requests from the same binding one by one, so in order to make the async call really in parallel, we need client use different binding for each async call
    //It was found out by analyzing netlog, two async call will use the same binding when using implicit handl, but async call and sync call will use different binding
    // use ip_tcp must need authtication, or will receive 5 (Access dinied), so must call RegisterIf2
    //ncacn_np doesn't require authentication, so can call RegisterIf
    status = RpcServerRegisterIf2(
        hello_v1_0_s_ifspec, // Interface to register.
        NULL, // Use the MIDL generated entry-point vector.
        NULL, // Use the MIDL generated entry-point vector.
        RPC_IF_ALLOW_CALLBACKS_WITH_NO_AUTH, // Forces use of security callback.
        RPC_C_LISTEN_MAX_CALLS_DEFAULT, // Use default number of concurrent calls.
        (unsigned)-1, // Infinite max size of incoming data blocks.
        SecurityCallback); // Naive security callback.

    if (status) exit(status);

    status = RpcServerListen(cMinCalls,
        RPC_C_LISTEN_MAX_CALLS_DEFAULT,
        fDontWait);

    if (status) exit(status);
}

