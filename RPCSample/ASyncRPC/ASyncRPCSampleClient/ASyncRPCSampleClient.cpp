// SyncRPCSample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "hello_h.h"
VOID RPC_ENTRY
HelloProcCallback(
__in PRPC_ASYNC_STATE   rpcState,
__in PVOID              Context,
__in RPC_ASYNC_EVENT    Event
)
{
    UNREFERENCED_PARAMETER(Context);
    int nReply=0;
    RpcAsyncCompleteCall(rpcState, &nReply);
    printf("APC Asynchronous call to HelloProc completed with reply %d\r\n", nReply);
    SetEvent(rpcState->UserInfo);
}

DWORD WINAPI HelloReplyByAPC(_In_ LPVOID lpParameter)
{
    RPC_STATUS status;
    RPC_ASYNC_STATE Async;
    unsigned long ulCode;

    // Initialize the ASYNC handle for APC.
    status = RpcAsyncInitializeHandle(&Async, sizeof(RPC_ASYNC_STATE));
    if (status) exit(status);
    //UserInfo can be used to track client context when use asynchronous procedure call (APC), now it is the string pointer to the message we sent to server,
    //then I will show how it was found back in the APC, remember to keep the buffer before it
    HANDLE hAPCComplete = CreateEvent(NULL, FALSE, FALSE, NULL);
    Async.UserInfo = hAPCComplete;
    Async.NotificationType = RpcNotificationTypeApc;
    Async.u.APC.hThread = 0;
    Async.u.APC.NotificationRoutine = HelloProcCallback;

    RpcTryExcept
    {
        HelloProc(&Async, L"Hello Proc by APC");
        printf("APC Asynchronous call to HelloProc returned\r\n");

        //Wait APC to complete, SleepEx will make thread enters alertable state and so APC can be called when ASYNC RPC completes.
        //And it returns when APC completed, wait event hAPCComplete to double confirm APC call completes (should be optional)
        SleepEx(INFINITE, TRUE);
        WaitForSingleObject(hAPCComplete, INFINITE);
    }
    RpcExcept(1)
    {
        ulCode = RpcExceptionCode();
        printf("Runtime reported exception 0x%lx = %ld\n", ulCode, ulCode);
    }
    RpcEndExcept

    return ERROR_SUCCESS;
}

DWORD WINAPI HelloReplyByEvent(_In_ LPVOID lpParameter)
{
    RPC_STATUS status;
    RPC_ASYNC_STATE Async;
    unsigned long ulCode;

    // Initialize the ASYNC handle for Event.
    status = RpcAsyncInitializeHandle(&Async, sizeof(RPC_ASYNC_STATE));
    if (status) exit(status);
    Async.UserInfo = NULL;
    Async.NotificationType = RpcNotificationTypeEvent;
    Async.u.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (Async.u.hEvent == 0)
    {
        exit(status);
    }

    int nReply=0;
    RpcTryExcept
    {
        HelloProc(&Async, L"Hello Proc by Event");
        printf("Event Asynchronous call to HelloProc returned\r\n");

        WaitForSingleObject(Async.u.hEvent, INFINITE);
        RpcAsyncCompleteCall(&Async, &nReply);
        printf("Event call to HelloProc is completed with reply %d\r\n", nReply);
    }
    RpcExcept(1)
    {
        ulCode = RpcExceptionCode();
        printf("Runtime reported exception 0x%lx = %ld\n", ulCode, ulCode);
    }
    RpcEndExcept
    return ERROR_SUCCESS;
}

DWORD WINAPI HelloReply(_In_ LPVOID lpParameter)
{
    RPC_STATUS status;
    RPC_ASYNC_STATE Async, APCAsync;
    unsigned long ulCode;

    // Initialize the ASYNC handle for Event.
    status = RpcAsyncInitializeHandle(&Async, sizeof(RPC_ASYNC_STATE));
    if (status) exit(status);
    Async.UserInfo = NULL;
    Async.NotificationType = RpcNotificationTypeEvent;
    Async.u.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (Async.u.hEvent == 0)
    {
        exit(status);
    }

    // Initialize the ASYNC handle for APC.
    status = RpcAsyncInitializeHandle(&APCAsync, sizeof(RPC_ASYNC_STATE));
    if (status) exit(status);
    //UserInfo can be used to track client context when use asynchronous procedure call (APC), now it is the string pointer to the message we sent to server,
    //then I will show how it was found back in the APC, remember to keep the buffer before it
    HANDLE hAPCComplete = CreateEvent(NULL, FALSE, FALSE, NULL);
    APCAsync.UserInfo = hAPCComplete;
    APCAsync.NotificationType = RpcNotificationTypeApc;
    //NOTES::Cannot use pseudo handle returned by GetCurrentThread, APC will be called even if current thread is not in alertable state when using that pseudo handle (weird??)
    APCAsync.u.APC.hThread = 0;
    APCAsync.u.APC.NotificationRoutine = HelloProcCallback;

    int nReply = 0;
    RpcTryExcept
    {
        HelloProc(&Async, L"Hello Proc by Event");
        printf("Event Asynchronous call to HelloProc returned\r\n");

        HelloProc(&APCAsync, L"Hello Proc by APC");
        printf("APC Asynchronous call to HelloProc returned\r\n");

        WaitForSingleObjectEx(Async.u.hEvent, INFINITE,TRUE);
        RpcAsyncCompleteCall(&Async, &nReply);
        printf("Event call to HelloProc is completed with reply %d\r\n", nReply);

        //Wait APC to complete, SleepEx will return when APC completed
        SleepEx(INFINITE, TRUE);
        WaitForSingleObject(hAPCComplete, INFINITE);
    }
    RpcExcept(1)
    {
        ulCode = RpcExceptionCode();
        printf("Runtime reported exception 0x%lx = %ld\n", ulCode, ulCode);
    }
    RpcEndExcept
        return ERROR_SUCCESS;
}

void main(void)
{
    TCHAR * pszUuid = NULL;
    TCHAR * pszProtocolSequence = TEXT("ncacn_ip_tcp");
    TCHAR * pszNetworkAddress = TEXT("yongyang_r1");
    TCHAR * pszEndpoint = TEXT("4747");
    TCHAR * pszOptions = NULL;
    TCHAR * pszStringBinding = NULL;
    //TCHAR * pszString = TEXT("hello, world");

    RPC_STATUS status = RpcStringBindingCompose(reinterpret_cast<RPC_WSTR>(pszUuid),
        reinterpret_cast<RPC_WSTR>(pszProtocolSequence),
        reinterpret_cast<RPC_WSTR>(pszNetworkAddress),
        reinterpret_cast<RPC_WSTR>(pszEndpoint),
        reinterpret_cast<RPC_WSTR>(pszOptions),
        reinterpret_cast<RPC_WSTR *>(&pszStringBinding));
    if (status) exit(status);

    status = RpcBindingFromStringBinding(reinterpret_cast<RPC_WSTR>(pszStringBinding), &hello_IfHandle);
    if (status) exit(status);
    //SyncHelloProc(L"test");

    //Notes::WT_EXECUTEINPERSISTENTTHREAD flag will cause two call back functions are executed in the same thread, so these two callbacks are executed by sequence (not in parallel) 
    //MSDN said WT_EXECUTEDEFAULT flag will cause the callback function is queued to a non-I/O worker thread, and so its APC cannot 
    //be guarantee to be called, which also means the thread will be alertable if you call these SleepEx, WaitForSignleObject functions in the callbacks explicitly.
    //QueueUserWorkItem(HelloReplyByAPC, NULL, WT_EXECUTEDEFAULT);
    //QueueUserWorkItem(HelloReplyByEvent, NULL, WT_EXECUTEDEFAULT);
    //Sleep(40000);

    //Notes::Why two RPC (RpcNotificationTypeApc and RpcNotificationTypeEvent) on server are called one bye one, 
    //RpcNotificationTypeApc completes in 10s and RpcNotificationTypeEvent completes in another 10s. Which doesn't make sense since Async RPC calls are supposed work concurrently.
    //The reason seems be the two ASYNC RPC are called in the same thread and so the second one will be really invoked only when the first ASYNC call returned, may be by design or a bug of RPC.
    //So use different thread to call ASYNC RPC to make them run on server concurrently
    HANDLE hAPCThread = CreateThread(NULL,
        0,
        HelloReplyByAPC,
        NULL,
        0,
        NULL);

    HANDLE hEventThread = CreateThread(NULL,
        0,
        HelloReplyByEvent,
        NULL,
        0,
        NULL);

    WaitForSingleObject(hAPCThread, INFINITE);
    WaitForSingleObject(hEventThread, INFINITE);
    CloseHandle(hAPCThread);
    CloseHandle(hEventThread);

    //HANDLE hThread = CreateThread(NULL,
    //    0,
    //    HelloReply,
    //    NULL,
    //    0,
    //    NULL);

    //WaitForSingleObject(hThread, INFINITE);
    //CloseHandle(hThread);

    status = RpcStringFree(reinterpret_cast<RPC_WSTR *>(&pszStringBinding));
    if (status) exit(status);

    //Shutdown();

    status = RpcBindingFree(&hello_IfHandle);
    if (status) exit(status);
}
