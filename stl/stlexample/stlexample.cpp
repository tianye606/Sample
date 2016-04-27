// stlexample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "OperatorOverload.h"

int _tmain(int argc, _TCHAR* argv[])
{
    try
    {
        average<int> ave(3);
        int a = ave(4, 6);
        throw bad_alloc();
    }
    catch (bad_alloc ex)
    {
        {
            throw;
        }
    }
	return 0;
}

