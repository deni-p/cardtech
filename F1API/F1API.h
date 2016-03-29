/*++

Module Name:

    F1API.H

Abstract:

    Master include file for applications that call functions
    provided by F1API.DLL

Revision History:

    2014-02-28 : created

--*/

#ifndef _F1API_H
#define _F1API_H

#include <Windows.h>


// Return codes of the APIs
//
#define F1_S_SUCCESS			0
#define F1_E_DEVICE_UNRECOGNIZED	0x300
#define F1_E_PORT_UNAVAILABLE		0x303
#define F1_E_UNKNOWN_ERROR		0x311
#define F1_E_INTERNAL_ERROR		0x312
#define F1_E_COMM_TIMEOUT		0x320
#define F1_E_INVALID_HANDLE		0x330
#define F1_E_INVALID_PARAMETER		0x331
#define F1_E_NO_MEMORY			0x332
#define F1_E_BUFFER_TOO_SMALL		0x334


// Handle of the C/I
//
typedef ULONG ISSUERHANDLE;
typedef ISSUERHANDLE *PISSUERHANDLE;


#ifdef __cplusplus
extern "C" {
#endif

// C/I Operations
//

LONG
WINAPI
F1_Connect(
	IN	DWORD	dwPortNumber,
	IN	DWORD	dwBaudRate,
	IN	BYTE	bAddress,
	OUT	PISSUERHANDLE phIssuer
	);
LONG
WINAPI
F1_Disconnect(
	IN	ISSUERHANDLE hIssuer
	);

LONG
WINAPI
F1_Reset(
	IN	ISSUERHANDLE hIssuer
	);
LONG
WINAPI
F1_SetBaudRate(
	IN	ISSUERHANDLE hIssuer,
	IN	DWORD	dwBaudRate
	);
LONG
WINAPI
F1_SetCommAddress(
	IN	ISSUERHANDLE hIssuer,
	IN	BYTE	bAddress
	);
LONG
WINAPI
F1_GetCommAddress(
	IN	ISSUERHANDLE hIssuer,
	OUT	PBYTE	pbAddress
	);

// Device Status Flags
//
#define STATUS_CAPTURE_BOX_FULL	0x1000
#define STATUS_DISPENSING	0x0800
#define STATUS_CAPTURING	0x0400
#define STATUS_DISPENSE_ERROR	0x0200
#define STATUS_CAPTURE_ERROR	0x0100
#define STATUS_CARD_OVERLAPPED	0x0040
#define STATUS_CARD_JAMMED	0x0020
#define STATUS_CARD_INSUFFICIENT	0x0010
#define STATUS_CARD_BOX_EMPTY	0x0008
#define STATUS_S3_CARD_IN	0x0004
#define STATUS_S2_CARD_IN	0x0002
#define STATUS_S1_CARD_IN	0x0001

LONG
WINAPI
F1_GetStatus(
	IN	ISSUERHANDLE hIssuer,
	OUT	PDWORD	pdwStatusFlags
	);

#define DISP_EJECT_TO_FRONT	0x30
#define DISP_RETURN_TO_FRONT	0x34
#define DISP_RETURN_TO_READ_POS	0x36
LONG
WINAPI
F1_Dispense(
	IN	ISSUERHANDLE hIssuer,
	IN	BYTE	bMode
	);
LONG
WINAPI
F1_Capture(
	IN	ISSUERHANDLE hIssuer
	);

#define EM_PROHIBITED		0x30
#define EM_CAPTURE_TO_BOX	0x31
#define EM_DOCKED_TO_READ_POS	0x32
LONG
WINAPI
F1_SetEntryMode(
	IN	ISSUERHANDLE hIssuer,
	IN	BYTE	bMode
	);

LONG
WINAPI
F1_GetEntryMode(
	IN	ISSUERHANDLE hIssuer,
	OUT	PBYTE	pbMode
	);

#ifdef __cplusplus
}
#endif

#endif
