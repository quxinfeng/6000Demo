// Page1.cpp : 实现文件
//

#include "stdafx.h"
#include "6000Demo.h"
#include "Page1.h"
#include "afxdialogex.h"
#include "com_fun.h"

// CPage1 对话框

IMPLEMENT_DYNAMIC(CPage1, CDialogEx)

CPage1::CPage1(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PAGE1, pParent)
{

}

CPage1::~CPage1()
{
}

void CPage1::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPage1, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBOPIC1, &CPage1::OnCbnSelchangeCombopic1)
	ON_BN_CLICKED(IDC_BTNPRESET, &CPage1::OnBnClickedBtnpreset)
	ON_CBN_SELCHANGE(IDC_COMBOVODEV, &CPage1::OnCbnSelchangeCombovodev)
	ON_CBN_SELCHANGE(IDC_COMBOVOSIZE, &CPage1::OnCbnSelchangeCombovosize)
	ON_CBN_SELCHANGE(IDC_COMBOPREVIEWNUM, &CPage1::OnCbnSelchangeCombopreviewnum)
END_MESSAGE_MAP()


// CPage1 消息处理程序


void CPage1::OnCbnSelchangeCombopic1()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CPage1::OnBnClickedBtnpreset()
{
	// TODO: 在此添加控件通知处理程序代码
	CComboBox* Cbx = NULL;
	Cbx = (CComboBox *)GetDlgItem(IDC_COMBOVODEV);
	int iVodev = Cbx->GetCurSel();
	if (iVodev == 2)
	{
		iVodev = 0x10;
	}
	Cbx = (CComboBox *)GetDlgItem(IDC_COMBOPREVIEWNUM);
	int iPicNum = Cbx->GetCurSel() + 1;
	int i;
	int iChnlist[4] = { 0 };
	CString Text;
	for (i = 0; i < iPicNum; i++)
	{
		Cbx = (CComboBox *)GetDlgItem(IDC_COMBOPIC1 + i);
		GetDlgItemText(IDC_COMBOPIC1 + i, Text);
		iChnlist[i] = atoi(Text.GetBuffer());
	}
	if (m_cClient->m_iLoginstat == 0x8)
	{
		m_cClient->ClientSetPreviewpara(iVodev, iPicNum, iChnlist);
	}
}


void CPage1::OnCbnSelchangeCombovodev()
{
	// TODO: 在此添加控件通知处理程序代码
	int i;
	CComboBox *Cbx = (CComboBox *)GetDlgItem(IDC_COMBOVODEV);
	int iVodev = Cbx->GetCurSel();
	Cbx = (CComboBox *)GetDlgItem(IDC_COMBOVOSIZE);
	if (iVodev < 2)
	{
		Cbx->EnableWindow(true);
		SetVodevsize(Cbx, m_cClient->m_iVodevSize[iVodev]);
	}
	else
	{
		Cbx->EnableWindow(false);
	}

	Cbx = (CComboBox *)GetDlgItem(IDC_COMBOPREVIEWNUM);
	Cbx->SetCurSel(m_cClient->m_tVodev[iVodev].iPicCnt - 1);
	for (i = 0; i < 4; i++)
	{
		Cbx = (CComboBox *)GetDlgItem(IDC_COMBOPIC1 + i);
		Cbx->EnableWindow(FALSE);

	}
	for ( i = 0; i < m_cClient->m_tVodev[iVodev].iPicCnt; i++)
	{
		Cbx = (CComboBox *)GetDlgItem(IDC_COMBOPIC1 + i);
		Cbx->EnableWindow(true);
		Cbx->SetCurSel(m_cClient->m_tVodev[iVodev].iChnlist[i]);
	}

}


void CPage1::OnCbnSelchangeCombovosize()
{
	// TODO: 在此添加控件通知处理程序代码
	CComboBox *Cbx = (CComboBox *)GetDlgItem(IDC_COMBOVODEV);
	int iVodev = Cbx->GetCurSel();
	Cbx = (CComboBox *)GetDlgItem(IDC_COMBOVOSIZE);
	int iVodevSize = 0;
	GetVodevsize(Cbx, &iVodevSize);
	m_cClient->ClientSetVodevSize(iVodev, iVodevSize);
}


void CPage1::OnCbnSelchangeCombopreviewnum()
{
	// TODO: 在此添加控件通知处理程序代码
	int i;
	CComboBox *Cbx = (CComboBox *)GetDlgItem(IDC_COMBOPREVIEWNUM);
	int iPicCnt = Cbx->GetCurSel() + 1;;
	
	for (i = 0; i < 4; i++)
	{
		Cbx = (CComboBox *)GetDlgItem(IDC_COMBOPIC1 + i);
		Cbx->EnableWindow(FALSE);

	}
	for (i = 0; i < iPicCnt; i++)
	{
		Cbx = (CComboBox *)GetDlgItem(IDC_COMBOPIC1 + i);
		Cbx->EnableWindow(true);
	}

}
