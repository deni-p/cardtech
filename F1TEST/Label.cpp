// Label.cpp : 实现文件
//

#include "stdafx.h"
#include "Label.h"


// CLabel

IMPLEMENT_DYNAMIC(CLabel, CStatic)

CLabel::CLabel()
{
	m_brBack = NULL;
	m_clrText = RGB(0,0,0);
}

CLabel::~CLabel()
{
}


BEGIN_MESSAGE_MAP(CLabel, CStatic)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()



void CLabel::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类

	CStatic::PreSubclassWindow();
}


BOOL CLabel::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CStatic::PreCreateWindow(cs);
}

// CLabel 消息处理程序


HBRUSH CLabel::CtlColor(CDC *pDC, UINT /*nCtlColor*/)
{
	if (m_brBack)
	{
		pDC->SetBkMode(TRANSPARENT);   
		pDC->SetTextColor(m_clrText);
		return m_brBack;
	}

	return NULL;
}

void CLabel::SetTextColor(COLORREF clr)
{
	m_clrText = clr;
}

void CLabel::SetBackgroundColor(COLORREF clr)
{
    if (m_brBack)   
        ::DeleteObject(m_brBack);

    m_brBack = ::CreateSolidBrush(clr);   
   
    Invalidate();   
}