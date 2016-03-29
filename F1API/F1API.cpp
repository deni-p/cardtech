// U5BCDAPI.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "F1API.h"
#include "CardIssuer.h"


static LONG ExecCommand(
	IN	ISSUERHANDLE hIssuer,
	IN	BYTE	bCommandCode,
	IN	BYTE	bParameterCode,
	IN	PBYTE	pbBodyData,
	IN	BYTE	cbBodyLength,
	OUT	PBYTE	pbRecvBuff,
	IN OUT	PDWORD	pcbRecvLength
	)
{
	CCardIssuer *pIssuer = CCardIssuer::FromHandle( hIssuer );
	if ( pIssuer == NULL )
		return F1_E_INVALID_HANDLE;

	return pIssuer->ExecCommand(bCommandCode, bParameterCode, pbBodyData, cbBodyLength, pbRecvBuff, pcbRecvLength);
}


static LONG ExecCommand(
	IN	ISSUERHANDLE hIssuer,
	IN	BYTE	bCommandCode,
	IN	BYTE	bParameterCode1,
	IN	BYTE	bParameterCode2,
	OUT	PBYTE	pbRecvBuff,
	IN OUT	PDWORD	pcbRecvLength
	)
{
	CCardIssuer *pIssuer = CCardIssuer::FromHandle( hIssuer );
	if ( pIssuer == NULL )
		return F1_E_INVALID_HANDLE;

	return pIssuer->ExecCommand(bCommandCode, bParameterCode1, bParameterCode2, pbRecvBuff, pcbRecvLength);
}


LONG
WINAPI
F1_Connect(
	IN	DWORD	dwPortNumber,
	IN	DWORD	dwBaudRate,
	IN	BYTE	bDevAddress,
	OUT	PISSUERHANDLE phIssuer
	)
{
	if ( phIssuer == NULL )
		return F1_E_INVALID_PARAMETER;

	if ( !(bDevAddress >= 0 && bDevAddress <= 15 || bDevAddress == 0xff) )
		return F1_E_INVALID_PARAMETER;

	CCardIssuer *pIssuer = new CCardIssuer();
	if (pIssuer == NULL)
		return F1_E_NO_MEMORY;

	LONG lResult = pIssuer->Connect(dwPortNumber, dwBaudRate, bDevAddress);
	if (lResult == 0)
	{
		*phIssuer = (ISSUERHANDLE) pIssuer;
	}
	else
	{
		delete pIssuer;
	}

	return lResult;
}


LONG
WINAPI
F1_Disconnect(
	IN	ISSUERHANDLE hIssuer
	)
{
	CCardIssuer *pIssuer = CCardIssuer::FromHandle( hIssuer );
	if ( pIssuer == NULL )
		return F1_E_INVALID_HANDLE;

	return pIssuer->Disconnect();
}



LONG
WINAPI
F1_Reset(
	IN	ISSUERHANDLE hIssuer
	)
{
	return ExecCommand(hIssuer, 'R', 'S', NULL, NULL, NULL, NULL);
}


LONG
WINAPI
F1_SetBaudRate(
	IN	ISSUERHANDLE hIssuer,
	IN	DWORD	dwBaudRate
	)
{
	switch ( dwBaudRate )
	{
	case 1200:	return ExecCommand(hIssuer, 'C', 'S',0x30, NULL, NULL);
	case 2400:	return ExecCommand(hIssuer, 'C', 'S',0x31, NULL, NULL);
	case 4800:	return ExecCommand(hIssuer, 'C', 'S',0x32, NULL, NULL);
	case 9600:	return ExecCommand(hIssuer, 'C', 'S',0x33, NULL, NULL);
	case 19200:	return ExecCommand(hIssuer, 'C', 'S',0x34, NULL, NULL);
	case 38400:	return ExecCommand(hIssuer, 'C', 'S',0x35, NULL, NULL);
	default:	return F1_E_INVALID_PARAMETER;
	}
}



LONG
WINAPI
F1_SetCommAddress(
	IN	ISSUERHANDLE hIssuer,
	IN	BYTE	bAddress
	)
{
	if ( !(bAddress >= 0 && bAddress <= 15 || bAddress == 0xff) )
		return F1_E_INVALID_PARAMETER;

	CCardIssuer *pIssuer = CCardIssuer::FromHandle( hIssuer );
	if ( pIssuer == NULL )
		return F1_E_INVALID_HANDLE;

	pIssuer->SetAddress(bAddress);

	return 0;
}


LONG
WINAPI
F1_GetCommAddress(
	IN	ISSUERHANDLE hIssuer,
	OUT	PBYTE	pbAddress
	)
{
	if ( pbAddress == NULL )
		return F1_E_INVALID_PARAMETER;

	CCardIssuer *pIssuer = CCardIssuer::FromHandle( hIssuer );
	if ( pIssuer == NULL )
		return F1_E_INVALID_HANDLE;

	*pbAddress = pIssuer->GetAddress();

	return 0;
}


LONG
WINAPI
F1_GetStatus(
	IN	ISSUERHANDLE hIssuer,
	OUT	PDWORD	pdwStatusFlags
	)
{
	if ( pdwStatusFlags == NULL )
		return F1_E_INVALID_PARAMETER;

	BYTE RecvBuff[4];
	DWORD RecvLength = sizeof(RecvBuff);

	LONG lResult = ExecCommand(hIssuer, 'A', 'P', NULL, NULL, RecvBuff, &RecvLength);
	if ( lResult == 0)
	{
		*pdwStatusFlags =  ((RecvBuff[0] - 0x30) << 12)
			+ ((RecvBuff[1] - 0x30) << 8)
			+ ((RecvBuff[2] - 0x30) << 4)
			+ (RecvBuff[3] - 0x30);
	}
	return lResult;
}


LONG
WINAPI
F1_Dispense(
	IN	ISSUERHANDLE hIssuer,
	IN	BYTE	bMode
	)
{
	switch ( bMode )
	{
	case DISP_EJECT_TO_FRONT:
	case DISP_RETURN_TO_FRONT:
	case DISP_RETURN_TO_READ_POS:
		return ExecCommand(hIssuer, 'F', 'C', bMode, NULL, NULL);
	default: 
		return F1_E_INVALID_PARAMETER;
	}
}

LONG
WINAPI
F1_Capture(
	IN	ISSUERHANDLE hIssuer
	)
{
	return ExecCommand(hIssuer, 'C', 'P', NULL, NULL, NULL, NULL);
}


LONG
WINAPI
F1_SetEntryMode(
	IN	ISSUERHANDLE hIssuer,
	IN	BYTE	bMode
	)
{
	switch ( bMode )
	{
	case EM_PROHIBITED:
	case EM_CAPTURE_TO_BOX:
	case EM_DOCKED_TO_READ_POS:
		return ExecCommand(hIssuer, 'I', 'N', bMode, NULL, NULL);
	default: 
		return F1_E_INVALID_PARAMETER;
	}
}


LONG
WINAPI
F1_GetEntryMode(
	IN	ISSUERHANDLE hIssuer,
	OUT	PBYTE	pbMode
	)
{
	if ( pbMode == NULL )
		return F1_E_INVALID_PARAMETER;

	BYTE RecvBuff[1];
	DWORD RecvLength = sizeof(RecvBuff);

	LONG lResult = ExecCommand(hIssuer, 'S', 'I', NULL, NULL, RecvBuff, &RecvLength);
	if ( lResult == 0)
	{
		if ( ! ( RecvBuff[0] == EM_PROHIBITED ||
			RecvBuff[0] == EM_CAPTURE_TO_BOX ||
			RecvBuff[0] == EM_DOCKED_TO_READ_POS ) )
			return F1_E_INTERNAL_ERROR;

		*pbMode = RecvBuff[0];
	}

	return lResult;
}
