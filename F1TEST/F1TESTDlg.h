
// F1TESTDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "Label.h"

// CF1TESTDlg �Ի���
class CF1TESTDlg : public CDialogEx
{
// ����
public:
	CF1TESTDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_F1TEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

	void	InitPortList();
	void	InitBaudList();
	void	InitAddressList();
	void	InitEntryModeList();
	void	InitPlanList();

	void	EnableUI(BOOL bConnected);


	static UINT StatusProc(LPVOID pvParam);
	static UINT AutoTestProc(LPVOID pvParam);


// ʵ��
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


	// ���ɵ���Ϣӳ�亯��
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
