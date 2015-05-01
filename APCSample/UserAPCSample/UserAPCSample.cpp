// UserAPCSample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

VOID CALLBACK APCProc(
    _In_ ULONG_PTR dwParam
    )
{
    printf("%s\r\n",(char*)dwParam);
    Sleep(10000);
}
int _tmain(int argc, _TCHAR* argv[])
{
    HANDLE hThread = GetCurrentThread();
    //Below two APCs are called in the same alertable main thread, so they are called in sequence
    //SleepEx will wait until timeout or APC function call is completed, it is not accurate MSDN said it will return when APC function is called
    QueueUserAPC(APCProc, hThread, (ULONG_PTR)"APC1");
    QueueUserAPC(APCProc, hThread, (ULONG_PTR)"APC2");

    DWORD state = SleepEx(10000000,true);
    printf("SleepEx returns with ");
    if (state == WAIT_IO_COMPLETION)
    {
        printf("WAIT_IO_COMPLETION");
    }
    else
    {
        printf("0x%x", state);
    }
	return 0;
}

