// SyncRPCSample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "hello_h.h"

void CallHelloProc(TCHAR * pszString)
{
    RPC_STATUS status;
    TCHAR * pszUuid = NULL;
    TCHAR * pszProtocolSequence = TEXT("ncacn_np");
    TCHAR * pszNetworkAddress = NULL;
    TCHAR * pszEndpoint = TEXT("\\pipe\\hello");
    TCHAR * pszOptions = NULL;
    TCHAR * pszStringBinding = NULL;
    //TCHAR * pszString = TEXT("hello, world");
    unsigned long ulCode;

    status = RpcStringBindingCompose(reinterpret_cast<RPC_WSTR>(pszUuid),
        reinterpret_cast<RPC_WSTR>(pszProtocolSequence),
        reinterpret_cast<RPC_WSTR>(pszNetworkAddress),
        reinterpret_cast<RPC_WSTR>(pszEndpoint),
        reinterpret_cast<RPC_WSTR>(pszOptions),
        reinterpret_cast<RPC_WSTR *>(&pszStringBinding));
    if (status) exit(status);

    status = RpcBindingFromStringBinding(reinterpret_cast<RPC_WSTR>(pszStringBinding), &hello_IfHandle);    
    if (status) exit(status);

    RpcTryExcept
    {
        int reply=HelloProc(pszString);
        printf("Synchronous call to HelloProc is completed with reply %d", reply);
        Shutdown();
    }
    RpcExcept(1)
    {
        ulCode = RpcExceptionCode();
        printf("Runtime reported exception 0x%lx = %ld\n", ulCode, ulCode);
    }
    RpcEndExcept

    status = RpcStringFree(reinterpret_cast<RPC_WSTR *>(&pszStringBinding));

    if (status) exit(status);

    status = RpcBindingFree(&hello_IfHandle);

    if (status) exit(status);

    exit(0);
}

void main(void)
{
    TCHAR * pszString = TEXT("Hello from synchronous client");
    CallHelloProc(pszString);
}

