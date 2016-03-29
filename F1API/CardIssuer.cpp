#include "StdAfx.h"
#include "F1API.h"
#include "CardIssuer.h"
#include "ObjectManager.h"


// 传输控制字符
//
#define ACK	0x06	// 肯定应答
#define NAK	0x15	// 否定应答
#define ENQ	0x05	// 命令请求
#define STX	0x02	// 起始标记
#define ETX	0x03	// 结束标记


#define PURGE_FLAGS  (PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR)


static BYTE GetBCC(PBYTE pbBuffer, DWORD cbLength)
{
	BYTE bcc = 0;
	for (DWORD i = 0; i < cbLength - 1; i++)
		bcc ^= pbBuffer[i];
	
	return bcc;
}


CObjectManager _objmgr;

CCardIssuer::CCardIssuer()
	: m_hDevice( INVALID_HANDLE_VALUE ),
	m_cBuffer(0)
{
	InitializeCriticalSection(&m_csLock);
}


CCardIssuer::~CCardIssuer()
{
	Disconnect();

	DeleteCriticalSection(&m_csLock);
}


LONG	CCardIssuer::Connect(DWORD dwPort, DWORD dwBaudRate, BYTE bAddress)
{
	LONG	lResult = InitPort(dwPort, dwBaudRate);
	if ( lResult != 0 )
		return lResult;

	m_bAddress = bAddress;

	lResult = ExecCommand('R', 'S', NULL, NULL, NULL, NULL);
	if ( lResult != 0 )
		return F1_E_DEVICE_UNRECOGNIZED;

	if (! _objmgr.Add(this))
	{
		Disconnect();
		return F1_E_NO_MEMORY;
	}

	return 0;
}


LONG	CCardIssuer::Disconnect()
{
	if ( m_hDevice != INVALID_HANDLE_VALUE )
	{
		if (! CloseHandle( m_hDevice ) )
			return F1_E_UNKNOWN_ERROR;

		m_hDevice = INVALID_HANDLE_VALUE;
	}

	_objmgr.Remove(this);

	return 0;
}


LONG	CCardIssuer::ExecCommand(BYTE bCommandCode, BYTE bParameterCode1, BYTE bParameterCode2, PBYTE pbRecvBuff, PDWORD pcbRecvLength)
{
	return ExecCommand(bCommandCode, bParameterCode1, &bParameterCode2, 1, pbRecvBuff, pcbRecvLength);
}


LONG	CCardIssuer::ExecCommand(BYTE bCommandCode, BYTE bParameterCode, PBYTE pbBodyData, BYTE cbBodyLength, PBYTE pbRecvBuff, PDWORD pcbRecvLength)
{
	LONG	lResult = 0;

	EnterCriticalSection(&m_csLock);

	MakeMessage(bCommandCode, bParameterCode, pbBodyData, cbBodyLength);

	lResult = TransmissionControl(bCommandCode, bParameterCode, pbRecvBuff, pcbRecvLength);

	LeaveCriticalSection(&m_csLock);

	return lResult;
}


LONG	CCardIssuer::TransmissionControl(BYTE bCommandCode, BYTE bParameterCode, PBYTE pbRecvBuff, PDWORD pcbRecvLength)
{
	BOOL bTimeOut;

	if (! PurgeComm(m_hDevice, PURGE_FLAGS))
		return F1_E_UNKNOWN_ERROR;

	// 发送命令
	//
	if (! SendData(m_bBuffer, m_cBuffer, &bTimeOut))
	{
		if (bTimeOut)
			return F1_E_COMM_TIMEOUT;
		else 
			return F1_E_UNKNOWN_ERROR;
	}

	if (! WaitForBytesAvailable(500))
		return F1_E_COMM_TIMEOUT;

	// 接受 ACK
	//
	if (! RecvData(m_bBuffer, m_bAddress == 0xff ? 1 : 3, &bTimeOut))
	{
		if (bTimeOut)
			return F1_E_COMM_TIMEOUT;
		else 
			return F1_E_UNKNOWN_ERROR;
	}

	if (m_bBuffer[0] != ACK)
		return F1_E_INTERNAL_ERROR;

	// 发送 ENQ
	//
	m_bBuffer[0] = ENQ;
	m_bBuffer[1] = (m_bAddress / 10) + 0x30;
	m_bBuffer[2] = (m_bAddress % 10) + 0x30;

	if (! SendData(m_bBuffer, m_bAddress == 0xff ? 1 : 3, &bTimeOut))
	{
		if (bTimeOut)
			return F1_E_COMM_TIMEOUT;
		else 
			return F1_E_UNKNOWN_ERROR;
	}

	if ( pbRecvBuff != NULL 
		&& pcbRecvLength != NULL )
	{
		if ( *pcbRecvLength > 0 )
		{
			if (! WaitForBytesAvailable( 5000 ) )
				return F1_E_COMM_TIMEOUT;

			m_cBuffer = *pcbRecvLength + (m_bAddress == 0xff ? 5: 7);
			if (! RecvData(m_bBuffer, m_cBuffer, &bTimeOut))
			{
				if (bTimeOut)
					return F1_E_COMM_TIMEOUT;
				else 
					return F1_E_UNKNOWN_ERROR;
			}

			BYTE bcc = GetBCC(m_bBuffer, m_cBuffer);
			if ( m_bBuffer[m_cBuffer - 1] != bcc)
				return F1_E_INTERNAL_ERROR;

			memcpy( pbRecvBuff, &m_bBuffer[ m_bAddress == 0xff ? 3 : 5], *pcbRecvLength);
		}
	}

	return 0;
}


BOOL	CCardIssuer::SendData(PBYTE pbBuffer, DWORD dwBytesToWrite, PBOOL pbTimeOut)
{
	DWORD	dwBytesWritten;
	BOOL	fRes;
	OVERLAPPED oWrite = { 0 };

	oWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (oWrite.hEvent == NULL)
		return FALSE;
	
	if (! WriteFile(m_hDevice, pbBuffer, dwBytesToWrite, &dwBytesWritten, &oWrite))
	{
		DWORD dwError = GetLastError();
		if (dwError == ERROR_IO_PENDING)
		{
			switch (WaitForSingleObject(oWrite.hEvent, INFINITE))
			{
			case WAIT_OBJECT_0:
				if (GetOverlappedResult(m_hDevice, &oWrite, &dwBytesWritten, FALSE))
				{
					if (dwBytesWritten != dwBytesToWrite)
					{
						fRes = FALSE;
						*pbTimeOut = TRUE;
					}
					else
						fRes = TRUE;
				}
				else
					fRes = FALSE;
				break;
			case WAIT_TIMEOUT:
				fRes = FALSE;
				*pbTimeOut = TRUE;
				break;
			default:
				fRes = FALSE;
				break;
			}
		}
		else
		{
			// WriteFile 失败但不延时.
			fRes = FALSE;
			SetLastError(dwError);
		}
	}
	else 
	{
		if (dwBytesWritten != dwBytesToWrite)
			fRes = FALSE;
		else
			fRes = TRUE;
	}
	
	CloseHandle(oWrite.hEvent);
	
	return fRes;
}


BOOL	CCardIssuer::RecvData(PBYTE pbBuffer, DWORD dwBytesToRead, PBOOL pbTimeOut)
{
	DWORD dwBytesRead;
	OVERLAPPED oRead = { 0 };
	oRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (oRead.hEvent == NULL)
		return FALSE;

	BOOL fRes;
	if (! ReadFile(m_hDevice, pbBuffer, dwBytesToRead, &dwBytesRead, &oRead))
	{
		DWORD dwError = GetLastError();
		if (dwError == ERROR_IO_PENDING)
		{
			switch (WaitForSingleObject(oRead.hEvent, INFINITE))
			{
			case WAIT_OBJECT_0:
				if (GetOverlappedResult(m_hDevice, &oRead, &dwBytesRead, FALSE))
				{
					if (dwBytesRead != dwBytesToRead)
					{
						fRes = FALSE;
						*pbTimeOut = TRUE;
					}
					else
						fRes = TRUE;
				}
				else
					fRes = FALSE;
				break;	
			case WAIT_TIMEOUT:
				fRes = FALSE;
				*pbTimeOut = TRUE;
				break;	
			default:
				fRes = FALSE;
				break;
			}
		}
		else
		{
			// ReadFile 失败但不延时
			fRes = FALSE;
			SetLastError(dwError);
		}
	}
	else
	{
		if (dwBytesRead != dwBytesToRead)
			fRes = FALSE;
		else
			fRes = TRUE;
	}
	
	CloseHandle(oRead.hEvent);

	return fRes;
}


LONG	CCardIssuer::InitPort(DWORD dwPortNumber, DWORD dwBaudRate)
{
	char szPortName[32];
	sprintf_s(szPortName, sizeof(szPortName),
		"....\\..\\COM%d", dwPortNumber);

	HANDLE hDevice = CreateFileA(szPortName, 
			GENERIC_READ | GENERIC_WRITE, 
			0, 
			NULL, 
			OPEN_EXISTING, 
			FILE_FLAG_OVERLAPPED,	// 重叠 I/O
			NULL);

	if (hDevice == INVALID_HANDLE_VALUE)
	{
		switch ( GetLastError() )
		{
		case ERROR_FILE_NOT_FOUND:
		case ERROR_ACCESS_DENIED:
			return F1_E_PORT_UNAVAILABLE;
		default:
			return F1_E_UNKNOWN_ERROR;
		}
	}

	DCB dcb;
	if (! GetCommState(hDevice, &dcb))
		goto CLEARUP;

	dcb.BaudRate = dwBaudRate;
	dcb.Parity = NOPARITY;
	dcb.ByteSize = DATABITS_8;
	dcb.StopBits = ONESTOPBIT;

	if (! SetCommState(hDevice, &dcb))
		goto CLEARUP;

	if (! SetupComm(hDevice, 4096, 4096))
		goto CLEARUP;

	COMMTIMEOUTS timeouts;
	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutMultiplier = 50;
	timeouts.ReadTotalTimeoutConstant = 100;
	timeouts.WriteTotalTimeoutMultiplier = 50;
	timeouts.WriteTotalTimeoutConstant = 100;

	if (! SetCommTimeouts(hDevice, &timeouts))
		goto CLEARUP;

	m_hDevice = hDevice;

	return 0;

CLEARUP:
	CloseHandle(hDevice);

	return F1_E_UNKNOWN_ERROR;;
}


void	CCardIssuer::MakeMessage(BYTE bCommandCode, BYTE bParameterCode, PBYTE pbBodyData, BYTE cbBodyLength)
{
	m_cBuffer = 0;
	m_bBuffer[m_cBuffer++] = STX;

	if ( m_bAddress < 0xff)
	{
	m_bBuffer[m_cBuffer++] = (m_bAddress / 10) + 0x30;
	m_bBuffer[m_cBuffer++] = (m_bAddress % 10) + 0x30;
	}

	m_bBuffer[m_cBuffer++] = bCommandCode;
	m_bBuffer[m_cBuffer++] = bParameterCode;

	if ( pbBodyData != NULL && cbBodyLength > 0 )
	{
		memcpy( &m_bBuffer[m_cBuffer], pbBodyData, cbBodyLength );
		m_cBuffer += cbBodyLength;
	}

	m_bBuffer[m_cBuffer++] = ETX;
	m_cBuffer++;
	m_bBuffer[m_cBuffer - 1] = GetBCC(m_bBuffer, m_cBuffer);
}


BOOL	CCardIssuer::WaitForBytesAvailable(DWORD dwMilliseconds)
{
	COMSTAT cs;
	DWORD dwError;
	DWORD dwStartTime = GetTickCount();
	DWORD dwCurrTime;
	DWORD dwElapsedTime = 0;

	while ( TRUE )
	{
		ClearCommError(m_hDevice, &dwError, &cs);
		if (cs.cbInQue > 0)
			break;

		dwCurrTime = GetTickCount();
		if ( dwCurrTime >= dwStartTime )
		{
			dwElapsedTime += dwCurrTime - dwStartTime;
		}
		else
		{
			dwElapsedTime += 0xFFFFFFFF - dwStartTime;
			dwElapsedTime += dwCurrTime;
		}

		dwStartTime = dwCurrTime;

		if ( dwElapsedTime >= dwMilliseconds)
			return FALSE;

		Sleep(50);
	}

	return TRUE;
}


CCardIssuer* CCardIssuer::FromHandle(ISSUERHANDLE hIssuer)
{
	if ( _objmgr.IsExist( (void*) hIssuer ))
		return (CCardIssuer*) hIssuer;

	return NULL;
}