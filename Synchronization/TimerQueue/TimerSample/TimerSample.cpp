// TimerSample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#define WLAN_SCAN_TIMER_DELAY           500 // millisecond, query scan result after this short delay

void NTAPI s_ScanTimerCallback(_In_ PVOID lpParameter, _In_ BOOLEAN /*TimerOrWaitFired*/)
{
    printf("s_ScanTimerCallback\r\n");
}

DWORD WINAPI DeleteTimer(_In_ LPVOID lpParameter)
{
    DeleteTimerQueueTimer(nullptr, lpParameter, INVALID_HANDLE_VALUE);
    return ERROR_SUCCESS;
}

int _tmain(int argc, _TCHAR* argv[])
{
    HRESULT hr = S_OK;
    HANDLE _hScanTimer;

    if (!CreateTimerQueueTimer(
        &_hScanTimer,
        nullptr, // Use default timer queue
        &s_ScanTimerCallback,
        NULL, // Context
        WLAN_SCAN_TIMER_DELAY, // DueTime
        WLAN_SCAN_TIMER_DELAY, // Period
        WT_EXECUTEDEFAULT // Flags
        ))
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        printf("CreateTimerQueueTimer failed with result %!HRESULT!\r\n", hr);
    }
    printf("Press any key to exit...\r\n");
    getchar();

    HANDLE hAPCThread = CreateThread(NULL,
        0,
        DeleteTimer,
        _hScanTimer,
        0,
        NULL);

    if (!DeleteTimerQueueTimer(nullptr, _hScanTimer, INVALID_HANDLE_VALUE))
    {
        printf("DeleteTimerQueueTimer failed with result %!HRESULT!", HRESULT_FROM_WIN32(GetLastError()));
    }
    WaitForSingleObject(hAPCThread, INFINITE);
	return 0;
}

