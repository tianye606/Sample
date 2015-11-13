/**********************************************************************
 * 
 *  Toby Opferman
 *
 *  Driver Example
 *
 *  This example is for educational purposes only.  I license this source
 *  out for use in learning how to write a device driver.
 *
 *     Driver Shared Header File
 **********************************************************************/




#ifndef __EXAMPLE_H__
#define __EXAMPLE_H__

typedef unsigned int UINT;
typedef char * PCHAR;

/* #define __USE_DIRECT__  */
#define __USE_BUFFERED__

NTSTATUS Example_Create(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS Example_Close(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS Example_IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS Example_WriteBufferedIO(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS Example_WriteDirectIO(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS Example_WriteNeither(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS Example_ReadBufferedIO(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS Example_ReadDirectIO(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS Example_ReadNeither(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS Example_UnSupportedFunction(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS Example_IoControlInternal(PDEVICE_OBJECT DeviceObject, PIRP Irp);

#ifdef __USE_DIRECT__
#define IO_TYPE DO_DIRECT_IO
#define USE_WRITE_FUNCTION  Example_WriteDirectIO
#define USE_READ_FUNCTION   Example_ReadDirectIO
#endif
 
#ifdef __USE_BUFFERED__
#define IO_TYPE DO_BUFFERED_IO
#define USE_WRITE_FUNCTION  Example_WriteBufferedIO
#define USE_READ_FUNCTION   Example_ReadBufferedIO
#endif

#ifndef IO_TYPE
#define IO_TYPE 0
#define USE_WRITE_FUNCTION  Example_WriteNeither
#define USE_READ_FUNCTION   Example_ReadNeither
#endif


#define EXAMPLE_POOL_TAG ((ULONG)' pxE')
/*
 * This is a list of connected handles
 */
typedef struct _EXAMPLE_LIST
{
    struct _EXAMPLE_LIST *pNext;
    UNICODE_STRING usPipeName;
    WCHAR szwUnicodeString[256];
    UINT uiRefCount;
    UINT uiStartIndex;
    UINT uiStopIndex;
    KMUTEX kInstanceBufferMutex;
    char pCircularBuffer[2000];

} EXAMPLE_LIST, *PEXAMPLE_LIST;


/*
 * Context that is user-defined, the developer
 * can create any type of device structure that they
 * need.  This structure can be uniquely defined for each created 
 * device object.
 *
 */

typedef struct _EXAMPLE_DEVICE_CONTEXT 
{
    PEXAMPLE_LIST pExampleList;
    KMUTEX kListMutex;

} EXAMPLE_DEVICE_CONTEXT, *PEXAMPLE_DEVICE_CONTEXT;

#endif






