
// F1TESTDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "Label.h"

// CF1TESTDlg 对话框
class CF1TESTDlg : public CDialogEx
{
// 构造
public:
	CF1TESTDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_F1TEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	void	InitPortList();
	void	InitBaudList();
	void	InitAddressList();
	void	InitEntryModeList();
	void	InitPlanList();

	void	EnableUI(BOOL bConnected);


	static UINT StatusProc(LPVOID pvParam);
	static UINT AutoTestProc(LPVOID pvParam);


// 实现
protected:
	HICON m_hIcon;
	CFont	m_BoldFont;
	CComboBox m_PortList;
	CComboBox m_BaudList;
	CComboBox m_EntryModeList;
	CComboBox m_PlanList;
	CComboBox m_AddressList;

	CLabel m_DispensedNumLab;
	CLabel m_CapturedNumLab;

	int	m_nDispensedNumber;
	int	m_nCapturedNumber;

	CWinThread* m_pThreads[2];
	HANDLE m_hStopEvents[2];
	DWORD	m_dwStatusFlags;


	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedReset();
	afx_msg void OnBnClickedEjectToFront();
	afx_msg void OnBnClickedReturnToFront();
	afx_msg void OnBnClickedReturnToReadPos();
	afx_msg void OnBnClickedCapture();
	afx_msg void OnCbnSelchangeEntryModeList();
	afx_msg void OnCbnSelchangeBaudList();
	afx_msg void OnCbnSelchangeAddressList();
	afx_msg void OnBnClickedGetEntryMode();
	afx_msg void OnBnClickedStartTest();
	afx_msg void OnBnClickedStopTest();
	afx_msg void OnBnClickedClearStatInfo();
	afx_msg void OnBnClickedConnect();
	afx_msg void OnBnClickedDisconnect();
	afx_msg void OnBnClickedClearOutput();


	
	afx_msg LRESULT DumpStatusInfo(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT UpdateStatInfo(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTestThreadAbort(WPARAM wParam, LPARAM lParam);
	
};
