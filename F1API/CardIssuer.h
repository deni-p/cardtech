#pragma once
class CCardIssuer
{
public:
	CCardIssuer();
	~CCardIssuer();

	LONG	Connect(DWORD dwPort, DWORD dwBaudRate, BYTE bAddress);
	LONG	Disconnect();

	LONG	ExecCommand(BYTE bCommandCode, BYTE bParameterCode1, BYTE bParameterCode2, PBYTE pbRecvBuff, PDWORD pcbRecvLength);
	LONG	ExecCommand(BYTE bCommandCode, BYTE bParameterCode, PBYTE pbBodyData, BYTE cbBodyLength, PBYTE pbRecvBuff, PDWORD pcbRecvLength);

	__forceinline HANDLE GetCommHandle() const { return m_hDevice; }

	__forceinline void SetAddress(BYTE bAddress) { m_bAddress = bAddress; }
	__forceinline BYTE GetAddress() { return m_bAddress; }

	static	CCardIssuer* FromHandle(ISSUERHANDLE hIssuer);

private:
	LONG	TransmissionControl(BYTE bCommandCode, BYTE bParameterCode, PBYTE pbRecvBuff, PDWORD pcbRecvLength);

	BOOL	SendData(PBYTE pbBuffer, DWORD dwBytesToWrite, PBOOL pbTimeOut);
	BOOL	RecvData(PBYTE pbBuffer, DWORD dwBytesToRead, PBOOL pbTimeOut);

	LONG	InitPort(DWORD dwPort, DWORD dwBaudRate);
	void	MakeMessage(BYTE bCommandCode, BYTE bParameterCode, PBYTE pbBodyData, BYTE cbBodyLength);

	BOOL	WaitForBytesAvailable(DWORD dwMilliseconds);

private:
	enum { MAX_MSG_LENGTH = 300 };

	CRITICAL_SECTION m_csLock;

	HANDLE	m_hDevice;
	BYTE	m_bAddress;
	BYTE	m_bBuffer[ MAX_MSG_LENGTH];
	DWORD	m_cBuffer;
};

