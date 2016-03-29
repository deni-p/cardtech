#pragma once


// CLabel

class CLabel : public CStatic
{
	DECLARE_DYNAMIC(CLabel)

public:
	CLabel();
	virtual ~CLabel();

	void	SetTextColor(COLORREF clr);
	void	SetBackgroundColor(COLORREF clr);

protected:
	virtual void PreSubclassWindow();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	COLORREF m_clrText;
	HBRUSH m_brBack;

	DECLARE_MESSAGE_MAP()
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
};


