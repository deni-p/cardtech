
// F1TEST.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "F1TEST.h"
#include "F1TESTDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CF1TESTApp

BEGIN_MESSAGE_MAP(CF1TESTApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CF1TESTApp ����

CF1TESTApp::CF1TESTApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CF1TESTApp ����

CF1TESTApp theApp;


// CF1TESTApp ��ʼ��

BOOL CF1TESTApp::InitInstance()
{
		free((void*)m_pszAppName);
	//Change the name of the application file.
	//The CWinApp destructor will free the memory.
	m_pszAppName=_tcsdup(_T("F1-1000 TEST 2.1"));
	afxCurrentAppName = m_pszAppName;


	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();
	AfxInitRichEdit2();

	CF1TESTDlg dlg;

	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}


 ISSUERHANDLE g_hIssuer = 0;
 CRichEditCtrl *g_pOutWnd = NULL;

 #define NAME_ALIGN	37
#define MAX_LINE	300

 void	LoadString(UINT nID, CString& strMsg)
 {
	 static TCHAR szBuff[128];
	 ::LoadString( NULL, nID, szBuff, sizeof(szBuff) / sizeof(TCHAR));
	 strMsg = szBuff;
 }


void	GetMessage(LONG lResult, CString& strMsg)
{
	switch ( lResult )
	{
	case F1_S_SUCCESS		: LoadString(IDS_SUCCESS, strMsg); break;
	case F1_E_DEVICE_UNRECOGNIZED	: LoadString(IDS_DEVICE_UNRECOGNIZED, strMsg); break;
	case F1_E_PORT_UNAVAILABLE	: LoadString(IDS_PORT_UNAVAILABLE, strMsg); break;
	case F1_E_UNKNOWN_ERROR		: LoadString(IDS_UNKNOWN_ERROR, strMsg); break;
	case F1_E_INTERNAL_ERROR	: LoadString(IDS_INTERNAL_ERROR, strMsg); break;
	case F1_E_COMM_TIMEOUT		: LoadString(IDS_COMM_TIMEOUT, strMsg); break;
	case F1_E_INVALID_HANDLE	: LoadString(IDS_INVALID_HANDLE, strMsg); break;
	case F1_E_INVALID_PARAMETER	: LoadString(IDS_INVALID_PARAMETER, strMsg); break;
	case F1_E_NO_MEMORY		: LoadString(IDS_NO_MEMORY, strMsg); break;
	case F1_E_BUFFER_TOO_SMALL	: LoadString(IDS_BUFFER_TOO_SMALL, strMsg); break;
	}
}


void	OutputMessage( LPCTSTR pszMsg, COLORREF clrText )
{
	if (g_pOutWnd == NULL)
		return;

	CHARFORMAT cFmt; 
	cFmt.cbSize = sizeof(CHARFORMAT);  
	cFmt.dwMask = CFM_COLOR | CFM_BOLD | CFM_SIZE;
	cFmt.dwEffects = CFE_BOLD;
	cFmt.crTextColor = clrText;
	cFmt.yHeight = 14*1440/96;

	g_pOutWnd->SetSel(-1, -1);
	g_pOutWnd->SetSelectionCharFormat(cFmt);
	g_pOutWnd->ReplaceSel(pszMsg);
	g_pOutWnd->PostMessage(WM_VSCROLL, SB_BOTTOM,0); 
	g_pOutWnd->SetFocus();

	if (g_pOutWnd->GetLineCount() > MAX_LINE)
	{
		long EndChar = g_pOutWnd->LineIndex(g_pOutWnd->GetLineCount() - MAX_LINE);
		g_pOutWnd->SetSel(0, EndChar);
		g_pOutWnd->ReplaceSel( NULL );

		g_pOutWnd->PostMessage(WM_VSCROLL, SB_BOTTOM,0); 
		g_pOutWnd->SetFocus();
	}
}


void	OutputResult( LONG lResult, LPCTSTR lpszCmdName, LPCTSTR lpszReplyInfo )
{
	if (g_pOutWnd == NULL)
		return;

	CString strErr;
	GetMessage(lResult, strErr);

	SYSTEMTIME st;
	GetLocalTime(&st);

	CString strTitle;
	strTitle.Format(_T("%02d:%02d:%02d:%03d  "), st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	strTitle += lpszCmdName;

	char  szTitle[128];
	int len = WideCharToMultiByte(CP_OEMCP,NULL, strTitle, -1, szTitle, sizeof(szTitle), NULL, FALSE);

	for (int i = 0; i < NAME_ALIGN - len; i++)
		strTitle.AppendChar( ' ');

	CString strMsg;
	if (lpszReplyInfo != NULL &&
		_tcslen(lpszReplyInfo) > 0)
	{
		strMsg.Format(_T("%s\t[%s]\r\n  [%s]\r\n\r\n"), strTitle, strErr, lpszReplyInfo);
	}
	else
	{
		strMsg.Format(_T("%s\t[%s]\r\n\r\n"), strTitle, strErr);
	}

	OutputMessage(strMsg, (lResult == 0) ? RGB(0,128,0) : RGB(255,0,0));
}


void	OutputResult( LONG lResult, CWnd *pCmdWnd, LPCTSTR lpszReplyInfo )
{
	CString strCmdName;
	pCmdWnd->GetWindowText(strCmdName);
	OutputResult(lResult, strCmdName, lpszReplyInfo);
}