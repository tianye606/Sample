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


#define MIN(a,b) ((a) < (b) ? (a) : (b))

/**********************************************************************
 * Internal Functions
 **********************************************************************/
BOOLEAN Example_WriteData(PEXAMPLE_LIST pExampleList, PCHAR pData, UINT uiLength, UINT *pdwStringLength);
BOOLEAN Example_ReadData(PEXAMPLE_LIST pExampleList, PCHAR pData, UINT uiLength, UINT *pdwStringLength);
BOOLEAN Example_IsStringTerminated(PCHAR pString, UINT uiLength, UINT *pdwStringLength);
NTSTATUS Example_HandleSampleIoctl_DirectInIo(PIRP Irp, PIO_STACK_LOCATION pIoStackIrp, UINT *pdwDataWritten);
NTSTATUS Example_HandleSampleIoctl_DirectOutIo(PIRP Irp, PIO_STACK_LOCATION pIoStackIrp, UINT *pdwDataWritten);
NTSTATUS Example_HandleSampleIoctl_BufferedIo(PIRP Irp, PIO_STACK_LOCATION pIoStackIrp, UINT *pdwDataWritten);
NTSTATUS Example_HandleSampleIoctl_NeitherIo(PIRP Irp, PIO_STACK_LOCATION pIoStackIrp, UINT *pdwDataWritten);
NTSTATUS Example_CreatePipeContext(PEXAMPLE_DEVICE_CONTEXT pExampleDeviceContext, PFILE_OBJECT pFileObject);
NTSTATUS Example_ReleasePipeContext(PEXAMPLE_DEVICE_CONTEXT pExampleDeviceContext, PFILE_OBJECT pFileObject);

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
#pragma alloc_text(PAGE, Example_WriteData)
#pragma alloc_text(PAGE, Example_ReadData)
#pragma alloc_text(PAGE, Example_HandleSampleIoctl_DirectInIo)
#pragma alloc_text(PAGE, Example_HandleSampleIoctl_DirectOutIo)
#pragma alloc_text(PAGE, Example_HandleSampleIoctl_NeitherIo)
#pragma alloc_text(PAGE, Example_HandleSampleIoctl_DirectInIo)
#pragma alloc_text(PAGE, Example_CreatePipeContext)
#pragma alloc_text(PAGE, Example_ReleasePipeContext)
#pragma alloc_text(PAGE, Example_IsStringTerminated)
                                

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
    PIO_STACK_LOCATION pIoStackIrp = NULL;
    
    /*
     * Each time the IRP is passed down the driver stack a new stack location is added
     * specifying certain parameters for the IRP to the driver.
     */
    pIoStackIrp = IoGetCurrentIrpStackLocation(Irp);    


    DbgPrint("Example_Create Called \r\n");

    NtStatus = Example_CreatePipeContext((PEXAMPLE_DEVICE_CONTEXT)DeviceObject->DeviceExtension, pIoStackIrp->FileObject);

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
    

    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return NtStatus;
}

/**********************************************************************
 * 
 *  Example_CreatePipeContext
 *
 *    This is called to create or find a pipe context
 *
 **********************************************************************/
NTSTATUS Example_CreatePipeContext(PEXAMPLE_DEVICE_CONTEXT pExampleDeviceContext, PFILE_OBJECT pFileObject)
{
    NTSTATUS NtStatus = STATUS_UNSUCCESSFUL;
    PEXAMPLE_LIST pExampleList = NULL;
    BOOLEAN bNeedsToCreate = FALSE;

    NtStatus = KeWaitForMutexObject(&pExampleDeviceContext->kListMutex, Executive, KernelMode, FALSE, NULL);

    if(NT_SUCCESS(NtStatus))
    {
         pExampleList = pExampleDeviceContext->pExampleList;
         bNeedsToCreate = TRUE;

         if(pExampleList)
         {
              do
              {   /*
                   * We want to use the unicode string that was used to open the driver to
                   * identify "pipe contexts" and match up applications that open the same name
                   * we do this by keeping a global list of all open instances in the device
                   * extension context.
                   * We then use reference counting so we only remove an instance from the list
                   * after all instances have been deleted.  We also put this in the FsContext
                   * of the IRP so all IRP's will be returned with this so we can easily use
                   * the context without searching for it.
                   */
                  if(RtlCompareUnicodeString(&pExampleList->usPipeName, &pFileObject->FileName, TRUE) == 0)
                  {
                      bNeedsToCreate = FALSE;
                      pExampleList->uiRefCount++;
                      pFileObject->FsContext = (PVOID)pExampleList;

                      NtStatus = STATUS_SUCCESS;
                  }
                  else
                  {
                      pExampleList = pExampleList->pNext;
                  }

              } while(pExampleList && bNeedsToCreate) ;
         }

         if(bNeedsToCreate)
         {
             pExampleList = (PEXAMPLE_LIST)ExAllocatePoolWithTag(NonPagedPool, sizeof(EXAMPLE_LIST), EXAMPLE_POOL_TAG);

             if(pExampleList)
             {
                 pExampleList->pNext = pExampleDeviceContext->pExampleList;
                 pExampleDeviceContext->pExampleList = pExampleList;

                 pExampleList->uiRefCount = 1;
                 pExampleList->usPipeName.Length = 0;
                 pExampleList->usPipeName.MaximumLength = sizeof(pExampleList->szwUnicodeString);
                 pExampleList->usPipeName.Buffer = pExampleList->szwUnicodeString;
                 pExampleList->uiStartIndex = 0;
                 pExampleList->uiStopIndex = 0;

                 KeInitializeMutex(&pExampleList->kInstanceBufferMutex, 0);

                 RtlCopyUnicodeString(&pExampleList->usPipeName, &pFileObject->FileName);
                 
                 pFileObject->FsContext = (PVOID)pExampleList;

                 NtStatus = STATUS_SUCCESS;
             }
             else
             {
                 NtStatus = STATUS_INSUFFICIENT_RESOURCES;
             }
         }

         KeReleaseMutex(&pExampleDeviceContext->kListMutex, FALSE);
    }
   
    
    return NtStatus;
}

/**********************************************************************
 * 
 *  Example_ReleasePipeContext
 *
 *    This is called to close a pipe context
 *
 **********************************************************************/
NTSTATUS Example_ReleasePipeContext(PEXAMPLE_DEVICE_CONTEXT pExampleDeviceContext, PFILE_OBJECT pFileObject)
{
    NTSTATUS NtStatus = STATUS_UNSUCCESSFUL;
    PEXAMPLE_LIST pExampleList = NULL, pExampleListFromIrp = (PEXAMPLE_LIST)pFileObject->FsContext;
    BOOLEAN bNotFound = TRUE;

    NtStatus = KeWaitForMutexObject(&pExampleDeviceContext->kListMutex, Executive, KernelMode, FALSE, NULL);

    if(NT_SUCCESS(NtStatus))
    {
         pExampleList = pExampleDeviceContext->pExampleList;

         if(pExampleListFromIrp)
         {
              if(pExampleList == pExampleListFromIrp)
              {
                 bNotFound = FALSE;
                 pExampleListFromIrp->uiRefCount--;
                          
                 if(pExampleListFromIrp->uiRefCount == 0)
                 {
                    pExampleDeviceContext->pExampleList = pExampleList->pNext;
                    ExFreePool(pExampleListFromIrp);

                 }

              }
              else
              {

                  do
                  {
                      if(pExampleListFromIrp == pExampleList->pNext)
                      {
                          bNotFound = FALSE;
                          pExampleListFromIrp->uiRefCount--;
                          
                          if(pExampleListFromIrp->uiRefCount == 0)
                          {
                             pExampleList->pNext = pExampleListFromIrp->pNext;
                             ExFreePool(pExampleListFromIrp);
                          }
    
                          NtStatus = STATUS_SUCCESS;
                      }
                      else
                      {
                          pExampleList = pExampleList->pNext;
                      }
    
                  } while(pExampleList && bNotFound == TRUE) ;
              }
         }

         if(bNotFound)
         {
             NtStatus = STATUS_UNSUCCESSFUL; /* Should Never Reach Here!!!!! */
         }



         KeReleaseMutex(&pExampleDeviceContext->kListMutex, FALSE);
    }
   
    
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
    
    PIO_STACK_LOCATION pIoStackIrp = NULL;

    /*
     * Each time the IRP is passed down the driver stack a new stack location is added
     * specifying certain parameters for the IRP to the driver.
     */
    pIoStackIrp = IoGetCurrentIrpStackLocation(Irp);    


    DbgPrint("Example_Close Called \r\n");

    NtStatus = Example_ReleasePipeContext((PEXAMPLE_DEVICE_CONTEXT)DeviceObject->DeviceExtension, pIoStackIrp->FileObject);


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
    
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
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
           if(Example_WriteData((PEXAMPLE_LIST)pIoStackIrp->FileObject->FsContext, pWriteDataBuffer, pIoStackIrp->Parameters.Write.Length, &dwDataWritten))
           {
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
           if(Example_WriteData((PEXAMPLE_LIST)pIoStackIrp->FileObject->FsContext, pWriteDataBuffer, pIoStackIrp->Parameters.Write.Length, &dwDataWritten))
           {
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
                                             
                    if(Example_WriteData((PEXAMPLE_LIST)pIoStackIrp->FileObject->FsContext, pWriteDataBuffer, pIoStackIrp->Parameters.Write.Length, &dwDataWritten))
                    {
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
    
        if(pReadDataBuffer && pIoStackIrp->Parameters.Read.Length)
        {                             

            if(Example_ReadData((PEXAMPLE_LIST)pIoStackIrp->FileObject->FsContext, pReadDataBuffer, pIoStackIrp->Parameters.Read.Length, &dwDataRead))
            {
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
    
        if(pReadDataBuffer && pIoStackIrp->Parameters.Read.Length > 0)
        {                             
            if(Example_ReadData((PEXAMPLE_LIST)pIoStackIrp->FileObject->FsContext, pReadDataBuffer, pIoStackIrp->Parameters.Read.Length, &dwDataRead))
            {
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
        
            if(pIoStackIrp->Parameters.Read.Length > 0 && Irp->UserBuffer)
            {

                ProbeForWrite(Irp->UserBuffer, pIoStackIrp->Parameters.Read.Length, TYPE_ALIGNMENT(char));
                pReadDataBuffer = Irp->UserBuffer;

                if(Example_ReadData((PEXAMPLE_LIST)pIoStackIrp->FileObject->FsContext, pReadDataBuffer, pIoStackIrp->Parameters.Read.Length, &dwDataRead))
                {
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
 *  Example_WriteData
 *
 *    This is called to write data to the circular buffer.
 *    This function does not block and will not overwrite data, so
 *    excess data will simply fail this call.  If any data at all 
 *    is written then we will return success with the number of bytes
 *    written.
 *
 **********************************************************************/
BOOLEAN Example_WriteData(PEXAMPLE_LIST pExampleList, PCHAR pData, UINT uiLength, UINT *pdwStringLength)
{
    BOOLEAN bDataWritten = FALSE;
    NTSTATUS NtStatus;

    *pdwStringLength = 0;

    /*
     * We want to acquire the instance mutex so we can manipulate the buffer indepdent of other
     * threads (in our process and other processes).
     * 
     * We want to wait so that we cannot be interrupted and we will not wake up to process APC's.
     *
     */
    
    NtStatus = KeWaitForMutexObject(&pExampleList->kInstanceBufferMutex, Executive, KernelMode, FALSE, NULL);

    if(NT_SUCCESS(NtStatus))
    {
        DbgPrint("Start Index = %i Stop Index = %i Size Of Buffer = %i\n", pExampleList->uiStartIndex, pExampleList->uiStopIndex, sizeof(pExampleList->pCircularBuffer));
        /*
         * To copy the buffer created in user mode to the kernel buffer we need to determine
         * where the circular buffer pointers are.
         *
         *  [*****              ************************]
         *     Stop           Start
         * 
         * If the Start index is farther than the Stop index then we can assume that memory from the Start to the
         * index (including wrapping).  So Start - (Stop + 1) = Valid Length.  We want to add 1 to the Stop index 
         * so that we do not get Start == Stop since this is reserved for empty buffer.
         *
         */

       DbgPrint("Example_WriteData = %i > %i\n", pExampleList->uiStartIndex, (pExampleList->uiStopIndex + 1));
       if(pExampleList->uiStartIndex > (pExampleList->uiStopIndex + 1))
       {
          UINT uiCopyLength = MIN((pExampleList->uiStartIndex - (pExampleList->uiStopIndex + 1)), uiLength);
          
          DbgPrint("uiCopyLength = %i (%i, %i)\n", uiCopyLength, (pExampleList->uiStartIndex - (pExampleList->uiStopIndex + 1)), uiLength);

          if(uiCopyLength)
          {
              RtlCopyMemory(pExampleList->pCircularBuffer + pExampleList->uiStopIndex, pData, uiCopyLength);

              pExampleList->uiStopIndex += uiCopyLength;
              *pdwStringLength =  uiCopyLength;

              bDataWritten = TRUE;
          }
       }
       else
       {
           UINT uiLinearLengthAvailable;
           UINT uiCopyLength;

            /*
             * To copy the buffer created in user mode to the kernel buffer we need to determine
             * where the circular buffer pointers are.
             *
             *  [    ****************                     ]
             *     Start           Stop
             * 
             * If the Start index is before the Stop Index we can assume that the memory from the Start index
             * to the STop Index is full.  Thus, we first want to copy linear memory from the stop index
             * to the end of the buffer.
             *
             * To complicate matters, we don't want Start == Stop since this will signify an empty buffer in our
             * simple example.  So, what we do is that if the Start Index == 0, instead of using
             * buffer size - Stop Index and wrapping the Stop Index we simply add 1 to Stop Index so that we
             * get a smaller area and we won't wrap around.
             *
             */
           if(pExampleList->uiStartIndex <= pExampleList->uiStopIndex)
           {
           
               if(pExampleList->uiStartIndex == 0)
               {
                   uiLinearLengthAvailable = sizeof(pExampleList->pCircularBuffer) - (pExampleList->uiStopIndex + 1);
               }
               else
               {
                   uiLinearLengthAvailable = sizeof(pExampleList->pCircularBuffer) - pExampleList->uiStopIndex;
               }
    
               uiCopyLength = MIN(uiLinearLengthAvailable, uiLength);
    
               DbgPrint("uiCopyLength %i = MIN(uiLinearLengthAvailable %i, uiLength %i)\n", uiCopyLength, uiLinearLengthAvailable, uiLength);
    
               if(uiCopyLength)
               {
                   RtlCopyMemory(pExampleList->pCircularBuffer + pExampleList->uiStopIndex, pData, uiCopyLength);
    
                   pExampleList->uiStopIndex += uiCopyLength;
                   *pdwStringLength =  uiCopyLength;
    
                   bDataWritten = TRUE;
    
                   if(pExampleList->uiStopIndex == sizeof(pExampleList->pCircularBuffer))
                   {
                      pExampleList->uiStopIndex = 0;
    
                      DbgPrint("pExampleList->uiStopIndex = 0 %i - %i = %i\n", uiLength , uiCopyLength, (uiLength - uiCopyLength));
    
                      if((uiLength - uiCopyLength) > 0)
                      {
                          UINT uiSecondCopyLength = MIN(pExampleList->uiStartIndex - (pExampleList->uiStopIndex + 1), uiLength - uiCopyLength);
                          
                          DbgPrint("uiSecondCopyLength = 0 %i\n", uiSecondCopyLength);
    
                          if(uiSecondCopyLength)
                          {
                             RtlCopyMemory(pExampleList->pCircularBuffer + pExampleList->uiStopIndex, pData, uiSecondCopyLength);
    
                             pExampleList->uiStopIndex += uiSecondCopyLength;
                             *pdwStringLength =  uiCopyLength + uiSecondCopyLength;
    
                             bDataWritten = TRUE;
                          }
    
                      }
                   }
    
               }
           }
       }
        
       DbgPrint("Start Index = %i Stop Index = %i Size Of Buffer = %i\n", pExampleList->uiStartIndex, pExampleList->uiStopIndex, sizeof(pExampleList->pCircularBuffer));

       KeReleaseMutex(&pExampleList->kInstanceBufferMutex, FALSE);
    }

    return bDataWritten;
}

/**********************************************************************
 * 
 *  Example_ReadData
 *
 *    This is called to read data from the circular buffer.
 *    This function does not block and will not wait for data, so
 *    no data will simply fail this call.  If any data at all 
 *    is read then we will return success with the number of bytes
 *    read.
 *
 **********************************************************************/
BOOLEAN Example_ReadData(PEXAMPLE_LIST pExampleList, PCHAR pData, UINT uiLength, UINT *pdwStringLength)
{
    BOOLEAN bDataRead = FALSE;
    NTSTATUS NtStatus;

    *pdwStringLength = 0;

    /*
     * We want to acquire the instance mutex so we can manipulate the buffer indepdent of other
     * threads (in our process and other processes).
     * 
     * We want to wait so that we cannot be interrupted and we will not wake up to process APC's.
     *
     */
    
    NtStatus = KeWaitForMutexObject(&pExampleList->kInstanceBufferMutex, Executive, KernelMode, FALSE, NULL);

    if(NT_SUCCESS(NtStatus))
    {
        DbgPrint("Start Index = %i Stop Index = %i Size Of Buffer = %i\n", pExampleList->uiStartIndex, pExampleList->uiStopIndex, sizeof(pExampleList->pCircularBuffer));
        /*
         * To copy the buffer created in kernel mode to the user buffer we need to determine
         * where the circular buffer pointers are.
         *
         *  [    ****************               ]
         *     Start           Stop
         * 
         * If the start index is before the stop index we have a linear buffer we can copy.
         *
         */

       DbgPrint("Start Index = %i Stop Index = %i Size Of Buffer = %i\n", pExampleList->uiStartIndex, pExampleList->uiStopIndex, sizeof(pExampleList->pCircularBuffer));

       if(pExampleList->uiStartIndex < pExampleList->uiStopIndex)
       {
          UINT uiCopyLength = MIN(pExampleList->uiStopIndex - pExampleList->uiStartIndex, uiLength);

          if(uiCopyLength)
          {
              RtlCopyMemory(pData, pExampleList->pCircularBuffer + pExampleList->uiStartIndex, uiCopyLength);
              pExampleList->uiStartIndex += uiCopyLength;
              *pdwStringLength =  uiCopyLength;
              bDataRead = TRUE;
          }
       }
       else
       {

           /*
            * To copy the buffer created in kernel mode to the user buffer we need to determine
            * where the circular buffer pointers are.
            *
            *  [****               **************]
            *     Stop           Start
            * 
            * If the stop index is before the start index we have a nonlinear buffer we can copy.
            * We want to copy all the way to the end if we can.  Then we wrap around, if the
            * Start and Stop are equal, we have an empty buffer now.  If they are not then we
            * have more memory to copy.  We must always respect the buffer size provided by user mode though.
            *
            */

           if(pExampleList->uiStartIndex > pExampleList->uiStopIndex)
           {
               UINT uiLinearLengthAvailable = sizeof(pExampleList->pCircularBuffer) - pExampleList->uiStartIndex;
               UINT uiCopyLength = MIN(uiLinearLengthAvailable, uiLength);
    
    
               uiCopyLength = MIN(uiLinearLengthAvailable, uiLength);
    
               if(uiCopyLength)
               {
                   RtlCopyMemory(pData, pExampleList->pCircularBuffer + pExampleList->uiStartIndex, uiCopyLength);
    
                   pExampleList->uiStartIndex += uiCopyLength;
                   *pdwStringLength =  uiCopyLength;
                   bDataRead = TRUE;
    
                   if(pExampleList->uiStartIndex == sizeof(pExampleList->pCircularBuffer))
                   {
                      pExampleList->uiStartIndex = 0;
    
                      if(uiLength - uiCopyLength > 0)
                      {
                          UINT uiSecondCopyLength = MIN(pExampleList->uiStopIndex - pExampleList->uiStartIndex, uiLength - uiCopyLength);
    
                          if(uiSecondCopyLength)
                          {
                             RtlCopyMemory(pData, pExampleList->pCircularBuffer + pExampleList->uiStartIndex, uiCopyLength);
                             pExampleList->uiStartIndex += uiSecondCopyLength;
                             *pdwStringLength =  uiCopyLength + uiSecondCopyLength;
                             bDataRead = TRUE;
                          }
    
                      }
                   }
    
               }
           }
       }
       
       DbgPrint("Start Index = %i Stop Index = %i Size Of Buffer = %i\n", pExampleList->uiStartIndex, pExampleList->uiStopIndex, sizeof(pExampleList->pCircularBuffer));

       KeReleaseMutex(&pExampleList->kInstanceBufferMutex, FALSE);
    }

    return bDataRead;
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
 *  Example_HandleSampleIoctl_DirectOutIo
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
 *  Example_HandleSampleIoctl_BufferedIo
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
 *  Example_HandleSampleIoctl_NeitherIo
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


