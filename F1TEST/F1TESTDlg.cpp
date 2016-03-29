
// F1TESTDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "F1TEST.h"
#include "F1TESTDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_DUMPSTATUSINFO	(WM_USER + 10)
#define WM_UPDATESTATINFO	(WM_USER +11)
#define WM_TESTTHREADABORT	(WM_USER + 12)


// CF1TESTDlg 对话框


CF1TESTDlg::CF1TESTDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CF1TESTDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pThreads[0] = m_pThreads[1] = NULL;
	m_hStopEvents[0] = m_hStopEvents[1] = NULL;

	m_nDispensedNumber = 0;
	m_nCapturedNumber = 0;
}

void CF1TESTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PORT_LIST, m_PortList);
	DDX_Control(pDX, IDC_BAUD_LIST, m_BaudList);
	DDX_Control(pDX, IDC_ENTRY_MODE_LIST, m_EntryModeList);
	DDX_Control(pDX, IDC_TEST_PLAN_LIST, m_PlanList);
	DDX_Control(pDX, IDC_ADDRESS_LIST, m_AddressList);

	DDX_Control(pDX, IDC_DISPENSED_NUM_BOX, m_DispensedNumLab);
	DDX_Control(pDX, IDC_CAPTURED_NUM_BOX, m_CapturedNumLab);
}

BEGIN_MESSAGE_MAP(CF1TESTDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_RESET, &CF1TESTDlg::OnBnClickedReset)
	ON_BN_CLICKED(IDC_EJECT_TO_FRONT, &CF1TESTDlg::OnBnClickedEjectToFront)
	ON_BN_CLICKED(IDC_RETURN_TO_FRONT, &CF1TESTDlg::OnBnClickedReturnToFront)
	ON_BN_CLICKED(IDC_RETURN_TO_READ_POS, &CF1TESTDlg::OnBnClickedReturnToReadPos)
	ON_BN_CLICKED(IDC_CAPTURE, &CF1TESTDlg::OnBnClickedCapture)
	ON_CBN_SELCHANGE(IDC_ENTRY_MODE_LIST, &CF1TESTDlg::OnCbnSelchangeEntryModeList)
	ON_CBN_SELCHANGE(IDC_BAUD_LIST, &CF1TESTDlg::OnCbnSelchangeBaudList)
	ON_BN_CLICKED(IDC_GET_ENTRY_MODE, &CF1TESTDlg::OnBnClickedGetEntryMode)
	ON_BN_CLICKED(IDC_START_TEST, &CF1TESTDlg::OnBnClickedStartTest)
	ON_BN_CLICKED(IDC_STOP_TEST, &CF1TESTDlg::OnBnClickedStopTest)
	ON_BN_CLICKED(IDC_CLEAR_STAT_INFO, &CF1TESTDlg::OnBnClickedClearStatInfo)
	ON_BN_CLICKED(IDC_CONNECT, &CF1TESTDlg::OnBnClickedConnect)
	ON_BN_CLICKED(IDC_DISCONNECT, &CF1TESTDlg::OnBnClickedDisconnect)
	ON_BN_CLICKED(IDC_CLEAR_OUTPUT, &CF1TESTDlg::OnBnClickedClearOutput)
	ON_CBN_SELCHANGE(IDC_ADDRESS_LIST, &CF1TESTDlg::OnCbnSelchangeAddressList)

	ON_MESSAGE(WM_DUMPSTATUSINFO, &CF1TESTDlg::DumpStatusInfo)
	ON_MESSAGE(WM_UPDATESTATINFO, &CF1TESTDlg::UpdateStatInfo)
	ON_MESSAGE(WM_TESTTHREADABORT, &CF1TESTDlg::OnTestThreadAbort)
END_MESSAGE_MAP()


// CF1TESTDlg 消息处理程序

BOOL CF1TESTDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	g_pOutWnd = (CRichEditCtrl*) GetDlgItem(IDC_OUTPUT_BOX);
	SetWindowText(afxCurrentAppName);

	EnableUI(FALSE);

	InitPortList();
	InitBaudList();
	InitAddressList();
	InitEntryModeList();
	InitPlanList();

	m_DispensedNumLab.SetBackgroundColor( RGB(255,255,255));
	m_DispensedNumLab.SetTextColor( RGB(0,0,0));

	m_CapturedNumLab.SetBackgroundColor( RGB(255,255,255));
	m_CapturedNumLab.SetTextColor( RGB(0,0,0));

	CWnd *p = (CEdit*) GetDlgItem(IDC_STATUS_BOX);
	CFont *pFont = p->GetFont();
	if (pFont != NULL)
	{
		LOGFONT lfont;
		pFont->GetLogFont(&lfont);
		lfont.lfWeight = FW_BOLD;
		lfont.lfHeight = 20;
		m_BoldFont.CreateFontIndirect(&lfont);
		p->SetFont(&m_BoldFont);
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CF1TESTDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CF1TESTDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CF1TESTDlg::OnBnClickedReset()
{
	LONG lResult = F1_Reset(g_hIssuer);
	OutputResult(lResult, GetFocus(), NULL);
}


void CF1TESTDlg::OnBnClickedEjectToFront()
{
	LONG lResult = F1_Dispense(g_hIssuer, DISP_EJECT_TO_FRONT);
	OutputResult(lResult, GetFocus(), NULL);
}


void CF1TESTDlg::OnBnClickedReturnToFront()
{
	LONG lResult = F1_Dispense(g_hIssuer, DISP_RETURN_TO_FRONT);
	OutputResult(lResult, GetFocus(), NULL);
}


void CF1TESTDlg::OnBnClickedReturnToReadPos()
{
	LONG lResult = F1_Dispense(g_hIssuer, DISP_RETURN_TO_READ_POS);
	OutputResult(lResult, GetFocus(), NULL);
}


void CF1TESTDlg::OnBnClickedCapture()
{
	LONG lResult = F1_Capture(g_hIssuer);
	OutputResult(lResult, GetFocus(), NULL);
}


void CF1TESTDlg::OnCbnSelchangeEntryModeList()
{
	CString strCmdName;
	LoadString(IDS_SET_ENTRY_MODE, strCmdName);

	BYTE bMode = (BYTE) m_EntryModeList.GetItemData( m_EntryModeList.GetCurSel() );
	LONG lResult = F1_SetEntryMode(g_hIssuer, bMode);
	OutputResult(lResult, strCmdName, NULL);

}


void CF1TESTDlg::OnCbnSelchangeBaudList()
{
	if ( g_hIssuer != 0)
	{
		CString strCmdName;
		LoadString(IDS_SET_BAUD_RATE, strCmdName);

		DWORD dwBaudRate = m_BaudList.GetItemData( m_BaudList.GetCurSel() );
		LONG lResult = F1_SetBaudRate(g_hIssuer, dwBaudRate);
		if ( lResult == 0 )
		{
			OnBnClickedDisconnect();
			Sleep(500);
			OnBnClickedConnect();
		}

		OutputResult(lResult, strCmdName, NULL);
	}
}



void CF1TESTDlg::OnCbnSelchangeAddressList()
{
	if ( g_hIssuer != 0)
	{
		CString strCmdName;
		LoadString(IDS_SET_COMM_ADDR, strCmdName);

		BYTE bAddr = (BYTE) m_AddressList.GetItemData( m_AddressList.GetCurSel() );
		LONG lResult = F1_SetCommAddress(g_hIssuer, bAddr);
		if ( lResult == 0)
		{
			DWORD PortNumber = m_PortList.GetItemData( m_PortList.GetCurSel() );
			DWORD BaudRate = m_BaudList.GetItemData( m_BaudList.GetCurSel() );

			CString strTitle;
			CString strConnInfo;
			CString strFormat;

			LoadString(IDS_CONNINFO_FORMAT, strFormat);
			strConnInfo.Format( strFormat, PortNumber, BaudRate,bAddr);

			strTitle.Format(_T("%s  %s"), afxCurrentAppName, strConnInfo);
			SetWindowText(strTitle);
		}

		OutputResult(lResult, strCmdName, NULL);
	}
}


void CF1TESTDlg::OnBnClickedGetEntryMode()
{
	BYTE bMode;
	LONG lResult = F1_GetEntryMode(g_hIssuer, &bMode);

	CString strMode;
	if ( lResult == 0 )
	{
		switch ( bMode )
		{
		case EM_PROHIBITED:		LoadString(IDS_PROHIBITED, strMode); break;
		case EM_CAPTURE_TO_BOX	:	LoadString(IDS_CAPTURE_TO_BOX, strMode); break;
		case EM_DOCKED_TO_READ_POS:	LoadString(IDS_DOCKED_TO_READ_POS, strMode); break;
		}
	}

	OutputResult(lResult, GetFocus(), strMode);
}


void CF1TESTDlg::OnBnClickedStartTest()
{
	m_pThreads[1] = AfxBeginThread(&AutoTestProc,
		this, THREAD_PRIORITY_NORMAL, CREATE_SUSPENDED, NULL);
	if (m_pThreads[1]  != NULL)
	{
		m_pThreads[1] ->m_bAutoDelete = FALSE;
		m_hStopEvents[1] = CreateEvent(NULL, TRUE, FALSE, NULL);


		GetDlgItem(IDC_START_TEST)->EnableWindow( FALSE);
		m_PlanList.EnableWindow( FALSE );

		m_pThreads[1] ->ResumeThread();
	}
	else
	{
		AfxMessageBox(_T("创建线程失败!"), MB_OK | MB_ICONERROR);
	}
}


void CF1TESTDlg::OnBnClickedStopTest()
{
	if ( m_pThreads[1] != NULL )
	{
		SetEvent(m_hStopEvents[1]);
		WaitForSingleObject(m_pThreads[1]->m_hThread, INFINITE);
		CloseHandle(m_hStopEvents[1]);
		delete m_pThreads[1];
		m_pThreads[1] = NULL;

		GetDlgItem(IDC_START_TEST)->EnableWindow( TRUE);
		m_PlanList.EnableWindow( TRUE );
	}
}


void CF1TESTDlg::OnBnClickedClearStatInfo()
{
	SetDlgItemInt( IDC_DISPENSED_NUM_BOX, m_nDispensedNumber = 0);
	SetDlgItemInt( IDC_CAPTURED_NUM_BOX, m_nCapturedNumber = 0 );
}


void CF1TESTDlg::OnBnClickedConnect()
{
	DWORD PortNumber = m_PortList.GetItemData( m_PortList.GetCurSel() );
	DWORD BaudRate = m_BaudList.GetItemData( m_BaudList.GetCurSel() );
	BYTE CommAddr = (BYTE) m_AddressList.GetItemData( m_AddressList.GetCurSel() );

	LONG lResult = F1_Connect(PortNumber, BaudRate, CommAddr, &g_hIssuer);
	if ( lResult != 0 )
	{
		BOOL bFound = FALSE;
		for (int i = 0; i < m_PortList.GetCount() && !bFound; i++)
		{
			PortNumber = m_PortList.GetItemData(i);
			for (int j = 0; j < m_BaudList.GetCount() && !bFound; j++)
			{
				BaudRate = m_BaudList.GetItemData(j);

				if ( F1_Connect(PortNumber, BaudRate, CommAddr, &g_hIssuer) == 0)
				{
					bFound = TRUE;
					m_PortList.SetCurSel(i);
					m_BaudList.SetCurSel(j);
					break;
				}
			}
		}

		if ( bFound )
			goto L2;

		CString strMsg;
		GetMessage( lResult, strMsg);
		AfxMessageBox( strMsg , MB_OK | MB_ICONERROR );
	}
	else
	{
L2:

		CString strTitle;
		CString strConnInfo;
		CString strFormat;

		LoadString(IDS_CONNINFO_FORMAT, strFormat);
		strConnInfo.Format( strFormat, PortNumber, BaudRate,CommAddr);

		strTitle.Format(_T("%s  %s"), afxCurrentAppName, strConnInfo);
		SetWindowText(strTitle);

		EnableUI( TRUE );

		m_pThreads[0] = AfxBeginThread(&StatusProc,
			this, THREAD_PRIORITY_NORMAL, CREATE_SUSPENDED, NULL);
		if (m_pThreads[0]  != NULL)
		{
			m_pThreads[0] ->m_bAutoDelete = FALSE;
			m_hStopEvents[0] = CreateEvent(NULL, TRUE, FALSE, NULL);
			m_pThreads[0] ->ResumeThread();
		}
		else
		{
			AfxMessageBox(_T("创建线程失败!"), MB_OK | MB_ICONERROR);
		}
	}
}


void CF1TESTDlg::OnBnClickedDisconnect()
{
	if ( g_hIssuer == 0)
		return;

	OnBnClickedStopTest();

	if ( m_pThreads[0] != NULL )
	{
		SetEvent(m_hStopEvents[0]);
		WaitForSingleObject(m_pThreads[0]->m_hThread, INFINITE);
		CloseHandle(m_hStopEvents[0]);
		delete m_pThreads[0];
		m_pThreads[0] = NULL;
	}

	if ( m_pThreads[1] != NULL )
	{
		SetEvent(m_hStopEvents[1]);
		WaitForSingleObject(m_pThreads[1]->m_hThread, INFINITE);
		delete m_pThreads[1];
		m_pThreads[1] = NULL;

		CloseHandle(m_hStopEvents[1]);
		m_hStopEvents[1] = NULL;
	}

	SetDlgItemText(IDC_STATUS_BOX, _T(""));


	LONG lResult = F1_Disconnect(g_hIssuer);
	if ( lResult != 0 )
	{
		CString strMsg;
		GetMessage( lResult, strMsg);
		AfxMessageBox( strMsg , MB_OK | MB_ICONERROR );
	}
	else
	{
		EnableUI ( FALSE );
		g_hIssuer = 0;
	}
}


void CF1TESTDlg::OnBnClickedClearOutput()
{
	g_pOutWnd->SetWindowText( _T("" ));
}


void	CF1TESTDlg::InitPortList()
{
	HANDLE hComm;
	BOOL fOK = FALSE;
	TCHAR szBuff[64];
	int idx;

	m_PortList.ResetContent();

	for (int i = 1; i < 256; i++)
	{
		fOK = FALSE;
		_stprintf_s(szBuff, _T("\\\\.\\COM%d"), i);
		hComm = CreateFile(szBuff, 
			GENERIC_READ | GENERIC_WRITE, 
			0, 
			NULL, 
			OPEN_EXISTING, 
			0,
			NULL);
		if (hComm == INVALID_HANDLE_VALUE)
		{
			if (GetLastError() != ERROR_FILE_NOT_FOUND)
			{
				fOK = TRUE;
			}
		}
		else
		{
			fOK = TRUE;
			CloseHandle(hComm);
		}

		if (fOK)
		{
			_stprintf_s(szBuff, _T("COM%d"), i);
			idx = m_PortList.AddString(szBuff);
			m_PortList.SetItemData(idx, i);
		}
	}

	if (m_PortList.GetCount() == 0)
		GetDlgItem(IDC_CONNECT)->EnableWindow(FALSE);
	else
		m_PortList.SetCurSel(0);
}


void	CF1TESTDlg::InitBaudList()
{
	DWORD dwBauds[] = { 1200, 2400, 4800, 9600,19200,38400 };
	CString strText;
	int idx;
	int sel = 0;

	for (int i = 0; i < sizeof(dwBauds) / sizeof(DWORD); i++)
	{
		strText.Format(_T("%d"), dwBauds[i]);
		idx = m_BaudList.AddString(strText);
		m_BaudList.SetItemData(idx, dwBauds[i]);	
	}

	m_BaudList.SetCurSel(3);
}


void	CF1TESTDlg::InitEntryModeList()
{
	CString strText;
	int	iIndex;

	LoadString(IDS_PROHIBITED, strText);
	iIndex = m_EntryModeList.AddString(strText);
	m_EntryModeList.SetItemData(iIndex, EM_PROHIBITED);

	LoadString(IDS_CAPTURE_TO_BOX, strText);
	iIndex = m_EntryModeList.AddString(strText);
	m_EntryModeList.SetItemData(iIndex, EM_CAPTURE_TO_BOX);

	LoadString(IDS_DOCKED_TO_READ_POS, strText);
	iIndex = m_EntryModeList.AddString(strText);
	m_EntryModeList.SetItemData(iIndex, EM_DOCKED_TO_READ_POS);

	m_EntryModeList.SetCurSel(0);
}



void	CF1TESTDlg::InitPlanList()
{
	CString strText;
	LoadString(IDS_DISP_EJECT_TO_FRONT, strText);
	m_PlanList.AddString(strText);
	LoadString(IDS_DISP_AND_CAPTURE, strText);
	m_PlanList.AddString(strText);
	m_PlanList.SetCurSel(0);
}



void	CF1TESTDlg::InitAddressList()
{
	CString strText;
	int idx;

	for (int i = 0; i <= 15; i++)
	{
		strText.Format(_T("%02d"), i);
		idx = m_AddressList.AddString(strText);
		m_AddressList.SetItemData(idx, i);
	}


		idx = m_AddressList.AddString( _T("不使用") );
		m_AddressList.SetItemData(idx, 0xff);

	m_AddressList.SetCurSel(idx);
}


void	CF1TESTDlg::EnableUI(BOOL bConnected)
{
	GetDlgItem(IDC_RESET)->EnableWindow( bConnected );
	GetDlgItem(IDC_EJECT_TO_FRONT)->EnableWindow( bConnected );
	GetDlgItem(IDC_RETURN_TO_FRONT)->EnableWindow( bConnected );
	GetDlgItem(IDC_RETURN_TO_READ_POS)->EnableWindow( bConnected );
	GetDlgItem(IDC_CAPTURE)->EnableWindow( bConnected );
	GetDlgItem(IDC_GET_ENTRY_MODE)->EnableWindow( bConnected );
	GetDlgItem(IDC_START_TEST)->EnableWindow( bConnected );
	GetDlgItem(IDC_STOP_TEST)->EnableWindow( bConnected );
	GetDlgItem(IDC_CLEAR_STAT_INFO)->EnableWindow( bConnected );
	GetDlgItem(IDC_CONNECT)->EnableWindow( !bConnected );
	GetDlgItem(IDC_DISCONNECT)->EnableWindow( bConnected );

	m_PortList.EnableWindow( !bConnected );
	m_EntryModeList.EnableWindow( bConnected );
	m_PlanList.EnableWindow( bConnected );

}



#define ERROR_FLAGS	(STATUS_CAPTURE_BOX_FULL | \
			STATUS_CARD_BOX_EMPTY | \
			STATUS_DISPENSE_ERROR | \
			STATUS_CAPTURE_ERROR | \
			STATUS_CARD_OVERLAPPED | \
			STATUS_CARD_JAMMED)

#define CARDINFLAGS	(STATUS_S1_CARD_IN | STATUS_S2_CARD_IN | STATUS_S3_CARD_IN )

static	BOOL bError = FALSE;
static	BOOL bDispComplete = FALSE;
static	BOOL bCapComplete = FALSE;
static	DWORD LastStatus = 0;

UINT CF1TESTDlg::StatusProc(LPVOID pvParam)
{
	CF1TESTDlg* pDlg = (CF1TESTDlg*) pvParam;
	LONG lResult = 0;
	BOOL b1 = FALSE;
	BOOL b2 = FALSE;

	while (true)
	{
		if (WaitForSingleObject(pDlg->m_hStopEvents[0], 20) == WAIT_OBJECT_0)
			break;

		lResult = F1_GetStatus(g_hIssuer, &pDlg->m_dwStatusFlags);
		if ( lResult == 0)
		{
			if ( pDlg->m_dwStatusFlags & (STATUS_DISPENSING) && !b1 )
			{
				b1 = TRUE;
			}
			else if ( (pDlg->m_dwStatusFlags == 0 || pDlg->m_dwStatusFlags == STATUS_CARD_INSUFFICIENT || pDlg->m_dwStatusFlags == STATUS_CARD_BOX_EMPTY) && b1 )
			{
				bDispComplete = TRUE;
				b1 = FALSE;
			}

			if ( pDlg->m_dwStatusFlags & STATUS_CAPTURING && !b2 )
			{
				b2 = TRUE;
			}
			else if ( b2 )
			{
				bCapComplete = TRUE;
				b2 = FALSE;
			}


			if ( pDlg->m_dwStatusFlags & ERROR_FLAGS && !bError)
			{
				LastStatus =  pDlg->m_dwStatusFlags;
				bError = TRUE;
			}

			::PostMessage(pDlg->GetSafeHwnd(), WM_DUMPSTATUSINFO, NULL, NULL);
		}

	}

	return 0;
}



UINT CF1TESTDlg::AutoTestProc(LPVOID pvParam)
{
	enum { TT_START, TT_RETURN_TO_FRONT, TT_EJECT_TO_FRONT, TT_CAPTURE, TT_COUNT1, TT_COUNT2 };

	CF1TESTDlg* pDlg = (CF1TESTDlg*) pvParam;
	LONG lResult = 0;
	int  iSel = pDlg->m_PlanList.GetCurSel();
	UINT iState = TT_START;

	 bError = FALSE;
	 bDispComplete = FALSE;
	bCapComplete = FALSE;
	int oldNum = pDlg->m_nDispensedNumber;

	while (true)
	{
		if (WaitForSingleObject(pDlg->m_hStopEvents[1], 50) == WAIT_OBJECT_0)
			break;

		if ( bError) 
			goto L_EXIT;

		switch ( iState )
		{
		case TT_START:
			if ( pDlg->m_dwStatusFlags & CARDINFLAGS )
				break;
	
			if ( iSel == 0 )
				iState = TT_EJECT_TO_FRONT;
			else
				iState = TT_RETURN_TO_FRONT;
			break;

		case TT_RETURN_TO_FRONT:
			bDispComplete = FALSE;
			bError = FALSE;

		
			lResult = F1_Dispense(g_hIssuer, DISP_RETURN_TO_FRONT);
			if ( lResult != 0)
				goto L_EXIT;
			Sleep(1000);

			iState = TT_COUNT1;;
			break;

		case TT_EJECT_TO_FRONT:
			bDispComplete = FALSE;
			bError = FALSE;
						lResult = F1_Dispense(g_hIssuer, DISP_RETURN_TO_READ_POS);
			if (lResult != 0)
				goto L_EXIT;
				Sleep(500);
			lResult = F1_Dispense(g_hIssuer, DISP_EJECT_TO_FRONT);
			if ( lResult != 0 )
				goto L_EXIT;
			Sleep(1000);

			iState = TT_COUNT1;
			break;

			
		case TT_CAPTURE:
			bCapComplete = FALSE;
			bError = FALSE;
			lResult = F1_Capture(g_hIssuer);
			if ( lResult != 0 )
				goto L_EXIT;

			iState = TT_COUNT2;
			break;

		case TT_COUNT1:
			if ( !bDispComplete )
				break;

			pDlg->m_nDispensedNumber++;
			::PostMessage(pDlg->GetSafeHwnd(), WM_UPDATESTATINFO, NULL, NULL);
			iState = iSel == 0 ? TT_START : TT_CAPTURE;

			if ( bError )
				goto L_EXIT;

			break;

		case TT_COUNT2:
			if ( !bCapComplete )
				break;

			if ( bError )
				goto L_EXIT;

			pDlg->m_nCapturedNumber++;
			::PostMessage(pDlg->GetSafeHwnd(), WM_UPDATESTATINFO, NULL, NULL);
			iState = TT_START;
			break;
		}
	}

L_EXIT:
	if (pDlg->m_nDispensedNumber > oldNum && (LastStatus & STATUS_CARD_BOX_EMPTY))
	{
			pDlg->m_nDispensedNumber++;
			::PostMessage(pDlg->GetSafeHwnd(), WM_UPDATESTATINFO, NULL, NULL);
	}

	CString strMsg;
	if (lResult != 0)
	{
		GetMessage(lResult, strMsg);
		::MessageBox(NULL, strMsg, NULL, MB_OK | MB_ICONERROR | MB_TOPMOST);
	}
	else
	{
		if ( bError )
		{
		if ( LastStatus & STATUS_CAPTURE_BOX_FULL)
			LoadString(IDS_CAPTURE_BOX_FULL, strMsg);
		else if ( LastStatus & STATUS_DISPENSE_ERROR)
			LoadString(IDS_DISPENSE_ERROR, strMsg);
		else if ( LastStatus & STATUS_CAPTURE_ERROR )
			LoadString(IDS_CAPTURE_ERROR, strMsg);
		else if ( LastStatus & STATUS_CARD_OVERLAPPED)
			LoadString(IDS_CARD_OVERLAPPED, strMsg);
		else if ( LastStatus & STATUS_CARD_JAMMED) 
			LoadString(IDS_CARD_JAMMED, strMsg);
		else if ( LastStatus & STATUS_CARD_BOX_EMPTY)
			LoadString(IDS_CARD_BOX_EMPTY, strMsg);

		::MessageBox(NULL, strMsg, NULL, MB_OK | MB_ICONERROR | MB_TOPMOST);
		}

	}
	::PostMessage(pDlg->GetSafeHwnd(), WM_TESTTHREADABORT, NULL, NULL);

	return 0;
}


LRESULT CF1TESTDlg::OnTestThreadAbort(WPARAM wParam, LPARAM lParam)
{

	if ( m_pThreads[1] != NULL )
	{
		delete m_pThreads[1];
		m_pThreads[1] = NULL;

		CloseHandle(m_hStopEvents[1]);
		m_hStopEvents[1] = NULL;
	}

	GetDlgItem(IDC_START_TEST)->EnableWindow( TRUE);
	m_PlanList.EnableWindow( TRUE );

	return 0;
}


LRESULT CF1TESTDlg::DumpStatusInfo(WPARAM wParam, LPARAM lParam)
{
	CString strMsg, strTemp;

	CString strCardIn;
	LoadString(IDS_SS_CARD_IN, strCardIn);


	if ( m_dwStatusFlags & STATUS_CAPTURE_BOX_FULL )
	{
		LoadString(IDS_SS_CAPTURE_BOX_FULL, strTemp);
		strMsg += strTemp;
		strMsg += _T("\r\n");
	}

	if ( m_dwStatusFlags & STATUS_DISPENSING )
	{
		LoadString(IDS_SS_DISPENSING, strTemp);
		strMsg += strTemp;
		strMsg += _T("\r\n");
	}

	if ( m_dwStatusFlags & STATUS_CAPTURING )
	{
		LoadString(IDS_SS_CAPTURING, strTemp);
		strMsg += strTemp;
		strMsg += _T("\r\n");
	}

	if ( m_dwStatusFlags & STATUS_DISPENSE_ERROR )
	{
		LoadString(IDS_SS_DISPENSE_ERROR, strTemp);
		strMsg += strTemp;
		strMsg += _T("\r\n");
	}

	if ( m_dwStatusFlags & STATUS_CAPTURE_ERROR )
	{
		LoadString(IDS_SS_CAPTURE_ERROR, strTemp);
		strMsg += strTemp;
		strMsg += _T("\r\n");
	}

	if ( m_dwStatusFlags & STATUS_CARD_JAMMED )
	{
		LoadString(IDS_SS_CARD_JAMMED, strTemp);
		strMsg += strTemp;
		strMsg += _T("\r\n");
	}

	if ( m_dwStatusFlags & STATUS_CARD_INSUFFICIENT )
	{
		LoadString(IDS_SS_CARD_INSUFFICIENT, strTemp);
		strMsg += strTemp;
		strMsg += _T("\r\n");
	}

	if ( m_dwStatusFlags & STATUS_CARD_BOX_EMPTY )
	{
		LoadString(IDS_SS_CARD_BOX_EMPTY, strTemp);
		strMsg += strTemp;
		strMsg += _T("\r\n");
	}

	if ( m_dwStatusFlags & STATUS_S3_CARD_IN )
	{
		strTemp.Format(_T("S3: %s\r\n"), strCardIn);
		strMsg += strTemp;
	}

	if ( m_dwStatusFlags & STATUS_S2_CARD_IN )
	{
		strTemp.Format(_T("S2: %s\r\n"), strCardIn);
		strMsg += strTemp;
	}

	if ( m_dwStatusFlags & STATUS_S1_CARD_IN )
	{
		strTemp.Format(_T("S1: %s\r\n"), strCardIn);
		strMsg += strTemp;
	}

	SetDlgItemText(IDC_STATUS_BOX, strMsg);

	return 0;
}



LRESULT CF1TESTDlg::UpdateStatInfo(WPARAM wParam, LPARAM lParam)
{
	SetDlgItemInt( IDC_DISPENSED_NUM_BOX, m_nDispensedNumber );
	SetDlgItemInt( IDC_CAPTURED_NUM_BOX, m_nCapturedNumber );

	return 0;
}