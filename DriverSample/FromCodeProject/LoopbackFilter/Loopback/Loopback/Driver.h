//
// LoopBack Driver by Potluri Suresh.
//
#ifndef __DRIVER_H__
#define __DRIVER_H__

#pragma once

#include <NTDDK.h>

typedef struct _DEVICE_EXTENSION {
	PDEVICE_OBJECT pDevice;
	ULONG DeviceNumber;
	UNICODE_STRING devName;
	UNICODE_STRING symLinkName;
	// Reserve space for pointer to loopback buffer
	PVOID deviceBuffer;
	ULONG deviceBufferSize;

} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

#endif /* __DRIVER_H__ */
