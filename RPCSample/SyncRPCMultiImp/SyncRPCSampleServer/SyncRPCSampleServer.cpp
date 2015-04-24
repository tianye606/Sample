// SyncRPCSampleServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "hello_h.h"
void HelloProc1(wchar_t * pszString);

//Define EPV and will be initialized to call HelloProc1, notice the routin position must match the method in the interface
typedef struct _if_hello_SERVER_EPV{
    void(*HelloProc) (wchar_t *);
    void(*Shutdown) (void);
} if_hello_SERVER_EPV;

int _tmain(int argc, _TCHAR* argv[])
{
    RPC_STATUS status;
    TCHAR * pszProtocolSequence = _TEXT("ncacn_np");
    TCHAR * pszSecurity = NULL;
    TCHAR * pszEndpoint = _TEXT("\\pipe\\hello");
    unsigned int    cMinCalls = 1;
    unsigned int    fDontWait = FALSE;
    UUID hello_obj = { 0x9E9641AE, 0x40CA, 0x4D0D, { 0x87, 0xB5, 0x57, 0xA1, 0x96, 0x61, 0xE7, 0xA8 }};
    status = RpcServerUseProtseqEp(reinterpret_cast<RPC_WSTR>(pszProtocolSequence),
        RPC_C_LISTEN_MAX_CALLS_DEFAULT,
        reinterpret_cast<RPC_WSTR>(pszEndpoint),
        pszSecurity);

    if (status) exit(status);
    if_hello_SERVER_EPV hello_EPV{ HelloProc1, Shutdown };

    status = RpcServerRegisterIf(hello_v1_0_s_ifspec,
        &hello_obj,
        &hello_EPV);

    if (status) exit(status);

    //Map Object UUID to Type UUID, Object UUID will be used in client binding and then find the type UUID by map,
    //And find the SERVER_EPV to execute the routing by the position
    RpcObjectSetType(&hello_obj, &hello_obj);

    status = RpcServerListen(cMinCalls,
        RPC_C_LISTEN_MAX_CALLS_DEFAULT,
        fDontWait);

    if (status) exit(status);
}

