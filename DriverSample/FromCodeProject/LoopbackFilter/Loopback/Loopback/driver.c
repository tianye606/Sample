// Driver.c - LoopBack Driver for two processors  by Potluri Suresh.
//
#include <NTDDK.h>
#include "Driver.h"

// Forward declarations
//
static NTSTATUS CreateDevice (IN PDRIVER_OBJECT	pDriverObject,IN ULONG DeviceNumber);
static VOID DriverUnload (IN PDRIVER_OBJECT	pDriverObject	);
static NTSTATUS DispatchCreate (IN PDEVICE_OBJECT	pDevObj,IN PIRP pIrp);
static NTSTATUS DispatchClose (IN PDEVICE_OBJECT	pDevObj, IN PIRP pIrp);
static NTSTATUS DispatchWrite (IN PDEVICE_OBJECT	pDevObj, IN PIRP pIrp);
static NTSTATUS DispatchRead (IN PDEVICE_OBJECT	pDevObj,IN PIRP pIrp);

NTSTATUS DriverEntry (PDRIVER_OBJECT pDriverObject,PUNICODE_STRING pRegistryPath	) 
{
	ULONG ulDeviceNumber = 0;
	NTSTATUS status;

	// Announce other driver entry points
	pDriverObject->DriverUnload = DriverUnload;

	// This includes Dispatch routines for Create, Write & Read
	pDriverObject->MajorFunction[IRP_MJ_CREATE]	=DispatchCreate;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE]	=DispatchClose;
	pDriverObject->MajorFunction[IRP_MJ_WRITE]	=DispatchWrite;
	pDriverObject->MajorFunction[IRP_MJ_READ]	=DispatchRead;
	
	// For each physical or logical device detected
	// that will be under this Driver's control,
	// a new Device object must be created.
	status = CreateDevice(pDriverObject, ulDeviceNumber);	

	return status;
}

NTSTATUS CreateDevice (PDRIVER_OBJECT	pDriverObject,ULONG ulDeviceNumber	) 
{
	NTSTATUS status;
	PDEVICE_OBJECT pDevObj;
	PDEVICE_EXTENSION pDevExt;
	UNICODE_STRING devName;
	UNICODE_STRING symLinkName;

	// Form the internal Device Name
	RtlInitUnicodeString(&devName, L"\\Device\\LOOPBACK");
	RtlInitUnicodeString(&symLinkName, L"\\DosDevices\\LBK"); 

	// Now create the device
	status = IoCreateDevice( pDriverObject,
						sizeof(DEVICE_EXTENSION),
						&devName,
						FILE_DEVICE_UNKNOWN,
						0, FALSE,
						&pDevObj );
	if (!NT_SUCCESS(status))
		return status;

	// Announce that we will be working with a copy of the user's buffer
	pDevObj->Flags |= DO_BUFFERED_IO;

	// Initialize the Device Extension
	pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;
	pDevExt->pDevice = pDevObj;	// back pointer

	// Form the internal Device Name
	pDevExt->devName = devName;
	pDevExt->symLinkName = symLinkName;
	pDevExt->DeviceNumber = ulDeviceNumber;
	pDevExt->deviceBuffer = NULL;
	pDevExt->deviceBufferSize = 0;

	// Now create the link name
	status = IoCreateSymbolicLink( &symLinkName, &devName );
	if (!NT_SUCCESS(status)) {
		// if it fails now, must delete Device object
		IoDeleteDevice( pDevObj );
		return status;
	}

	return STATUS_SUCCESS;
}

VOID DriverUnload (PDRIVER_OBJECT	pDriverObject) 
{
	PDEVICE_OBJECT	pNextObj;

	// Loop through each device controlled by Driver
	pNextObj = pDriverObject->DeviceObject;

	while (pNextObj != NULL) {
		PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION) pNextObj->DeviceExtension;
		// Free up any buffer still held by this device
		if (pDevExt->deviceBuffer != NULL) {
			ExFreePool(pDevExt->deviceBuffer);
			pDevExt->deviceBuffer = NULL;
			pDevExt->deviceBufferSize = 0;
		}

		// ... delete symbolic link name
		IoDeleteSymbolicLink(&pDevExt->symLinkName);
		pNextObj = pNextObj->NextDevice;

		// then delete the device using the Extension
		IoDeleteDevice( pDevExt->pDevice );
	}
	return;
}

NTSTATUS DispatchCreate (PDEVICE_OBJECT	pDevObj, PIRP pIrp) 
{
	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;	

	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;	// no bytes xfered
	IoCompleteRequest( pIrp, IO_NO_INCREMENT );
	return STATUS_SUCCESS;
}

NTSTATUS DispatchClose (IN PDEVICE_OBJECT	pDevObj,IN PIRP pIrp)
{
	ULONG	i,thNum;
	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION) pDevObj->DeviceExtension;

	if (pDevExt->deviceBuffer != NULL) {
		ExFreePool(pDevExt->deviceBuffer);
		pDevExt->deviceBuffer = NULL;
		pDevExt->deviceBufferSize = 0;
	}

	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;	// no bytes xfered
	IoCompleteRequest( pIrp, IO_NO_INCREMENT );
	return STATUS_SUCCESS;
}


NTSTATUS DispatchWrite (IN PDEVICE_OBJECT pDevObj,IN PIRP pIrp) 
{	
	NTSTATUS status = STATUS_SUCCESS;
	PVOID userBuffer;
	ULONG xferSize;
	ULONG	i,thNum;

	// The stack location contains the user buffer info
	PIO_STACK_LOCATION pIrpStack = IoGetCurrentIrpStackLocation( pIrp );
	
	// Dig out the Device Extension from the Device object
	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;

	/* Thread  */
	if (pDevExt->deviceBuffer != NULL) {
		ExFreePool(pDevExt->deviceBuffer);
		pDevExt->deviceBuffer = NULL;
		pDevExt->deviceBufferSize = 0;
	}

	// Determine the length of the request
	xferSize = pIrpStack->Parameters.Write.Length;
	// Obtain user buffer pointer
	userBuffer = pIrp->AssociatedIrp.SystemBuffer;

	// Allocate the new buffer
	pDevExt->deviceBuffer  =	ExAllocatePool( PagedPool, xferSize );

	if (pDevExt->deviceBuffer  == NULL) {
		// buffer didn't allocate???
		status = STATUS_INSUFFICIENT_RESOURCES;
		xferSize = 0;
	} else {
		// copy the buffer
		pDevExt->deviceBufferSize = xferSize;
		RtlCopyMemory( pDevExt->deviceBuffer, userBuffer,xferSize );
	}

	// Now complete the IRP
	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = xferSize;	// bytes xfered
	IoCompleteRequest( pIrp, IO_NO_INCREMENT );
	return status;
}

NTSTATUS DispatchRead (PDEVICE_OBJECT	pDevObj,PIRP pIrp) 
{	
	NTSTATUS status = STATUS_SUCCESS;
	PVOID userBuffer;
	ULONG xferSize,i,thNum ;

	// The stack location contains the user buffer info
	PIO_STACK_LOCATION pIrpStack = IoGetCurrentIrpStackLocation( pIrp );

	// Dig out the Device Extension from the Device object
	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION) pDevObj->DeviceExtension;

	// Determine the length of the request
	xferSize = pIrpStack->Parameters.Read.Length;
	userBuffer = pIrp->AssociatedIrp.SystemBuffer;

	// Don't transfer more than the user's request
	xferSize = (xferSize < pDevExt->deviceBufferSize) ? xferSize : 
											pDevExt->deviceBufferSize;

	// Now copy the pool buffer into user space
	RtlCopyMemory( userBuffer, pDevExt->deviceBuffer, xferSize );

	// Free pool buffer
	ExFreePool(pDevExt->deviceBuffer);
	pDevExt->deviceBuffer = NULL;
	pDevExt->deviceBufferSize = 0;

	// Now complete the IRP
	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = xferSize;	// bytes xfered
	IoCompleteRequest( pIrp, IO_NO_INCREMENT );
	return status;
}

