/*
 * LbkFlt.h - LoopBack Filter Driver by Mayur Thigale.
 * Refer for loopback driver http://www.codeproject.com/KB/system/loopback.aspx
 */
#include <NTDDK.h>
#include "lbkflt.h"

/* 
 * Forward declarations 
 */
static NTSTATUS CreateAndAttachDevice (IN PDRIVER_OBJECT pDriverObject);

static NTSTATUS LBKFltCreate(IN PDEVICE_OBJECT	pDevObj, IN PIRP pIrp);
static NTSTATUS LBKFltClose (IN PDEVICE_OBJECT	pDevObj, IN PIRP pIrp);
static NTSTATUS LBKFltWrite (IN PDEVICE_OBJECT	pDevObj, IN PIRP pIrp);
static NTSTATUS LBKFltRead  (IN PDEVICE_OBJECT	pDevObj, IN PIRP pIrp);

static VOID	LBKFltUnload (IN PDRIVER_OBJECT pDriverObject);

static NTSTATUS LBKFltPassThrough( IN PDEVICE_OBJECT  pDeviceObject, IN PIRP  pIrp);

/* 
 * Driver Entry Point
 */
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath) 
{
	ULONG ulIndex = 0;
	NTSTATUS status;

	UNREFERENCED_PARAMETER(pRegistryPath);
	/*
	 * Announce other driver entry points
	 */
	pDriverObject->DriverUnload = LBKFltUnload;

	/*
	 * Set Dispatch entry points to pass through
	 */
	for (ulIndex = 0; ulIndex <= IRP_MJ_MAXIMUM_FUNCTION;  ulIndex++) {
		pDriverObject->MajorFunction[ulIndex] = LBKFltPassThrough;
	}

	/*
	 * Override entry points we are interested in	
	 */
	pDriverObject->MajorFunction[IRP_MJ_CREATE]	= LBKFltCreate;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE]	= LBKFltClose;
	pDriverObject->MajorFunction[IRP_MJ_WRITE]	= LBKFltWrite;
	pDriverObject->MajorFunction[IRP_MJ_READ]	= LBKFltRead;

	/*
	 * Create a new device and attach to loopback device
	 */
	status = CreateAndAttachDevice(pDriverObject);	

	return status;
}


NTSTATUS CreateAndAttachDevice(PDRIVER_OBJECT pDriverObject) 
{
	NTSTATUS status = STATUS_SUCCESS;
	PDEVICE_OBJECT pDevObj = NULL, pLBKdev = NULL;
	PDEVICE_EXTENSION pDevExt = NULL;
	UNICODE_STRING devNameFlt = {0}, devName = {0};
	UNICODE_STRING symLinkNameFlt = {0};
	PFILE_OBJECT pFileObject = NULL;

	/*
	 * Form the Device Name and symbolic name
	 */
	RtlInitUnicodeString(&devNameFlt, L"\\Device\\LOOPBACKFLT");
	RtlInitUnicodeString(&symLinkNameFlt, L"\\DosDevices\\LBKFLT"); 

	RtlInitUnicodeString(&devName, L"\\Device\\LOOPBACK");

	/*
	 * Now create the device
	 */
	status = IoCreateDevice( pDriverObject,
						sizeof(DEVICE_EXTENSION),
						&devNameFlt,
						FILE_DEVICE_UNKNOWN,
						0, FALSE,
						&pDevObj );
	if (!NT_SUCCESS(status)){
		goto cleanup_failure;
	}

	/*
	 * Retrieve device extension pointer from device object
	 */
	pDevExt=(PDEVICE_EXTENSION)pDevObj->DeviceExtension;
	
	/*
	 * Attach device to loopback device stack 
	 */
	/*---------------------- Alternatives Start---------------------------------------------------------*/
	/*---------------------- Alternative 1-------------------------------------------------------------*/
	/*status = IoAttachDevice(pDevObj, &devName, &(pDevExt->pTargetDeviceObject));
	if(status != STATUS_SUCCESS){
		DbgPrint("IoAttachDevice failed with error = 0x%0x\n", status);
		goto cleanup_failure;
	}*/

	/*---------------------- Alternative 2-------------------------------------------------------------*/
	/*
	 * Get Device Pointer
	 */	
	/*status = IoGetDeviceObjectPointer( &devName, FILE_ALL_ACCESS, &pFileObject, &(pLBKdev));	
	if(status != STATUS_SUCCESS){
		DbgPrint("IoGetDeviceObjectPointer failed with error = 0x%0x\n", status);
		goto cleanup_failure;
	}
	ASSERT(pLBKdev);	
	DbgPrint("%wZ Device Object Pointer 0x%0x\n", &devName, pLBKdev);

	ObReferenceObject(pLBKdev);

	pDevExt->pTargetDeviceObject = IoAttachDeviceToDeviceStack( pDevObj, pLBKdev);
	if(pDevExt->pTargetDeviceObject){
   		 DbgPrint("Attached to loopback device successfully\n");
		 status = STATUS_UNSUCCESSFUL;
		 goto cleanup_failure;
	}else{
		 DbgPrint("Attaching to loopback device failed\n");
	}*/

	/*---------------------- Alternative 3--------------------------------------------------------------*/
	/*
	 * Get Device Pointer
	 */	
	status = IoGetDeviceObjectPointer( &devName,  FILE_ALL_ACCESS,  &pFileObject,  &(pLBKdev)  );	
	if(status != STATUS_SUCCESS){
		DbgPrint("IoGetDeviceObjectPointer failed with error = 0x%0x\n", status);
		goto cleanup_failure;
	}
	ASSERT(pLBKdev);	
	DbgPrint("%wZ Device Object Pointer 0x%0x\n", &devName, pLBKdev);

	ObReferenceObject(pLBKdev);
	
 	status = IoAttachDeviceToDeviceStackSafe(pDevObj, pLBKdev, &pDevExt->pTargetDeviceObject);
	if(status != STATUS_SUCCESS){
		DbgPrint("IoGetDeviceObjectPointer failed with error = 0x%0x\n", status);
		goto cleanup_failure;
	}

	/*---------------------- Way End--------------------------------------------------------------*/

	/*
	 * Propogate device flags
	 */
	pDevObj->Flags = pDevExt->pTargetDeviceObject->Flags;
	/*
	 * Create the symbolic link name, this is not mandatory 
	 * but can be helpful for user mode apps to communicate
	 */
	status = IoCreateSymbolicLink( &symLinkNameFlt, &devNameFlt );
	if (!NT_SUCCESS(status)) {
		// if it fails now, must delete Device object
		DbgPrint("IoCreateSymbolicLink failed with error = 0x%0x\n", status);
		goto cleanup_failure;
	}

	/*
	 * Fill Device extension information
	 */
	pDevExt->pDevice = pDevObj;
	pDevExt->pFileObject = pFileObject;
	pDevExt->pLBKdev = pLBKdev;
	RtlInitUnicodeString(&pDevExt->devName, L"\\Device\\LOOPBACKFLT");
	RtlInitUnicodeString(&pDevExt->symLinkName, L"\\DosDevices\\LBKFLT"); 

	goto cleanup;

cleanup_failure:
	if(pDevObj)			IoDeleteDevice(pDevObj);
	if(pFileObject)		ObDereferenceObject(pFileObject);	
	if(pLBKdev)			ObDereferenceObject(pDevObj);

cleanup:
	return status;
}


VOID LBKFltUnload (PDRIVER_OBJECT	pDriverObject) 
{
	PDEVICE_OBJECT	pNextObj;

	/*
	 * Loop through each device controlled by Driver
	 * Currently we have only one device
	 */
	pNextObj = pDriverObject->DeviceObject;

	while (NULL != pNextObj ) {
		PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION) pNextObj->DeviceExtension;
		
		IoDeleteSymbolicLink(&pDevExt->symLinkName);
		pNextObj = pNextObj->NextDevice;

		if(pDevExt->pFileObject)		ObDereferenceObject(pDevExt->pFileObject);	
		if(pDevExt->pLBKdev)			ObDereferenceObject(pDevExt->pLBKdev);

		IoDetachDevice( pDevExt->pTargetDeviceObject );
		IoDeleteDevice( pDevExt->pDevice );
	}
	return;
}


NTSTATUS LBKFltPassThrough( IN PDEVICE_OBJECT  pDevObj, IN PIRP  pIrp)
{
	PDEVICE_EXTENSION   pDevExt = (PDEVICE_EXTENSION) pDevObj->DeviceExtension;

	DbgPrint("LBFlltPassThrough was called \n");

	/*
	 * Let below driver use our stack location
	 */
	IoSkipCurrentIrpStackLocation(pIrp);
	/*
	 * send IRP to lower target device
	 */
	return IoCallDriver(pDevExt->pTargetDeviceObject, pIrp);
}


NTSTATUS LBKFltCreate(PDEVICE_OBJECT	pDevObj, PIRP pIrp) 
{
	PDEVICE_EXTENSION   pDevExt = (PDEVICE_EXTENSION) pDevObj->DeviceExtension;

	DbgPrint("LBKFltCreate was called \n");

	IoSkipCurrentIrpStackLocation(pIrp);	
	return IoCallDriver(pDevExt->pTargetDeviceObject, pIrp);
}


NTSTATUS LBKFltClose(PDEVICE_OBJECT	pDevObj, PIRP pIrp) 
{
	PDEVICE_EXTENSION   pDevExt = (PDEVICE_EXTENSION) pDevObj->DeviceExtension;

	DbgPrint("LBKFltClose was called \n");

	IoSkipCurrentIrpStackLocation(pIrp);

	return IoCallDriver(pDevExt->pTargetDeviceObject, pIrp);
}


NTSTATUS LBKFltWrite(IN PDEVICE_OBJECT pDevObj,IN PIRP pIrp) 
{	
	PDEVICE_EXTENSION   pDevExt = (PDEVICE_EXTENSION) pDevObj->DeviceExtension;
	PIO_STACK_LOCATION pIrpStack = IoGetCurrentIrpStackLocation( pIrp );
	PREADWRITE_IO_CONTEXT Context = NULL;

	DbgPrint("LBKFltWrite was called \n");

	Context = ExAllocatePoolWithTag(NonPagedPool, sizeof(READWRITE_IO_CONTEXT), LBKFLT_TAG_NPAGED);
	if(NULL == Context){
		DbgPrint("Memory allocation failure for read io context \n");
		pIrp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
		pIrp->IoStatus.Information = 0;
		IoCompleteRequest(pIrp, IO_NO_INCREMENT);
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	Context->liOffset = pIrpStack->Parameters.Write.ByteOffset;
	Context->ulLength = pIrpStack->Parameters.Write.Length;

#ifdef STRING_TESET_MODE
	/*
	 * Only DO_BUFFERED_IO is supported
	 */
	DbgPrint("Data = %s\n", pIrp->AssociatedIrp.SystemBuffer);
#endif /* STRING_TESET_MODE */

	/*
	 * We are not setting completion routine here to check the io completion status
	 * There are chances that lower drivers fail the request
	 */

	IoSkipCurrentIrpStackLocation(pIrp);

	ExFreePoolWithTag(Context, LBKFLT_TAG_NPAGED);
	return IoCallDriver(pDevExt->pTargetDeviceObject, pIrp);
}


NTSTATUS ReadIoCompletion( IN PDEVICE_OBJECT  pDevObj, IN PIRP  pIrp, IN PVOID  Context )
{
	PREADWRITE_IO_CONTEXT pCtx = (PREADWRITE_IO_CONTEXT)Context;

	UNREFERENCED_PARAMETER(pDevObj);

	if(NT_SUCCESS(pIrp->IoStatus.Status)){
		
		/*
		 * Log the IO context or do processing if required for the context
		 * We are not doing any procesing here
		 */
		ExFreePoolWithTag(pCtx, LBKFLT_TAG_NPAGED);
#ifdef STRING_TESET_MODE
		/*
		 * Only DO_BUFFERED_IO is supported
		 */
		DbgPrint("Data = %s\n", pIrp->AssociatedIrp.SystemBuffer);
#endif /* STRING_TESET_MODE */
	}

	return STATUS_SUCCESS;
}


NTSTATUS LBKFltRead(IN PDEVICE_OBJECT	pDevObj, IN PIRP pIrp) 
{
	PDEVICE_EXTENSION   pDevExt = (PDEVICE_EXTENSION) pDevObj->DeviceExtension;
	PIO_STACK_LOCATION pIrpStack = IoGetCurrentIrpStackLocation( pIrp );
	PREADWRITE_IO_CONTEXT Context = NULL;

	DbgPrint("LBKFltRead was called \n");

	Context = ExAllocatePoolWithTag(NonPagedPool, sizeof(READWRITE_IO_CONTEXT), LBKFLT_TAG_NPAGED);
	if(NULL == Context){
		DbgPrint("Memory allocation failure for read io context \n");
		pIrp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
		pIrp->IoStatus.Information = 0;
		IoCompleteRequest(pIrp, IO_NO_INCREMENT);
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	Context->liOffset = pIrpStack->Parameters.Read.ByteOffset;
	Context->ulLength = pIrpStack->Parameters.Read.Length;

	IoCopyCurrentIrpStackLocationToNext(pIrp);

	IoSetCompletionRoutine( pIrp,
							ReadIoCompletion,
							Context,
							TRUE,//InvokeOnSuccess
							TRUE, // InvokeOnError
							TRUE // InvokeOnCancel
							);
	return IoCallDriver(pDevExt->pTargetDeviceObject, pIrp);
}

