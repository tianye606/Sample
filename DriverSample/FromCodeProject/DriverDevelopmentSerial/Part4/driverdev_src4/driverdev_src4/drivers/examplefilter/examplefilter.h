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




#ifndef __EXAMPLE_FILTER_H__
#define __EXAMPLE_FILTER_H__

typedef unsigned int UINT;
typedef char * PCHAR;

/* #define __USE_DIRECT__ */
#define __USE_BUFFERED__

NTSTATUS ExampleFilter_Create(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS ExampleFilter_Close(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS ExampleFilter_IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS ExampleFilter_Write(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS ExampleFilter_Read(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS ExampleFilter_UnSupportedFunction(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS ExampleFilter_IoControlInternal(PDEVICE_OBJECT DeviceObject, PIRP Irp);

typedef struct _EXAMPLE_FILTER_EXTENSION
{
    PDEVICE_OBJECT pNextDeviceInChain;

} EXAMPLE_FILTER_EXTENSION, *PEXAMPLE_FILTER_EXTENSION;


#define EXAMPLE_FILTER_POOL_TAG ((ULONG)'pxEF')


#endif






