#pragma once
#include "Client.h"

// CPage0 �Ի���

class CPage0 : public CDialogEx
{
	DECLARE_DYNAMIC(CPage0)

public:
	CPage0(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPage0();

// �Ի�������
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
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	void RefreshChnPara();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnCbnSelchangeVs();
	afx_msg void OnCbnSelchangeFramerate();
	afx_msg void OnCbnSelchangeIframerate();
	afx_msg void OnCbnSelchangeBitrate();
	afx_msg void OnCbnSelchangeRatetype();
	afx_msg void OnCbnSelchangeVideoqua();
};
