// Label.cpp : ʵ���ļ�
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
	// TODO: �ڴ����ר�ô����/����û���

	CStatic::PreSubclassWindow();
}


BOOL CLabel::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ����ר�ô����/����û���

	return CStatic::PreCreateWindow(cs);
}

// CLabel ��Ϣ�������


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