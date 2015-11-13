/**********************************************************************
 * 
 *  Toby Opferman
 *
 *  Example Communicating with a driver
 *
 *  This example is for educational purposes only.  I license this source
 *  out for use in learning how to write a device driver.
 *
 *    
 **********************************************************************/


#include <windows.h>
#include <WINIOCTL.H>
#include <stdio.h>
#include <public.h>

/*********************************************************
 *   Main Function Entry
 *
 *********************************************************/
int _cdecl main(void)
{
    HANDLE hFile;
    DWORD dwReturn;
    char szTemp[256] = {0};
    
    hFile = CreateFile("\\\\.\\Example", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    if(hFile != INVALID_HANDLE_VALUE)
    {
        WriteFile(hFile, "** Hello from user mode!", sizeof("** Hello from user mode!"), &dwReturn, NULL); 
        ReadFile(hFile, &szTemp, 256, &dwReturn, NULL);
        printf(szTemp);
        printf("\n");

        ZeroMemory(szTemp, sizeof(szTemp));
        DeviceIoControl(hFile, IOCTL_EXAMPLE_SAMPLE_DIRECT_IN_IO, "** Hello from User Mode Direct IN I/O", sizeof("** Hello from User Mode Direct IN I/O"), szTemp, sizeof(szTemp), &dwReturn, NULL);
        printf(szTemp);
        printf("\n");

        ZeroMemory(szTemp, sizeof(szTemp));
        DeviceIoControl(hFile, IOCTL_EXAMPLE_SAMPLE_DIRECT_OUT_IO, "** Hello from User Mode Direct OUT I/O", sizeof("** Hello from User Mode Direct OUT I/O"), szTemp, sizeof(szTemp), &dwReturn, NULL);
        printf(szTemp);
        printf("\n");

        ZeroMemory(szTemp, sizeof(szTemp));
        DeviceIoControl(hFile, IOCTL_EXAMPLE_SAMPLE_BUFFERED_IO, "** Hello from User Mode Buffered I/O", sizeof("** Hello from User Mode Buffered I/O"), szTemp, sizeof(szTemp), &dwReturn, NULL);
        printf(szTemp);
        printf("\n");

        ZeroMemory(szTemp, sizeof(szTemp));
        DeviceIoControl(hFile, IOCTL_EXAMPLE_SAMPLE_NEITHER_IO, "** Hello from User Mode Neither I/O", sizeof("** Hello from User Mode Neither I/O"), szTemp, sizeof(szTemp), &dwReturn, NULL);
        printf(szTemp);
        printf("\n");

        CloseHandle(hFile);
    }
    
    return 0;
}


