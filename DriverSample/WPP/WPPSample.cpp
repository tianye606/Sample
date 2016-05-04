// WPPSample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "WPPTrace.h"
#include "WPPSample.tmh"

int __cdecl _tmain(int argc, _TCHAR* argv[])
{
    //Need Modify the Visual Studio project to run the WPP preprocessor and build the solution.
    //1         Right click the Applciation project in Solutions Explorer and click Properties.
    //2         In the project property page, click Configuration Properties and click WPP Tracing.
    //3         Under General, set the Run WPP option to Yes and Enable Minimal Rebuild to N0
    //4         Under File Options set the Scan Configuration Data to .\trace.h(where you have declared the TraceEvent)

    //Rember to clean build, or change in WPPTrace.tmh won't be updated even if WPPTrace.h is updated
    WPP_INIT_TRACING(NULL);
    TraceEntry();
    TraceEvents(TRACE_LEVEL_INFORMATION, WPP_BIT_ALL_INFO, _TEXT("WPP Trace Events"));
    TRACE_RETURN(0);
    WPP_CLEANUP();
	return 0;
}

