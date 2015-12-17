
// 6000DemoDlg.h : 头文件
//
#include "Client.h"

#include "Page0.h"
#include "Page1.h"
#include "Page2.h"
#include "Page3.h"
#include "LogDlg.h"
#pragma once

#define MAX_VO 49
// CMy6000DemoDlg 对话框
class CMy6000DemoDlg : public CDialogEx
{
public:
	Client m_cClient;
	CPage0 m_tpage0;
	CPage1 m_tpage1;
	CPage2 m_tpage2;
	CPage3 m_tpage3;
	CLogDlg m_tLogDlg;
// 构造
public:
	CMy6000DemoDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MY6000DEMO_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	// ip地址

	char m_cstrIP[16];
	int m_iPort;
	
	int m_iPicCnt;      //及画面
	int m_iChnSelect;   //当前选中通道
	int m_iPicSelect;
	TChnvo m_cVideoVo[MAX_VO];
	
public:
	afx_msg void OnBnClickedLogon();
	afx_msg void OnEnChangePortedit();
	afx_msg void OnEnChangeIpedit();
	afx_msg void OnStnClickedVersionstatic();
	afx_msg void OnBnClickedButton1();
	
	afx_msg void OnCbnSelchangeChnnumcombo();
	
	
	void VideoVoVoctrlRed(UINT _iID, bool _blFlg);
	
	afx_msg void OnStnClickedVo(UINT _iID);

	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnCbnSelchangePiccombo();
	
	int Findafreevo();
	afx_msg void OnBnClickedConnect();


	afx_msg LRESULT OnMyRefreshChnCom(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMyRefreshLogItem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMyRefreshVersion(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMyRefreshChnPara(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnCbnSelchangeCombonetmode();
	afx_msg void OnClose();
public:
	CTabCtrl m_tTab;
	
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedDisconnect();
	afx_msg void OnBnClickedLogbutton();
};
