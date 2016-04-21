// stlexample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "OperatorOverload.h"

int _tmain(int argc, _TCHAR* argv[])
{
    average<int> ave(3);
    int a = ave(4, 6);
	return 0;
}

