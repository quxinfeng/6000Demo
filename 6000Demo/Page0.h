#pragma once
#include "Client.h"

// CPage0 对话框

class CPage0 : public CDialogEx
{
	DECLARE_DYNAMIC(CPage0)

public:
	CPage0(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPage0();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

public:
	Client *m_cClient;
	int m_iChn;
	int m_ibrightness;
	int m_iConstant;
	int m_iSaturation;
	int m_iHue;

	BOOL m_iDisplay;
	int m_iOSDType;
	int m_iPosX;
	int m_iPosY;
	char strText[256];
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	void RefreshChnPara();
	void RefreshOsdPara();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnCbnSelchangeVs();
	afx_msg void OnCbnSelchangeFramerate();
	afx_msg void OnCbnSelchangeIframerate();
	afx_msg void OnCbnSelchangeBitrate();
	afx_msg void OnCbnSelchangeRatetype();
	afx_msg void OnCbnSelchangeVideoqua();

	afx_msg void OnCbnSelchangeOsdtype();

	afx_msg void OnBnClickedButtonapply();
	afx_msg void OnBnClickedCheckosddisplay();
	afx_msg void OnBnClickedButtonapplypos();
	afx_msg void OnCbnSelchangeComboosdcolor();
};
