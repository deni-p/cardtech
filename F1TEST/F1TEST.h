
// F1TEST.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CF1TESTApp:
// �йش����ʵ�֣������ F1TEST.cpp
//

class CF1TESTApp : public CWinApp
{
public:
	CF1TESTApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CF1TESTApp theApp;
extern ISSUERHANDLE g_hIssuer;
extern CRichEditCtrl *g_pOutWnd;


void	LoadString(UINT nID, CString& strMsg);
void	GetMessage(LONG lResult, CString& strMsg);

void	OutputMessage( LPCTSTR pszMsg, COLORREF clrText = RGB(0,0,0) );
void	OutputResult( LONG lResult, LPCTSTR lpszCmdName, LPCTSTR lpszReplyInfo );
void	OutputResult( LONG lResult, CWnd *pCmdWnd, LPCTSTR lpszReplyInfo );