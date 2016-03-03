

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Wed Mar 02 15:32:51 2016
 */
/* Compiler settings for ILogger.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0603 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __ILogger_h_h__
#define __ILogger_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ILogger_FWD_DEFINED__
#define __ILogger_FWD_DEFINED__
typedef interface ILogger ILogger;

#endif 	/* __ILogger_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __ILogger_INTERFACE_DEFINED__
#define __ILogger_INTERFACE_DEFINED__

/* interface ILogger */
/* [object][uuid] */ 


EXTERN_C const IID IID_ILogger;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("AFDB9683-F18A-4B85-90D1-B6158DAFA46C")
    ILogger : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Log( 
            /* [in] */ LPCWSTR text) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ILoggerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ILogger * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ILogger * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ILogger * This);
        
        HRESULT ( STDMETHODCALLTYPE *Log )( 
            ILogger * This,
            /* [in] */ LPCWSTR text);
        
        END_INTERFACE
    } ILoggerVtbl;

    interface ILogger
    {
        CONST_VTBL struct ILoggerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILogger_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ILogger_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ILogger_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ILogger_Log(This,text)	\
    ( (This)->lpVtbl -> Log(This,text) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ILogger_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


