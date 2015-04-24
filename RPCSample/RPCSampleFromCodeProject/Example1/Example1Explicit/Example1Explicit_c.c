

/* this ALWAYS GENERATED file contains the RPC client stubs */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Thu Apr 23 17:13:33 2015
 */
/* Compiler settings for Example1Explicit.idl:
    Oicf, W4, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0603 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#if !defined(_M_IA64) && !defined(_M_AMD64) && !defined(_ARM_)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extern to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma warning( disable: 4024 )  /* array to pointer mapping*/
#pragma warning( disable: 4100 ) /* unreferenced arguments in x86 call */

#pragma optimize("", off ) 

#include <string.h>

#include "Example1Explicit.h"

#define TYPE_FORMAT_STRING_SIZE   7                                 
#define PROC_FORMAT_STRING_SIZE   35                                
#define EXPR_FORMAT_STRING_SIZE   1                                 
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   0            

typedef struct _Example1Explicit_MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } Example1Explicit_MIDL_TYPE_FORMAT_STRING;

typedef struct _Example1Explicit_MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } Example1Explicit_MIDL_PROC_FORMAT_STRING;

typedef struct _Example1Explicit_MIDL_EXPR_FORMAT_STRING
    {
    long          Pad;
    unsigned char  Format[ EXPR_FORMAT_STRING_SIZE ];
    } Example1Explicit_MIDL_EXPR_FORMAT_STRING;


static const RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};


extern const Example1Explicit_MIDL_TYPE_FORMAT_STRING Example1Explicit__MIDL_TypeFormatString;
extern const Example1Explicit_MIDL_PROC_FORMAT_STRING Example1Explicit__MIDL_ProcFormatString;
extern const Example1Explicit_MIDL_EXPR_FORMAT_STRING Example1Explicit__MIDL_ExprFormatString;

#define GENERIC_BINDING_TABLE_SIZE   0            


/* Standard interface: Example1Explicit, ver. 1.0,
   GUID={0x00000002,0xEAF3,0x4A7A,{0xA0,0xF2,0xBC,0xE4,0xC3,0x0D,0xA7,0x7E}} */



static const RPC_CLIENT_INTERFACE Example1Explicit___RpcClientInterface =
    {
    sizeof(RPC_CLIENT_INTERFACE),
    {{0x00000002,0xEAF3,0x4A7A,{0xA0,0xF2,0xBC,0xE4,0xC3,0x0D,0xA7,0x7E}},{1,0}},
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    0,
    0,
    0,
    0,
    0x00000000
    };
RPC_IF_HANDLE Example1Explicit_v1_0_c_ifspec = (RPC_IF_HANDLE)& Example1Explicit___RpcClientInterface;

extern const MIDL_STUB_DESC Example1Explicit_StubDesc;

static RPC_BINDING_HANDLE Example1Explicit__MIDL_AutoBindHandle;


void Output( 
    /* [in] */ handle_t hBinding,
    /* [string][in] */ const char *szOutput)
{

    NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&Example1Explicit_StubDesc,
                  (PFORMAT_STRING) &Example1Explicit__MIDL_ProcFormatString.Format[0],
                  ( unsigned char * )&hBinding);
    
}


#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT50_OR_LATER)
#error You need Windows 2000 or later to run this stub because it uses these features:
#error   /robust command line switch.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will fail with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const Example1Explicit_MIDL_PROC_FORMAT_STRING Example1Explicit__MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure Output */

			0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x0 ),	/* 0 */
/*  8 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 10 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 12 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 14 */	NdrFcShort( 0x0 ),	/* 0 */
/* 16 */	NdrFcShort( 0x0 ),	/* 0 */
/* 18 */	0x42,		/* Oi2 Flags:  clt must size, has ext, */
			0x1,		/* 1 */
/* 20 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */
/* 24 */	NdrFcShort( 0x0 ),	/* 0 */
/* 26 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hBinding */

/* 28 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 30 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 32 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

			0x0
        }
    };

static const Example1Explicit_MIDL_TYPE_FORMAT_STRING Example1Explicit__MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/*  4 */	
			0x22,		/* FC_C_CSTRING */
			0x5c,		/* FC_PAD */

			0x0
        }
    };

static const unsigned short Example1Explicit_FormatStringOffsetTable[] =
    {
    0
    };


static const MIDL_STUB_DESC Example1Explicit_StubDesc = 
    {
    (void *)& Example1Explicit___RpcClientInterface,
    MIDL_user_allocate,
    MIDL_user_free,
    &Example1Explicit__MIDL_AutoBindHandle,
    0,
    0,
    0,
    0,
    Example1Explicit__MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x50002, /* Ndr library version */
    0,
    0x800025b, /* MIDL Version 8.0.603 */
    0,
    0,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0
    };
#pragma optimize("", on )
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* !defined(_M_IA64) && !defined(_M_AMD64) && !defined(_ARM_) */

