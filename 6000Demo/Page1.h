#pragma once
#include "Client.h"

// CPage1 �Ի���

class CPage1 : public CDialogEx
{
	DECLARE_DYNAMIC(CPage1)

public:
	CPage1(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPage1();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PAGE1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	Client *m_cClient;
	afx_msg void OnCbnSelchangeCombopic1();
	afx_msg void OnBnClickedBtnpreset();
	afx_msg void OnCbnSelchangeCombovodev();
	afx_msg void OnCbnSelchangeCombovosize();
	afx_msg void OnCbnSelchangeCombopreviewnum();
};
