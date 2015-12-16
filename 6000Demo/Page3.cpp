// Page3.cpp : 实现文件
//

#include "stdafx.h"
#include "6000Demo.h"
#include "Page3.h"
#include "afxdialogex.h"


// CPage3 对话框

IMPLEMENT_DYNAMIC(CPage3, CDialogEx)

CPage3::CPage3(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PAGE3, pParent)
	, m_iDisplay(FALSE)
	, m_iOSDType(0)
	, m_iPosX(0)
	, m_iPosY(0)
	, m_cOsdText(_T(""))
{

}

CPage3::~CPage3()
{
}

void CPage3::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECKOSDDISPLAY, m_iDisplay);
	DDX_CBIndex(pDX, IDC_OSDTYPE, m_iOSDType);
	DDX_Text(pDX, IDC_EDITPOSX, m_iPosX);
	DDV_MinMaxInt(pDX, m_iPosX, 0, 704);
	DDX_Text(pDX, IDC_EDITPOSY, m_iPosY);
	DDV_MinMaxInt(pDX, m_iPosY, 0, 576);
	DDX_Text(pDX, IDC_EDIT1, m_cOsdText);
	DDV_MaxChars(pDX, m_cOsdText, 256);
}


BEGIN_MESSAGE_MAP(CPage3, CDialogEx)
END_MESSAGE_MAP()


// CPage3 消息处理程序
void CPage3::RefreshChnPara()
{
	
	UpdateData(TRUE);
	m_iDisplay = m_cClient->m_tVChn[m_iChn].m_TOverLay[m_iOSDType].m_iDispaly;


}