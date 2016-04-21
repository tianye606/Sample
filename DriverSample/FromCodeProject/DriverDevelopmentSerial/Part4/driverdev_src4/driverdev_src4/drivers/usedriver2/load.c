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

    _stprintf_s(szTemp, 256, _T("\\\\.\\Example\\%s"), argv[1]);
    
    hFile = CreateFile(szTemp, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    if(hFile != INVALID_HANDLE_VALUE)
    {

        while(TRUE)
        {
        
            printf("Enter a string and press enter, 'Q' to quit>\n");
            _getts_s(szTemp, 256);
            if (szTemp[0] == 'q' || szTemp[0] == 'Q')
            {
                break;
            }
            dwReturn = 0;
            WriteFile(hFile, szTemp, ((DWORD)_tcslen(szTemp) + 1)*sizeof(TCHAR), &dwReturn, NULL);
            
            printf("%d bytes written\n", dwReturn);
        }
        
        CloseHandle(hFile);
    }
    
    return 0;
}


