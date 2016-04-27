// CLRExample.cpp : main project file.

#include "stdafx.h"

using namespace System;
using namespace System::Runtime::InteropServices;

int main(array<System::String ^> ^args)
{
    try
    {
        Console::WriteLine(L"Hello World");
        throw gcnew COMException("TEST");
    }
    catch (COMException^ ex)
    {
        throw;
    }
    return 0;
}
