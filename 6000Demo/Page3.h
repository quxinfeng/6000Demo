#pragma once

#include "Client.h"
// CPage3 �Ի���

class CPage3 : public CDialogEx
{
	DECLARE_DYNAMIC(CPage3)

public:
	CPage3(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPage3();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PAGE3 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	Client *m_cClient;
	int m_iChn;
	BOOL m_iDisplay;
	int m_iOSDType;
	int m_iPosX;
	int m_iPosY;
	CString m_cOsdText;
public:
	void RefreshChnPara();
};