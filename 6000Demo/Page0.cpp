// Page0.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "6000Demo.h"
#include "Page0.h"
#include "afxdialogex.h"
#include "com_fun.h"

// CPage0 �Ի���

IMPLEMENT_DYNAMIC(CPage0, CDialogEx)

CPage0::CPage0(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PAGE0, pParent)
{
	m_ibrightness = 0;
	m_iConstant = 0;
	m_iSaturation = 0;
	m_iHue = 0;
}

CPage0::~CPage0()
{
}

void CPage0::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Slider(pDX, IDC_SLIDERBRIGHTNESS, m_ibrightness);
	DDX_Slider(pDX, IDC_SLIDERCONSTRAST, m_iConstant);
	DDX_Slider(pDX, IDC_SLIDERSATURATION, m_iSaturation);
	DDX_Slider(pDX, IDC_SLIDERHUE, m_iHue);
}


BEGIN_MESSAGE_MAP(CPage0, CDialog)
	ON_WM_HSCROLL()
	ON_CBN_SELCHANGE(IDC_VS, &CPage0::OnCbnSelchangeVs)
	ON_CBN_SELCHANGE(IDC_FRAMERATE, &CPage0::OnCbnSelchangeFramerate)
	ON_CBN_SELCHANGE(IDC_IFRAMERATE, &CPage0::OnCbnSelchangeIframerate)
	ON_CBN_SELCHANGE(IDC_BITRATE, &CPage0::OnCbnSelchangeBitrate)
	ON_CBN_SELCHANGE(IDC_RATETYPE, &CPage0::OnCbnSelchangeRatetype)
	ON_CBN_SELCHANGE(IDC_VIDEOQUA, &CPage0::OnCbnSelchangeVideoqua)
END_MESSAGE_MAP()


// CPage0 ��Ϣ��������


void CPage0::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: �ڴ�������Ϣ������������/�����Ĭ��ֵ
	UpdateData(TRUE);
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
	if (pScrollBar->GetDlgCtrlID() == IDC_SLIDERBRIGHTNESS)
	{
		// nPos���Ǵ�ʱ���˵�λ�ã���ʾ�� CEdit �Ϳ����� 
		char temp[30];
		sprintf_s(temp, 30, "%d", nPos);
		
		//���� ��ȡ Slider ��ǰλ�� 
		sprintf_s(temp, 30, "%d", ((CSliderCtrl*)pScrollBar)->GetPos());

		GetDlgItem(IDC_STATICBRIGHTNESS)->SetWindowText(temp);


	}
	else if (pScrollBar->GetDlgCtrlID() == IDC_SLIDERSATURATION)
	{
		// nPos���Ǵ�ʱ���˵�λ�ã���ʾ�� CEdit �Ϳ����� 
		char temp[30];
		sprintf_s(temp, 30, "%d", nPos);

		//���� ��ȡ Slider ��ǰλ�� 
		sprintf_s(temp, 30, "%d", ((CSliderCtrl*)pScrollBar)->GetPos());

		GetDlgItem(IDC_STATICSATURATION)->SetWindowText(temp);
	}
	else if (pScrollBar->GetDlgCtrlID() == IDC_SLIDERCONSTRAST)
	{
		// nPos���Ǵ�ʱ���˵�λ�ã���ʾ�� CEdit �Ϳ����� 
		char temp[30];
		sprintf_s(temp, 30, "%d", nPos);

		//���� ��ȡ Slider ��ǰλ�� 
		sprintf_s(temp, 30, "%d", ((CSliderCtrl*)pScrollBar)->GetPos());

		GetDlgItem(IDC_STATICCONSTRAST)->SetWindowText(temp);
	}
	else if (pScrollBar->GetDlgCtrlID() == IDC_SLIDERHUE)
	{
		// nPos���Ǵ�ʱ���˵�λ�ã���ʾ�� CEdit �Ϳ����� 
		char temp[30];
		sprintf_s(temp, 30, "%d", nPos);

		//���� ��ȡ Slider ��ǰλ�� 
		sprintf_s(temp, 30, "%d", ((CSliderCtrl*)pScrollBar)->GetPos());

		GetDlgItem(IDC_STATICHUE)->SetWindowText(temp);
	}

	m_cClient->m_tVChn[m_iChn].SetChnVideopara(m_ibrightness, m_iConstant, m_iHue, m_iSaturation);
}



void CPage0::OnCbnSelchangeVs()
{
	int iVideosize = HD1080;
	CString Text;
	GetDlgItemText(IDC_VS, Text);
	
	if (strcmp(Text.GetBuffer(), "CIF") == 0)
	{
		iVideosize = CIF;
	}
	else if (strcmp(Text.GetBuffer(), "D1") == 0)
	{
		iVideosize = D1;

	}
	else if (strcmp(Text.GetBuffer(), "720P") == 0)
	{
		iVideosize = HD720;
	}
	else if (strcmp(Text.GetBuffer(), "960P") == 0)
	{
		iVideosize = HD960;
	}
	else if (strcmp(Text.GetBuffer(), "1080P") == 0)
	{
		iVideosize = HD1080;
	}
	else
	{
	}

	// TODO: �ڴ����ӿؼ�֪ͨ�����������
	if (m_cClient->m_icmdSocket && iVideosize != m_cClient->m_tVChn[m_iChn].m_tVideopara.m_iVideoSize)
	{
		m_cClient->m_tVChn[m_iChn].SetChnVideosize(iVideosize);
	}
}


void CPage0::OnCbnSelchangeFramerate()
{
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
	int iFramerate = 25;
	CString Text;
	GetDlgItemText(IDC_FRAMERATE, Text);
	iFramerate = atoi(Text.GetBuffer());
	if (m_cClient->m_icmdSocket && iFramerate != m_cClient->m_tVChn[m_iChn].m_tVideopara.m_iFrameRate)
	{
		m_cClient->m_tVChn[m_iChn].SetChnFramerate(iFramerate);
	}

}



void CPage0::OnCbnSelchangeIframerate()
{
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
	int iIFramerate = 25;
	CString Text;
	GetDlgItemText(IDC_IFRAMERATE, Text);
	iIFramerate = atoi(Text.GetBuffer());
	if (m_cClient->m_icmdSocket && iIFramerate != m_cClient->m_tVChn[m_iChn].m_tVideopara.m_iIFrameRate)
	{
		m_cClient->m_tVChn[m_iChn].SetChnIframerate(iIFramerate);
	}
}


void CPage0::OnCbnSelchangeBitrate()
{
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
	int iBitrate = 4096;
	CString Text;
	GetDlgItemText(IDC_BITRATE, Text);
	iBitrate = atoi(Text.GetBuffer());
	if (m_cClient->m_icmdSocket && iBitrate != m_cClient->m_tVChn[m_iChn].m_tVideopara.m_iBitRate)
	{
		m_cClient->m_tVChn[m_iChn].SetChnBitrate(iBitrate);
	}
}



void CPage0::OnCbnSelchangeRatetype()
{
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
	CComboBox *Cbx = (CComboBox *)GetDlgItem(IDC_RATETYPE);

	int iEncodeType = Cbx->GetCurSel();
	if (m_cClient->m_icmdSocket && iEncodeType != m_cClient->m_tVChn[m_iChn].m_tVideopara.m_iEncodeType)
	{
		m_cClient->m_tVChn[m_iChn].SetChnEncodetype(iEncodeType);
	}
}



void CPage0::OnCbnSelchangeVideoqua()
{
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
	CComboBox *Cbx = (CComboBox *)GetDlgItem(IDC_VIDEOQUA);

	int iVideoQua = Cbx->GetCurSel()*2 + 6;
	if (m_cClient->m_icmdSocket && iVideoQua != m_cClient->m_tVChn[m_iChn].m_tVideopara.m_iVideoQuality)
	{
		m_cClient->m_tVChn[m_iChn].SetChnVideoquality(iVideoQua);
	}
}
void CPage0::RefreshChnPara()
{
	CComboBox *cbx = (CComboBox *)GetDlgItem(IDC_VS);
	TRACE("m_cClient.m_tVChn[%d].m_iVideoSize = %d \n", m_iChn, m_cClient->m_tVChn[m_iChn].m_tVideopara.m_iVideoSize);
	SetVideoSizeCombobox(cbx, m_cClient->m_tVChn[m_iChn].m_tVideopara.m_iVideoSize);
	cbx = (CComboBox *)GetDlgItem(IDC_FRAMERATE);
	TRACE("m_cClient.m_tVChn[%d].m_iFrameRate = %d \n", m_iChn, m_cClient->m_tVChn[m_iChn].m_tVideopara.m_iFrameRate);
	SetComboboxInt(cbx, m_cClient->m_tVChn[m_iChn].m_tVideopara.m_iFrameRate);
	cbx = (CComboBox *)GetDlgItem(IDC_IFRAMERATE);
	TRACE("m_cClient.m_tVChn[%d].m_iIFrameRate = %d \n", m_iChn, m_cClient->m_tVChn[m_iChn].m_tVideopara.m_iIFrameRate);
	SetComboboxInt(cbx, m_cClient->m_tVChn[m_iChn].m_tVideopara.m_iIFrameRate);
	cbx = (CComboBox *)GetDlgItem(IDC_BITRATE);
	TRACE("m_cClient.m_tVChn[%d].m_iBitrate = %d \n", m_iChn, m_cClient->m_tVChn[m_iChn].m_tVideopara.m_iBitRate);
	SetComboboxInt(cbx, m_cClient->m_tVChn[m_iChn].m_tVideopara.m_iBitRate);
	cbx = (CComboBox *)GetDlgItem(IDC_RATETYPE);
	TRACE("m_cClient.m_tVChn[%d].m_iEncodeType = %d \n", m_iChn, m_cClient->m_tVChn[m_iChn].m_tVideopara.m_iEncodeType);
	cbx->SetCurSel(m_cClient->m_tVChn[m_iChn].m_tVideopara.m_iEncodeType);
	cbx = (CComboBox *)GetDlgItem(IDC_VIDEOQUA);
	TRACE("m_cClient.m_tVChn[%d].m_iVideoQuality = %d \n", m_iChn, m_cClient->m_tVChn[m_iChn].m_tVideopara.m_iVideoQuality);
	cbx->SetCurSel((m_cClient->m_tVChn[m_iChn].m_tVideopara.m_iVideoQuality - 6) / 2);
	cbx = (CComboBox *)GetDlgItem(IDC_AUDIOSAMPLE);
	SetComboboxInt(cbx, m_cClient->m_tVChn[m_iChn].m_tVideopara.m_iAudioSample);


	char temp[30];


	//���� ��ȡ Slider ��ǰλ�� 
	m_ibrightness = m_cClient->m_tVChn[m_iChn].m_tVideopara.m_ViCnf.m_iLum;
	m_iConstant = m_cClient->m_tVChn[m_iChn].m_tVideopara.m_ViCnf.m_iContrast;
	m_iHue = m_cClient->m_tVChn[m_iChn].m_tVideopara.m_ViCnf.m_iHue;
	m_iSaturation = m_cClient->m_tVChn[m_iChn].m_tVideopara.m_ViCnf.m_iSaturation;
	UpdateData(FALSE);
	CStatic *Cst = (CStatic *)GetDlgItem(IDC_STATICBRIGHTNESS);


	sprintf_s(temp, 30, "%d", m_ibrightness);
	Cst->SetWindowText(temp);


	Cst = (CStatic *)GetDlgItem(IDC_STATICSATURATION);
	sprintf_s(temp, 30, "%d", m_iSaturation);
	Cst->SetWindowText(temp);



	Cst = (CStatic *)GetDlgItem(IDC_STATICCONSTRAST);
	sprintf_s(temp, 30, "%d", m_iConstant);
	Cst->SetWindowText(temp);



	Cst = (CStatic *)GetDlgItem(IDC_STATICHUE);
	sprintf_s(temp, 30, "%d", m_iHue);
	Cst->SetWindowText(temp);

}