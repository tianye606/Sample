/*
 * LbkFlt.h - LoopBack Filter Driver by Mayur Thigale.
 * Refer for loopback driver http://www.codeproject.com/KB/system/loopback.aspx
 */
#ifndef __LBKFLT_H__
#define __LBKFLT_H__

#pragma once

#include <NTDDK.h>

/*
 * Comment this line if you are not writing a NULL terminated string to loopback device
 * Loopback test app (Testor) writes string
 */
#define STRING_TESET_MODE   1

#define LBKFLT_DEVNAME		L"\\Device\\LOOPBACKFLT"
#define LBKFLT_LINKDEVNAME  L"\\DosDevices\\LBKFLT"

#define LBK_DEVNAME			L"\\Device\\LOOPBACK"

#define LBKFLT_TAG_NPAGED   'pnFL'

typedef struct _DEVICE_EXTENSION {
	PDEVICE_OBJECT pDevice;
	PDEVICE_OBJECT pTargetDeviceObject;	
	PDEVICE_OBJECT pLBKdev;
	UNICODE_STRING devName;
	UNICODE_STRING symLinkName;
	PFILE_OBJECT   pFileObject;
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;


typedef struct _READWRITE_IO_CONTEXT{

	LARGE_INTEGER liOffset;
	ULONG ulLength;
}READWRITE_IO_CONTEXT, *PREADWRITE_IO_CONTEXT;

#endif /* __LBKFLT_H__ */
