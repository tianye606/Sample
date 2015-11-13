/**********************************************************************
 * 
 *  Toby Opferman
 *
 *  Driver Example
 *
 *  This example is for educational purposes only.  I license this source
 *  out for use in learning how to write a device driver.
 *
 *     Driver Entry Point
 **********************************************************************/
#define _X86_


#include <wdm.h>
#include "examplefilter.h"

 
    
VOID ExampleFilter_Unload(PDRIVER_OBJECT  DriverObject);    
NTSTATUS DriverEntry(PDRIVER_OBJECT  pDriverObject, PUNICODE_STRING  pRegistryPath); 

/* 
 * These compiler directives tell the Operating System how to load the
 * driver into memory. The "INIT" section is discardable as you only
 * need the driver entry upon initialization, then it can be discarded.
 *
 */
#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, ExampleFilter_Unload)


/**********************************************************************
 * 
 *  DriverEntry
 *
 *    This is the default entry point for drivers.  The parameters
 *    are a driver object and the registry path.
 *
 **********************************************************************/
NTSTATUS DriverEntry(PDRIVER_OBJECT  pDriverObject, PUNICODE_STRING  pRegistryPath)
{
    NTSTATUS NtStatus = STATUS_SUCCESS;
    UINT uiIndex = 0;
    PDEVICE_OBJECT pDeviceObject = NULL, pFilteredDevice = NULL;
    UNICODE_STRING usDeviceToFilter;
    PEXAMPLE_FILTER_EXTENSION pExampleFilterDeviceContext;

    DbgPrint("DriverEntry Called \r\n");

    NtStatus = IoCreateDevice(pDriverObject, sizeof(EXAMPLE_FILTER_EXTENSION), NULL, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDeviceObject);

    if(NtStatus == STATUS_SUCCESS)
    {

        /*
         * The "MajorFunction" is a list of function pointers for entry points into the driver.
         * You can set them all to point to 1 function, then have a switch statement for all
         * IRP_MJ_*** functions or you can set specific function pointers for each entry
         * into the driver.
         *
         */
        for(uiIndex = 0; uiIndex < IRP_MJ_MAXIMUM_FUNCTION; uiIndex++)
             pDriverObject->MajorFunction[uiIndex] = ExampleFilter_UnSupportedFunction;
    
        pDriverObject->MajorFunction[IRP_MJ_CLOSE]             = ExampleFilter_Close;
        pDriverObject->MajorFunction[IRP_MJ_CREATE]            = ExampleFilter_Create;
        pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL]    = ExampleFilter_IoControl;
        pDriverObject->MajorFunction[IRP_MJ_INTERNAL_DEVICE_CONTROL] = ExampleFilter_IoControlInternal;
        pDriverObject->MajorFunction[IRP_MJ_READ]              = ExampleFilter_Read;
        pDriverObject->MajorFunction[IRP_MJ_WRITE]             = ExampleFilter_Write;
    
        /* 
         * Required to unload the driver dynamically.  If this function is missing
         * the driver cannot be dynamically unloaded.
         */
        pDriverObject->DriverUnload =  ExampleFilter_Unload;

        /*
         *  We want to save the device we attached to on the lower end so we can use it in
         *  subsequent calls and be able to send IRP's down properly.
         */

        pExampleFilterDeviceContext = (PEXAMPLE_FILTER_EXTENSION)pDeviceObject->DeviceExtension;

        /*
         * We want to attach this device above our other device.  So this driver will actually
         * sit on top of our other driver.  We will be able to filter all calls to the device.
         */

        RtlInitUnicodeString(&usDeviceToFilter, L"\\Device\\Example");
        NtStatus = IoAttachDevice(pDeviceObject, &usDeviceToFilter, &pExampleFilterDeviceContext->pNextDeviceInChain);
        
        /*
         * WARNING!
         * 
         *  The IoAttachDevice() API opens the DeviceObject for us then calls IoAttachDeviceToStack() then closes
         * the object.  That means we will get the CLEAN UP and CLOSE IRP Major functions before IoAttachDevice
         * returns!  We need to make sure the function pointer we pass in can be accessed by those functions in
         * some system wide memory space.  The DeviceExtension is a great place to put this and it must
         * be passed into IoAttachDevice!!!!!!!!!
         */
        

        if(!NT_SUCCESS(NtStatus))
        {
           IoDeleteDevice(pDeviceObject);
        }
        else
        {
            pFilteredDevice = pExampleFilterDeviceContext->pNextDeviceInChain;


            /*
             * Our device driver should ensure that it is using the same type of I/O as the
             * device it is attempting to filter.  For example, buffered, direct or neither should
             * be the same.
             */
            pDeviceObject->Flags |= pFilteredDevice->Flags & (DO_BUFFERED_IO | DO_DIRECT_IO);
            pDeviceObject->DeviceType = pFilteredDevice->DeviceType;
            pDeviceObject->Characteristics = pFilteredDevice->Characteristics;
            pDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
        }
    }

                               
    return NtStatus;
}


/**********************************************************************
 * 
 *  ExampleFilter_Unload
 *
 *    This is an optional unload function which is called when the
 *    driver is unloaded.
 *
 **********************************************************************/
VOID ExampleFilter_Unload(PDRIVER_OBJECT  DriverObject)
{    
    
    PEXAMPLE_FILTER_EXTENSION pExampleFilterDeviceContext = (PEXAMPLE_FILTER_EXTENSION)DriverObject->DeviceObject->DeviceExtension;
    DbgPrint("ExampleFilter_Unload Called \r\n");
    /*
     * We need to detach the device from the device stack before
     * we delete it.
     *
     *  FYI - We aren't doing any error checking here to determine
     *        if anything succeeded!
     */
    IoDetachDevice(pExampleFilterDeviceContext->pNextDeviceInChain);
    IoDeleteDevice(DriverObject->DeviceObject);
}


