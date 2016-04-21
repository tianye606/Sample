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
#include <TCHAR.h>
/*********************************************************
 *   Main Function Entry
 *
 *********************************************************/
int _cdecl main(int argc, char **argv)
{
    HANDLE hFile;
    DWORD dwReturn;
    TCHAR szTemp[256] = {0};

    //DOSName works fine
    //TODO::Need understand why \Device\Example is not working
    //End with \ will return ERROR_PATH_NOT_FOUND (0x3)
    //End without \ will return ERROR_WRONG_TARGET_NAME (0x574)
    //Seems no way to make \Device\Example works
    _stprintf_s(szTemp,256, TEXT("\\\\.\\ExampleFilter\\%s"), argv[1]);
    //_stprintf_s(szTemp, 256, TEXT("\\\\.\\Example"));
    
    hFile = CreateFile(szTemp, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    if(hFile != INVALID_HANDLE_VALUE)
    {

        while(szTemp[0] != 'q' && szTemp[0] != 'Q')
        {
        
            printf("Press enter to get a string from the driver or 'Q' to quit\n");
            _getts_s(szTemp,256);

            if(szTemp[0] != 'q' &&  szTemp[0] != 'Q')
            {
               memset(szTemp, 0, sizeof(szTemp));
               dwReturn = 0;
               ReadFile(hFile, szTemp, sizeof(szTemp), &dwReturn, NULL); 
               printf("%d bytes read\n", dwReturn);
               _tprintf(TEXT("%s\n"), szTemp);
            }
        }
        
        CloseHandle(hFile);
    }
    else
    {
        printf("CreateFile failed %0X\r\n", GetLastError());
    }
    return 0;
}


