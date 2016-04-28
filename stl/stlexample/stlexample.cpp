// stlexample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include "OperatorOverload.h"
#include "VirutalInheritance.h"

int _tmain(int argc, _TCHAR* argv[])
{
    try
    {
        //Opertor () overload 
        average<int> ave(3);
        int a = ave(4, 6);

        //virtual inheritance
        Bat bat;
        bat.eat();
        printf("%d\r\n", bat.data);

        Animal &animal = bat;
        animal.eat();

        //exception catch
        throw bad_alloc();
    }
    catch (bad_alloc ex)
    {
        {
            ;
        }
    }
	return 0;
}

