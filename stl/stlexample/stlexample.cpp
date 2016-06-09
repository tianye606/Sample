// stlexample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include "OperatorOverload.h"
#include "VirutalInheritance.h"
#include "MultiInheritance.h"

int _tmain(int argc, _TCHAR* argv[])
{
    try
    {
        //Opertor () overload 
        average<int> ave(3);
        int a = ave(4, 6);
		//type cast operator
		printf("data is %d\r\n", ave);
		//member access operator
		printf("first argument is %d\r\n", ave->m_firstArg);

        //virtual inheritance
        Bat bat;
        bat.eat();
        printf("%d\r\n", bat.data);

        Animal &animal = bat;
        animal.eat();

		//multipule inheritance
		MultiChild* child = new MultiChild();
		//Must cast to the specified type to access data member 
		//since child has two copies of data member from base1 and base2.
		//Virtual inheritance cannot solve the issue, since data member is not from the same class.
		//Vitural inhertance can solve the diamond inheritance ambiguity
		static_cast<MultiBase1*>(child)->data = 10;

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

