
// F1TEST.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CF1TESTApp:
// 有关此类的实现，请参阅 F1TEST.cpp
//

class CF1TESTApp : public CWinApp
{
public:
	CF1TESTApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

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