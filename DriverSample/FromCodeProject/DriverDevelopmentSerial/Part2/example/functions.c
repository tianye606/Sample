/**********************************************************************
 * 
 *  Toby Opferman
 *
 *  Driver Example
 *
 *  This example is for educational purposes only.  I license this source
 *  out for use in learning how to write a device driver.
 *
 *     Driver Functionality
 **********************************************************************/

#define _X86_ 

#include <wdm.h>
#include "example.h"
#include <public.h>


/**********************************************************************
 * Internal Functions
 **********************************************************************/
BOOLEAN Example_IsStringTerminated(PCHAR pString, UINT uiLength, UINT *pdwStringLength);
NTSTATUS Example_HandleSampleIoctl_DirectInIo(PIRP Irp, PIO_STACK_LOCATION pIoStackIrp, UINT *pdwDataWritten);
NTSTATUS Example_HandleSampleIoctl_DirectOutIo(PIRP Irp, PIO_STACK_LOCATION pIoStackIrp, UINT *pdwDataWritten);
NTSTATUS Example_HandleSampleIoctl_BufferedIo(PIRP Irp, PIO_STACK_LOCATION pIoStackIrp, UINT *pdwDataWritten);
NTSTATUS Example_HandleSampleIoctl_NeitherIo(PIRP Irp, PIO_STACK_LOCATION pIoStackIrp, UINT *pdwDataWritten);

#pragma alloc_text(PAGE, Example_Create) 
#pragma alloc_text(PAGE, Example_Close) 
#pragma alloc_text(PAGE, Example_IoControl) 
#pragma alloc_text(PAGE, Example_ReadDirectIO)
#pragma alloc_text(PAGE, Example_ReadBufferedIO)
#pragma alloc_text(PAGE, Example_ReadNeither)
#pragma alloc_text(PAGE, Example_WriteDirectIO)
#pragma alloc_text(PAGE, Example_WriteBufferedIO)
#pragma alloc_text(PAGE, Example_WriteNeither)
#pragma alloc_text(PAGE, Example_UnSupportedFunction)
#pragma alloc_text(PAGE, Example_IsStringTerminated)
#pragma alloc_text(PAGE, Example_HandleSampleIoctl_DirectInIo)
#pragma alloc_text(PAGE, Example_HandleSampleIoctl_DirectOutIo)
#pragma alloc_text(PAGE, Example_HandleSampleIoctl_NeitherIo)
#pragma alloc_text(PAGE, Example_HandleSampleIoctl_DirectInIo)




/**********************************************************************
 * 
 *  Example_Create
 *
 *    This is called when an instance of this driver is created (CreateFile)
 *
 **********************************************************************/
NTSTATUS Example_Create(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS NtStatus = STATUS_SUCCESS;
    DbgPrint("Example_Create Called \r\n");

    return NtStatus;
}

/**********************************************************************
 * 
 *  Example_Close
 *
 *    This is called when an instance of this driver is closed (CloseHandle)
 *
 **********************************************************************/
NTSTATUS Example_Close(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS NtStatus = STATUS_SUCCESS;
    DbgPrint("Example_Close Called \r\n");

    return NtStatus;
}



/**********************************************************************
 * 
 *  Example_IoControl
 *
 *    This is called when an IOCTL is issued on the device handle (DeviceIoControl)
 *
 **********************************************************************/
NTSTATUS Example_IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS NtStatus = STATUS_NOT_SUPPORTED;
    PIO_STACK_LOCATION pIoStackIrp = NULL;
    UINT dwDataWritten = 0;

    DbgPrint("Example_IoControl Called \r\n");
    
    /*
     * Each time the IRP is passed down the driver stack a new stack location is added
     * specifying certain parameters for the IRP to the driver.
     */
    pIoStackIrp = IoGetCurrentIrpStackLocation(Irp);    

    if(pIoStackIrp) /* Should Never Be NULL! */
    {
        switch(pIoStackIrp->Parameters.DeviceIoControl.IoControlCode)
        {
            case IOCTL_EXAMPLE_SAMPLE_DIRECT_IN_IO:
                 NtStatus = Example_HandleSampleIoctl_DirectInIo(Irp, pIoStackIrp, &dwDataWritten);
                 break;

            case IOCTL_EXAMPLE_SAMPLE_DIRECT_OUT_IO:
                 NtStatus = Example_HandleSampleIoctl_DirectOutIo(Irp, pIoStackIrp, &dwDataWritten);
                 break;

            case IOCTL_EXAMPLE_SAMPLE_BUFFERED_IO:
                 NtStatus = Example_HandleSampleIoctl_BufferedIo(Irp, pIoStackIrp, &dwDataWritten);
                 break;

            case IOCTL_EXAMPLE_SAMPLE_NEITHER_IO:
                 NtStatus = Example_HandleSampleIoctl_NeitherIo(Irp, pIoStackIrp, &dwDataWritten);
                 break;
        }
    }

    /*
     * This does not always need to be completed in this manner.  The I/O Manager is friendly
     * and in the simple case (as this driver is implemented) the IRP will be completed
     * by IoCompleteRequest() and the Status will be set to the return value.
     *
     * What will not be set however is the "Information" field, it cannot be set to how many bytes
     * were read or written obviously because the I/O Manager does not know, only your device
     * driver does.
     *
     * There are cases where you will need to complete the IRP and set the status however
     * our simple driver does not require that.
     *
     * In the Write operation the "bytes written" is really only used as an informant to
     * the application.  The Read operation is a bit different.  For example, some types of buffering
     * it may not matter if you set the number of bytes read.  For example "Neither" you write
     * directly into the user mode buffer so the user mode gets the data even if you don't
     * tell it the amount.  However if you remember how buffered I/O works?  It makes a copy
     * in memory.  If the I/O manager doesn't know the size then it can't copy it back to the
     * user mode buffer.
     *  
     *
     * IO_NO_INCREMENT - What is this?  If an IRP request is taking a long time you may want to help
     * the scheduler to re-schedule the thread as soon as possible.  For example perhaps it issued
     * a network request and went to sleep.  Then on another thread the network request completes
     * You may want to use one of the pre-defined constants or your own to increment the priority of
     * that thread to be rescheduled being since it hasn't been scheduled in a while.
     *
     */

    Irp->IoStatus.Status = NtStatus;
    Irp->IoStatus.Information = dwDataWritten;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return NtStatus;

}






/**********************************************************************
 * 
 *  Example_WriteDirectIO
 *
 *    This is called when a write is issued on the device handle (WriteFile/WriteFileEx)
 *
 *    This version uses Direct I/O
 *
 **********************************************************************/
NTSTATUS Example_WriteDirectIO(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS NtStatus = STATUS_UNSUCCESSFUL;
    PIO_STACK_LOCATION pIoStackIrp = NULL;
    UINT dwDataWritten = 0;
    PCHAR pWriteDataBuffer;

    DbgPrint("Example_WriteDirectIO Called \r\n");
    
    /*
     * Each time the IRP is passed down the driver stack a new stack location is added
     * specifying certain parameters for the IRP to the driver.
     */
    pIoStackIrp = IoGetCurrentIrpStackLocation(Irp);
    
    if(pIoStackIrp && Irp->MdlAddress)
    {
        pWriteDataBuffer = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
    
        if(pWriteDataBuffer)
        {                             
            /*
             * We need to verify that the string is NULL terminated. Bad things can happen
             * if we access memory not valid while in the Kernel.
             */
           if(Example_IsStringTerminated(pWriteDataBuffer, pIoStackIrp->Parameters.Write.Length, &dwDataWritten))
           {
                DbgPrint(pWriteDataBuffer);
                NtStatus = STATUS_SUCCESS;
           }
        }
    }

    /*
     * This does not always need to be completed in this manner.  The I/O Manager is friendly
     * and in the simple case (as this driver is implemented) the IRP will be completed
     * by IoCompleteRequest() and the Status will be set to the return value.
     *
     * What will not be set however is the "Information" field, it cannot be set to how many bytes
     * were read or written obviously because the I/O Manager does not know, only your device
     * driver does.
     *
     * There are cases where you will need to complete the IRP and set the status however
     * our simple driver does not require that.
     *
     * In the Write operation the "bytes written" is really only used as an informant to
     * the application.  The Read operation is a bit different.  For example, some types of buffering
     * it may not matter if you set the number of bytes read.  For example "Neither" you write
     * directly into the user mode buffer so the user mode gets the data even if you don't
     * tell it the amount.  However if you remember how buffered I/O works?  It makes a copy
     * in memory.  If the I/O manager doesn't know the size then it can't copy it back to the
     * user mode buffer.
     *  
     *
     * IO_NO_INCREMENT - What is this?  If an IRP request is taking a long time you may want to help
     * the scheduler to re-schedule the thread as soon as possible.  For example perhaps it issued
     * a network request and went to sleep.  Then on another thread the network request completes
     * You may want to use one of the pre-defined constants or your own to increment the priority of
     * that thread to be rescheduled being since it hasn't been scheduled in a while.
     *
     */

    Irp->IoStatus.Status = NtStatus;
    Irp->IoStatus.Information = dwDataWritten;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return NtStatus;
}

/**********************************************************************
 * 
 *  Example_WriteBufferedIO
 *
 *    This is called when a write is issued on the device handle (WriteFile/WriteFileEx)
 *
 *    This version uses Buffered I/O
 *
 **********************************************************************/
NTSTATUS Example_WriteBufferedIO(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS NtStatus = STATUS_UNSUCCESSFUL;
    PIO_STACK_LOCATION pIoStackIrp = NULL;
    UINT dwDataWritten = 0;
    PCHAR pWriteDataBuffer;

    DbgPrint("Example_WriteBufferedIO Called \r\n");
    
    /*
     * Each time the IRP is passed down the driver stack a new stack location is added
     * specifying certain parameters for the IRP to the driver.
     */
    pIoStackIrp = IoGetCurrentIrpStackLocation(Irp);
    
    if(pIoStackIrp)
    {
        pWriteDataBuffer = (PCHAR)Irp->AssociatedIrp.SystemBuffer;
    
        if(pWriteDataBuffer)
        {                             
            /*
             * We need to verify that the string is NULL terminated. Bad things can happen
             * if we access memory not valid while in the Kernel.
             */
           if(Example_IsStringTerminated(pWriteDataBuffer, pIoStackIrp->Parameters.Write.Length, &dwDataWritten))
           {
                DbgPrint(pWriteDataBuffer);
                NtStatus = STATUS_SUCCESS;
           }
        }
    }
    
    /*
     * This does not always need to be completed in this manner.  The I/O Manager is friendly
     * and in the simple case (as this driver is implemented) the IRP will be completed
     * by IoCompleteRequest() and the Status will be set to the return value.
     *
     * What will not be set however is the "Information" field, it cannot be set to how many bytes
     * were read or written obviously because the I/O Manager does not know, only your device
     * driver does.
     *
     * There are cases where you will need to complete the IRP and set the status however
     * our simple driver does not require that.
     *
     * In the Write operation the "bytes written" is really only used as an informant to
     * the application.  The Read operation is a bit different.  For example, some types of buffering
     * it may not matter if you set the number of bytes read.  For example "Neither" you write
     * directly into the user mode buffer so the user mode gets the data even if you don't
     * tell it the amount.  However if you remember how buffered I/O works?  It makes a copy
     * in memory.  If the I/O manager doesn't know the size then it can't copy it back to the
     * user mode buffer.
     *  
     *
     * IO_NO_INCREMENT - What is this?  If an IRP request is taking a long time you may want to help
     * the scheduler to re-schedule the thread as soon as possible.  For example perhaps it issued
     * a network request and went to sleep.  Then on another thread the network request completes
     * You may want to use one of the pre-defined constants or your own to increment the priority of
     * that thread to be rescheduled being since it hasn't been scheduled in a while.
     *
     */

    Irp->IoStatus.Status = NtStatus;
    Irp->IoStatus.Information = dwDataWritten;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);


    return NtStatus;
}

/**********************************************************************
 * 
 *  Example_WriteNeither
 *
 *    This is called when a write is issued on the device handle (WriteFile/WriteFileEx)
 *
 *    This version uses Neither buffered or direct I/O.  User mode memory is
 *    read directly.
 *
 **********************************************************************/
NTSTATUS Example_WriteNeither(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS NtStatus = STATUS_UNSUCCESSFUL;
    PIO_STACK_LOCATION pIoStackIrp = NULL;
    PCHAR pWriteDataBuffer;
    UINT dwDataWritten = 0;
    DbgPrint("Example_WriteNeither Called \r\n");
    
    /*
     * Each time the IRP is passed down the driver stack a new stack location is added
     * specifying certain parameters for the IRP to the driver.
     */
    pIoStackIrp = IoGetCurrentIrpStackLocation(Irp);
    
    if(pIoStackIrp)
    {
        /*
         * We need this in an exception handler or else we could trap.
         */
        __try {
        
                if(Irp->UserBuffer)
                {

                    ProbeForRead(Irp->UserBuffer, pIoStackIrp->Parameters.Write.Length, TYPE_ALIGNMENT(char));
                    pWriteDataBuffer = Irp->UserBuffer;
                                             
                    /*
                     * We need to verify that the string is NULL terminated. Bad things can happen
                     * if we access memory not valid while in the Kernel.
                     */
                   if(Example_IsStringTerminated(pWriteDataBuffer, pIoStackIrp->Parameters.Write.Length, &dwDataWritten))
                   {
                        DbgPrint(pWriteDataBuffer);
                        NtStatus = STATUS_SUCCESS;
                   }
                }

        } __except( EXCEPTION_EXECUTE_HANDLER ) {

              NtStatus = GetExceptionCode();     
        }

    }

    /*
     * This does not always need to be completed in this manner.  The I/O Manager is friendly
     * and in the simple case (as this driver is implemented) the IRP will be completed
     * by IoCompleteRequest() and the Status will be set to the return value.
     *
     * What will not be set however is the "Information" field, it cannot be set to how many bytes
     * were read or written obviously because the I/O Manager does not know, only your device
     * driver does.
     *
     * There are cases where you will need to complete the IRP and set the status however
     * our simple driver does not require that.
     *
     * In the Write operation the "bytes written" is really only used as an informant to
     * the application.  The Read operation is a bit different.  For example, some types of buffering
     * it may not matter if you set the number of bytes read.  For example "Neither" you write
     * directly into the user mode buffer so the user mode gets the data even if you don't
     * tell it the amount.  However if you remember how buffered I/O works?  It makes a copy
     * in memory.  If the I/O manager doesn't know the size then it can't copy it back to the
     * user mode buffer.
     *  
     *
     * IO_NO_INCREMENT - What is this?  If an IRP request is taking a long time you may want to help
     * the scheduler to re-schedule the thread as soon as possible.  For example perhaps it issued
     * a network request and went to sleep.  Then on another thread the network request completes
     * You may want to use one of the pre-defined constants or your own to increment the priority of
     * that thread to be rescheduled being since it hasn't been scheduled in a while.
     *
     */

    Irp->IoStatus.Status = NtStatus;
    Irp->IoStatus.Information = dwDataWritten;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return NtStatus;
}


/**********************************************************************
 * 
 *  Example_ReadDirectIO
 *
 *    This is called when a read is issued on the device handle (ReadFile/ReadFileEx)
 *
 *    This version uses Direct I/O
 *
 **********************************************************************/
NTSTATUS Example_ReadDirectIO(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS NtStatus = STATUS_BUFFER_TOO_SMALL;
    PIO_STACK_LOCATION pIoStackIrp = NULL;
    PCHAR pReturnData = "Example_ReadDirectIO - Hello from the Kernel!";
    UINT dwDataSize = sizeof("Example_ReadDirectIO - Hello from the Kernel!");
    UINT dwDataRead = 0;
    PCHAR pReadDataBuffer;

    DbgPrint("Example_ReadDirectIO Called \r\n");
    
    /*
     * Each time the IRP is passed down the driver stack a new stack location is added
     * specifying certain parameters for the IRP to the driver.
     */
    pIoStackIrp = IoGetCurrentIrpStackLocation(Irp);
    
    if(pIoStackIrp && Irp->MdlAddress)
    {
        pReadDataBuffer = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
    
        if(pReadDataBuffer && pIoStackIrp->Parameters.Read.Length >= dwDataSize)
        {                             
            /*
             * We use "RtlCopyMemory" in the kernel instead of memcpy.
             * RtlCopyMemory *IS* memcpy, however it's best to use the
             * wrapper in case this changes in the future.
             */
            RtlCopyMemory(pReadDataBuffer, pReturnData, dwDataSize);
            dwDataRead = dwDataSize;
            NtStatus = STATUS_SUCCESS;
        }
    }
    
    /*
     * This does not always need to be completed in this manner.  The I/O Manager is friendly
     * and in the simple case (as this driver is implemented) the IRP will be completed
     * by IoCompleteRequest() and the Status will be set to the return value.
     *
     * What will not be set however is the "Information" field, it cannot be set to how many bytes
     * were read or written obviously because the I/O Manager does not know, only your device
     * driver does.
     *
     * There are cases where you will need to complete the IRP and set the status however
     * our simple driver does not require that.
     *
     * In the Write operation the "bytes written" is really only used as an informant to
     * the application.  The Read operation is a bit different.  For example, some types of buffering
     * it may not matter if you set the number of bytes read.  For example "Neither" you write
     * directly into the user mode buffer so the user mode gets the data even if you don't
     * tell it the amount.  However if you remember how buffered I/O works?  It makes a copy
     * in memory.  If the I/O manager doesn't know the size then it can't copy it back to the
     * user mode buffer.
     *  
     *
     * IO_NO_INCREMENT - What is this?  If an IRP request is taking a long time you may want to help
     * the scheduler to re-schedule the thread as soon as possible.  For example perhaps it issued
     * a network request and went to sleep.  Then on another thread the network request completes
     * You may want to use one of the pre-defined constants or your own to increment the priority of
     * that thread to be rescheduled being since it hasn't been scheduled in a while.
     *
     */

    Irp->IoStatus.Status = NtStatus;
    Irp->IoStatus.Information = dwDataRead;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return NtStatus;
}

/**********************************************************************
 * 
 *  Example_ReadBufferedIO
 *
 *    This is called when a read is issued on the device handle (ReadFile/ReadFileEx)
 *
 *    This version uses Buffered I/O
 *
 **********************************************************************/
NTSTATUS Example_ReadBufferedIO(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS NtStatus = STATUS_BUFFER_TOO_SMALL;
    PIO_STACK_LOCATION pIoStackIrp = NULL;
    PCHAR pReturnData = "Example_ReadBufferedIO - Hello from the Kernel!";
    UINT dwDataSize = sizeof("Example_ReadBufferedIO - Hello from the Kernel!");
    UINT dwDataRead = 0;
    PCHAR pReadDataBuffer;

    DbgPrint("Example_ReadBufferedIO Called \r\n");
    
    /*
     * Each time the IRP is passed down the driver stack a new stack location is added
     * specifying certain parameters for the IRP to the driver.
     */
    pIoStackIrp = IoGetCurrentIrpStackLocation(Irp);
    
    if(pIoStackIrp)
    {
        pReadDataBuffer = (PCHAR)Irp->AssociatedIrp.SystemBuffer;
    
        if(pReadDataBuffer && pIoStackIrp->Parameters.Read.Length >= dwDataSize)
        {                             
            /*
             * We use "RtlCopyMemory" in the kernel instead of memcpy.
             * RtlCopyMemory *IS* memcpy, however it's best to use the
             * wrapper in case this changes in the future.
             */
            RtlCopyMemory(pReadDataBuffer, pReturnData, dwDataSize);
            dwDataRead = dwDataSize;
            NtStatus = STATUS_SUCCESS;
        }
    }
    
    /*
     * This does not always need to be completed in this manner.  The I/O Manager is friendly
     * and in the simple case (as this driver is implemented) the IRP will be completed
     * by IoCompleteRequest() and the Status will be set to the return value.
     *
     * What will not be set however is the "Information" field, it cannot be set to how many bytes
     * were read or written obviously because the I/O Manager does not know, only your device
     * driver does.
     *
     * There are cases where you will need to complete the IRP and set the status however
     * our simple driver does not require that.
     *
     * In the Write operation the "bytes written" is really only used as an informant to
     * the application.  The Read operation is a bit different.  For example, some types of buffering
     * it may not matter if you set the number of bytes read.  For example "Neither" you write
     * directly into the user mode buffer so the user mode gets the data even if you don't
     * tell it the amount.  However if you remember how buffered I/O works?  It makes a copy
     * in memory.  If the I/O manager doesn't know the size then it can't copy it back to the
     * user mode buffer.
     *  
     *
     * IO_NO_INCREMENT - What is this?  If an IRP request is taking a long time you may want to help
     * the scheduler to re-schedule the thread as soon as possible.  For example perhaps it issued
     * a network request and went to sleep.  Then on another thread the network request completes
     * You may want to use one of the pre-defined constants or your own to increment the priority of
     * that thread to be rescheduled being since it hasn't been scheduled in a while.
     *
     */

    Irp->IoStatus.Status = NtStatus;
    Irp->IoStatus.Information = dwDataRead;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return NtStatus;
}

/**********************************************************************
 * 
 *  Example_ReadNeither
 *
 *    This is called when a Read is issued on the device handle (ReadFile/ReadFileEx)
 *
 *    This version uses Neither buffered or direct I/O.  User mode memory is
 *    written directly.
 *
 **********************************************************************/
NTSTATUS Example_ReadNeither(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS NtStatus = STATUS_BUFFER_TOO_SMALL;
    PIO_STACK_LOCATION pIoStackIrp = NULL;
    PCHAR pReturnData = "Example_ReadNeither - Hello from the Kernel!";
    UINT dwDataSize = sizeof("Example_ReadNeither - Hello from the Kernel!");
    UINT dwDataRead = 0;
    PCHAR pReadDataBuffer;

    DbgPrint("Example_ReadNeither Called \r\n");
    
    /*
     * Each time the IRP is passed down the driver stack a new stack location is added
     * specifying certain parameters for the IRP to the driver.
     */
    pIoStackIrp = IoGetCurrentIrpStackLocation(Irp);
    
    if(pIoStackIrp)
    {
        /*
         * We need this in an exception handler or else we could trap.
         */
        __try {
        
            if(pIoStackIrp->Parameters.Read.Length >= dwDataSize && Irp->UserBuffer)
            {

                ProbeForWrite(Irp->UserBuffer, pIoStackIrp->Parameters.Read.Length, TYPE_ALIGNMENT(char));
                pReadDataBuffer = Irp->UserBuffer;

                /*
                 * We use "RtlCopyMemory" in the kernel instead of memcpy.
                 * RtlCopyMemory *IS* memcpy, however it's best to use the
                 * wrapper in case this changes in the future.
                 */
                RtlCopyMemory(pReadDataBuffer, pReturnData, dwDataSize); 
                dwDataRead = dwDataSize;
                NtStatus = STATUS_SUCCESS;
            }

        } __except( EXCEPTION_EXECUTE_HANDLER ) {

              NtStatus = GetExceptionCode();     
        }
        
    }

    /*
     * This does not always need to be completed in this manner.  The I/O Manager is friendly
     * and in the simple case (as this driver is implemented) the IRP will be completed
     * by IoCompleteRequest() and the Status will be set to the return value.
     *
     * What will not be set however is the "Information" field, it cannot be set to how many bytes
     * were read or written obviously because the I/O Manager does not know, only your device
     * driver does.
     *
     * There are cases where you will need to complete the IRP and set the status however
     * our simple driver does not require that.
     *
     * In the Write operation the "bytes written" is really only used as an informant to
     * the application.  The Read operation is a bit different.  For example, some types of buffering
     * it may not matter if you set the number of bytes read.  For example "Neither" you write
     * directly into the user mode buffer so the user mode gets the data even if you don't
     * tell it the amount.  However if you remember how buffered I/O works?  It makes a copy
     * in memory.  If the I/O manager doesn't know the size then it can't copy it back to the
     * user mode buffer.
     *  
     *
     * IO_NO_INCREMENT - What is this?  If an IRP request is taking a long time you may want to help
     * the scheduler to re-schedule the thread as soon as possible.  For example perhaps it issued
     * a network request and went to sleep.  Then on another thread the network request completes
     * You may want to use one of the pre-defined constants or your own to increment the priority of
     * that thread to be rescheduled being since it hasn't been scheduled in a while.
     *
     */

    Irp->IoStatus.Status = NtStatus;
    Irp->IoStatus.Information = dwDataRead;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return NtStatus;
}

                       

/**********************************************************************
 * 
 *  Example_UnSupportedFunction
 *
 *    This is called when a major function is issued that isn't supported.
 *
 **********************************************************************/
NTSTATUS Example_UnSupportedFunction(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS NtStatus = STATUS_NOT_SUPPORTED;
    DbgPrint("Example_UnSupportedFunction Called \r\n");

    return NtStatus;
}


/**********************************************************************
 * 
 *  Example_IsStringTerminated
 *
 *    Simple function to determine a string is NULL terminated.
 *
 **** We could validate also the characters in the string are printable! ***
 *
 **********************************************************************/
BOOLEAN Example_IsStringTerminated(PCHAR pString, UINT uiLength, UINT *pdwStringLength)
{
    BOOLEAN bStringIsTerminated = FALSE;
    UINT uiIndex = 0;
    
    DbgPrint("Example_IsStringTerminated(0x%0x, %d)\r\n", pString, uiLength);

    *pdwStringLength = 0;

    while(uiIndex < uiLength && bStringIsTerminated == FALSE)
    {
        if(pString[uiIndex] == '\0')
        {
            *pdwStringLength = uiIndex + 1; /* Include the total count we read, includes the NULL */
            bStringIsTerminated = TRUE;
            DbgPrint("  String Is Terminated!\r\n");
        }
        else
        {
           uiIndex++;
        }
    }

    return bStringIsTerminated;
}


/**********************************************************************
 * 
 *  Example_HandleSampleIoctl_DirectInIo
 *
 *    Sample IOCTL TO Handle Direct In I/O
 *
 *
 **********************************************************************/
NTSTATUS Example_HandleSampleIoctl_DirectInIo(PIRP Irp, PIO_STACK_LOCATION pIoStackIrp, UINT *pdwDataWritten)
{
    NTSTATUS NtStatus = STATUS_UNSUCCESSFUL;
    PCHAR pInputBuffer;
    PCHAR pOutputBuffer;
    UINT dwDataRead = 0, dwDataWritten = 0;
    PCHAR pReturnData = "IOCTL - Direct In I/O From Kernel!";
    UINT dwDataSize = sizeof("IOCTL - Direct In I/O From Kernel!");
    DbgPrint("Example_HandleSampleIoctl_DirectInIo Called \r\n");

    /*
     * METHOD_IN_DIRECT
     *
     *    Input Buffer = Irp->AssociatedIrp.SystemBuffer
     *    Ouput Buffer = Irp->MdlAddress  
     *
     *    Input Size   =  Parameters.DeviceIoControl.InputBufferLength
     *    Output Size  =  Parameters.DeviceIoControl.OutputBufferLength
     *
     * What's the difference between METHOD_IN_DIRECT && METHOD_OUT_DIRECT?
     *
     * This function is actually *WRONG*!!!!  We are using the output buffer
     * as an output buffer!  The difference is that METHOD_IN_DIRECT creates
     * an MDL for the outputbuffer with *READ* access so the user mode application
     * can send large amounts of data to the driver for reading.
     *
     * METHOD_OUT_DIRECT creates an MDL for the outputbuffer with *WRITE* access so the user mode
     * application can recieve large amounts of data from the driver!
     *
     * In both cases, the Input buffer is in the same place, the SystemBuffer.  There is a lot
     * of consfusion as people do think that the MdlAddress contains the input buffer and this
     * is not true in either case.   
     */

    pOutputBuffer = NULL;

    if(Irp->MdlAddress)
    {
       pOutputBuffer = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
    }
          
    pInputBuffer = Irp->AssociatedIrp.SystemBuffer;

    if(pInputBuffer && pOutputBuffer)
    {
                                                             
        /*
         * We need to verify that the string is NULL terminated. Bad things can happen
         * if we access memory not valid while in the Kernel.
         */
       if(Example_IsStringTerminated(pInputBuffer, pIoStackIrp->Parameters.DeviceIoControl.InputBufferLength, &dwDataRead))
       {
            DbgPrint("UserModeMessage = '%s'", pInputBuffer);

            DbgPrint("%i >= %i", pIoStackIrp->Parameters.DeviceIoControl.OutputBufferLength, dwDataSize);

            if(pIoStackIrp->Parameters.DeviceIoControl.OutputBufferLength >= dwDataSize)
            {
                /*
                 * We use "RtlCopyMemory" in the kernel instead of memcpy.
                 * RtlCopyMemory *IS* memcpy, however it's best to use the
                 * wrapper in case this changes in the future.
                 */
                RtlCopyMemory(pOutputBuffer, pReturnData, dwDataSize);
                *pdwDataWritten = dwDataSize;
                NtStatus = STATUS_SUCCESS;
            }
            else
            {
                *pdwDataWritten = dwDataSize;
                NtStatus = STATUS_BUFFER_TOO_SMALL;
            }

       }
    }

    return NtStatus;                                      
}   


/**********************************************************************
 * 
 *  Example_IsStringTerminated
 *
 *    Sample IOCTL TO Handle Direct Out I/O
 *
 *
 **********************************************************************/
NTSTATUS Example_HandleSampleIoctl_DirectOutIo(PIRP Irp, PIO_STACK_LOCATION pIoStackIrp, UINT *pdwDataWritten)
{
    NTSTATUS NtStatus = STATUS_UNSUCCESSFUL;
    PCHAR pInputBuffer;
    PCHAR pOutputBuffer;
    UINT dwDataRead = 0, dwDataWritten = 0;
    PCHAR pReturnData = "IOCTL - Direct Out I/O From Kernel!";
    UINT dwDataSize = sizeof("IOCTL - Direct Out I/O From Kernel!");
    DbgPrint("Example_HandleSampleIoctl_DirectOutIo Called \r\n");

    /*
     * METHOD_OUT_DIRECT
     *
     *    Input Buffer = Irp->AssociatedIrp.SystemBuffer
     *    Ouput Buffer = Irp->MdlAddress
     *
     *    Input Size   =  Parameters.DeviceIoControl.InputBufferLength
     *    Output Size  =  Parameters.DeviceIoControl.OutputBufferLength
     *
     * What's the difference between METHOD_IN_DIRECT && METHOD_OUT_DIRECT?
     *
     * The function which we implemented METHOD_IN_DIRECT is actually *WRONG*!!!!  We are using the output buffer
     * as an output buffer!  The difference is that METHOD_IN_DIRECT creates
     * an MDL for the outputbuffer with *READ* access so the user mode application
     * can send large amounts of data to the driver for reading.
     *
     * METHOD_OUT_DIRECT creates an MDL for the outputbuffer with *WRITE* access so the user mode
     * application can recieve large amounts of data from the driver!
     *
     * In both cases, the Input buffer is in the same place, the SystemBuffer.  There is a lot
     * of consfusion as people do think that the MdlAddress contains the input buffer and this
     * is not true in either case.     
     */


    pInputBuffer = Irp->AssociatedIrp.SystemBuffer;
    pOutputBuffer = NULL;

    if(Irp->MdlAddress)
    {
        pOutputBuffer = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
    }

    if(pInputBuffer && pOutputBuffer)
    {
                                                             
        /*
         * We need to verify that the string is NULL terminated. Bad things can happen
         * if we access memory not valid while in the Kernel.
         */
       if(Example_IsStringTerminated(pInputBuffer, pIoStackIrp->Parameters.DeviceIoControl.InputBufferLength, &dwDataRead))
       {
            DbgPrint("UserModeMessage = '%s'", pInputBuffer);
            DbgPrint("%i >= %i", pIoStackIrp->Parameters.DeviceIoControl.OutputBufferLength, dwDataSize);
            if(pIoStackIrp->Parameters.DeviceIoControl.OutputBufferLength >= dwDataSize)
            {
                /*
                 * We use "RtlCopyMemory" in the kernel instead of memcpy.
                 * RtlCopyMemory *IS* memcpy, however it's best to use the
                 * wrapper in case this changes in the future.
                 */
                RtlCopyMemory(pOutputBuffer, pReturnData, dwDataSize);
                *pdwDataWritten = dwDataSize;
                NtStatus = STATUS_SUCCESS;
            }
            else
            {
                *pdwDataWritten = dwDataSize;
                NtStatus = STATUS_BUFFER_TOO_SMALL;
            }

       }
    }

    return NtStatus;
}


/**********************************************************************
 * 
 *  Example_IsStringTerminated
 *
 *    Sample IOCTL TO Handle Buffered I/O
 *
 *
 **********************************************************************/
NTSTATUS Example_HandleSampleIoctl_BufferedIo(PIRP Irp, PIO_STACK_LOCATION pIoStackIrp, UINT *pdwDataWritten)
{
    NTSTATUS NtStatus = STATUS_UNSUCCESSFUL;
    PCHAR pInputBuffer;
    PCHAR pOutputBuffer;
    UINT dwDataRead = 0, dwDataWritten = 0;
    PCHAR pReturnData = "IOCTL - Buffered I/O From Kernel!";
    UINT dwDataSize = sizeof("IOCTL - Buffered I/O From Kernel!");
    DbgPrint("Example_HandleSampleIoctl_BufferedIo Called \r\n");

    /*
     * METHOD_BUFFERED
     *
     *    Input Buffer = Irp->AssociatedIrp.SystemBuffer
     *    Ouput Buffer = Irp->AssociatedIrp.SystemBuffer
     *
     *    Input Size   =  Parameters.DeviceIoControl.InputBufferLength
     *    Output Size  =  Parameters.DeviceIoControl.OutputBufferLength
     *
     *    Since they both use the same location so the "buffer" allocated by the I/O
     *    manager is the size of the larger value (Output vs. Input)
     */


    pInputBuffer = Irp->AssociatedIrp.SystemBuffer;
    pOutputBuffer = Irp->AssociatedIrp.SystemBuffer;

    if(pInputBuffer && pOutputBuffer)
    {
                                                             
        /*
         * We need to verify that the string is NULL terminated. Bad things can happen
         * if we access memory not valid while in the Kernel.
         */
       if(Example_IsStringTerminated(pInputBuffer, pIoStackIrp->Parameters.DeviceIoControl.InputBufferLength, &dwDataRead))
       {
            DbgPrint("UserModeMessage = '%s'", pInputBuffer);
            DbgPrint("%i >= %i", pIoStackIrp->Parameters.DeviceIoControl.OutputBufferLength, dwDataSize);
            if(pIoStackIrp->Parameters.DeviceIoControl.OutputBufferLength >= dwDataSize)
            {
                /*
                 * We use "RtlCopyMemory" in the kernel instead of memcpy.
                 * RtlCopyMemory *IS* memcpy, however it's best to use the
                 * wrapper in case this changes in the future.
                 */
                RtlCopyMemory(pOutputBuffer, pReturnData, dwDataSize);
                *pdwDataWritten = dwDataSize;
                NtStatus = STATUS_SUCCESS;
            }
            else
            {
                *pdwDataWritten = dwDataSize;
                NtStatus = STATUS_BUFFER_TOO_SMALL;
            }

       }
    }

    return NtStatus;
}



/**********************************************************************
 * 
 *  Example_IsStringTerminated
 *
 *    Sample IOCTL TO Handle Neither I/O
 *
 *
 **********************************************************************/
NTSTATUS Example_HandleSampleIoctl_NeitherIo(PIRP Irp, PIO_STACK_LOCATION pIoStackIrp, UINT *pdwDataWritten)
{
    NTSTATUS NtStatus = STATUS_UNSUCCESSFUL;
    PCHAR pInputBuffer;
    PCHAR pOutputBuffer;
    UINT dwDataRead = 0, dwDataWritten = 0;
    PCHAR pReturnData = "IOCTL - Neither I/O From Kernel!";
    UINT dwDataSize = sizeof("IOCTL - Neither I/O From Kernel!");

    DbgPrint("Example_HandleSampleIoctl_NeitherIo Called \r\n");

    /*
     * METHOD_NEITHER
     *
     *    Input Buffer = Parameters.DeviceIoControl.Type3InputBuffer
     *    Ouput Buffer = Irp->UserBuffer
     *
     *    Input Size   =  Parameters.DeviceIoControl.InputBufferLength
     *    Output Size  =  Parameters.DeviceIoControl.OutputBufferLength
     *
     */


    pInputBuffer = pIoStackIrp->Parameters.DeviceIoControl.Type3InputBuffer;
    pOutputBuffer = Irp->UserBuffer;

    if(pInputBuffer && pOutputBuffer)
    {

        /*
         * We need this in an exception handler or else we could trap.
         */
        __try {
        
                ProbeForRead(pInputBuffer, pIoStackIrp->Parameters.DeviceIoControl.InputBufferLength, TYPE_ALIGNMENT(char));
                                                             
                /*
                 * We need to verify that the string is NULL terminated. Bad things can happen
                 * if we access memory not valid while in the Kernel.
                 */
               if(Example_IsStringTerminated(pInputBuffer, pIoStackIrp->Parameters.DeviceIoControl.InputBufferLength, &dwDataRead))
               {
                    DbgPrint("UserModeMessage = '%s'", pInputBuffer);
                    
                    ProbeForWrite(pOutputBuffer, pIoStackIrp->Parameters.DeviceIoControl.OutputBufferLength, TYPE_ALIGNMENT(char));
                    DbgPrint("%i >= %i", pIoStackIrp->Parameters.DeviceIoControl.OutputBufferLength, dwDataSize);
                    if(pIoStackIrp->Parameters.DeviceIoControl.OutputBufferLength >= dwDataSize)
                    {
                        /*
                         * We use "RtlCopyMemory" in the kernel instead of memcpy.
                         * RtlCopyMemory *IS* memcpy, however it's best to use the
                         * wrapper in case this changes in the future.
                         */
                        RtlCopyMemory(pOutputBuffer, pReturnData, dwDataSize);
                        *pdwDataWritten = dwDataSize;
                        NtStatus = STATUS_SUCCESS;
                    }
                    else
                    {
                        *pdwDataWritten = dwDataSize;
                        NtStatus = STATUS_BUFFER_TOO_SMALL;
                    }

               }
                

        } __except( EXCEPTION_EXECUTE_HANDLER ) {

              NtStatus = GetExceptionCode();     
        }

    }


    return NtStatus;
}


