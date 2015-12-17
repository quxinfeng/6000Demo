
// 6000DemoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "6000Demo.h"
#include "6000DemoDlg.h"
#include "afxdialogex.h"
#include "socket.h"
#include "com_fun.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()

};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_WM_HSCROLL()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CMy6000DemoDlg �Ի���



CMy6000DemoDlg::CMy6000DemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MY6000DEMO_DIALOG, pParent)
	, m_iPort(0)
	, m_iChnSelect(0)
	
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMy6000DemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tTab);
	
	
}

BEGIN_MESSAGE_MAP(CMy6000DemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_LOGON, &CMy6000DemoDlg::OnBnClickedLogon)
	ON_EN_CHANGE(IDC_PORTEDIT, &CMy6000DemoDlg::OnEnChangePortedit)
	ON_EN_CHANGE(IDC_IPEDIT, &CMy6000DemoDlg::OnEnChangeIpedit)
	ON_STN_CLICKED(IDC_VERSIONSTATIC, &CMy6000DemoDlg::OnStnClickedVersionstatic)
	
	ON_CBN_SELCHANGE(IDC_CHNNUMCOMBO, &CMy6000DemoDlg::OnCbnSelchangeChnnumcombo)
	ON_CBN_SELCHANGE(IDC_PICCOMBO, &CMy6000DemoDlg::OnCbnSelchangePiccombo)

	ON_CONTROL_RANGE(BN_CLICKED, IDC_VO0, IDC_VO48, OnStnClickedVo)
	
	ON_BN_CLICKED(IDC_CONNECT, &CMy6000DemoDlg::OnBnClickedConnect)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_COMBONETMODE, &CMy6000DemoDlg::OnCbnSelchangeCombonetmode)
	ON_WM_CLOSE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CMy6000DemoDlg::OnTcnSelchangeTab1)
	ON_BN_CLICKED(IDC_DISCONNECT, &CMy6000DemoDlg::OnBnClickedDisconnect)
	ON_BN_CLICKED(IDC_LOGBUTTON, &CMy6000DemoDlg::OnBnClickedLogbutton)
	//�Զ�����Ϣ
	ON_MESSAGE(WM_MY_REFRESHCHNCOM, OnMyRefreshChnCom)
	ON_MESSAGE(WM_MY_REFRESHVERSION, OnMyRefreshVersion)
	ON_MESSAGE(WM_MY_REFRESHCHNPARA, OnMyRefreshChnPara)
	ON_MESSAGE(WM_MY_REFRESHLOGITEM, OnMyRefreshLogItem)
END_MESSAGE_MAP()


// CMy6000DemoDlg ��Ϣ�������

BOOL CMy6000DemoDlg::OnInitDialog()
{
	int i = 0;
	CDialogEx::OnInitDialog();
	
	// ��������...���˵�����ӵ�ϵͳ�˵��С�
	//_CrtSetBreakAlloc(1117);
	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	HWND Hwnd = AfxGetMainWnd()->m_hWnd;
	
	m_cClient.m_cMainHwnd = Hwnd;
	Hwnd = m_tLogDlg.m_hWnd;
	m_cClient.m_cLogHwnd = Hwnd;
	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	SetDlgItemText(IDC_IPEDIT, "172.16.43.233");
	SetDlgItemInt(IDC_PORTEDIT, 3000);
	SetDlgItemText(IDC_USEREDIT, "Admin");
	SetDlgItemText(IDC_PASSWORDEDIT, "1111");
	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	CString strTemp;
	
		
	((CComboBox*)GetDlgItem(IDC_CHNNUMCOMBO))->ResetContent();//����������������

	GetDlgItem(IDC_CHNNUMCOMBO)->EnableWindow(false);
	
	m_iPicSelect = -1;
	m_iPicCnt = 1;
	
	for (i = 0; i < MAX_VO; i++)
	{
		m_cVideoVo[i].iIndex = i;
		m_cVideoVo[i].iChn = 0;
		
		m_cVideoVo[i].m_tVo = (CStatic *)GetDlgItem(IDC_VO0 + i);
		m_cVideoVo[i].m_tVo->ShowWindow(false);
		//	m_cVideoVo[i].m_cVo->mo

	}
	
	
	((CComboBox*)GetDlgItem(IDC_PICCOMBO))->SetCurSel(0);
	OnCbnSelchangePiccombo();
	


	CComboBox *Cbx = (CComboBox *)GetDlgItem(IDC_SUBCOMBO);
	
	
	Cbx->ResetContent();//����������������
	
	strTemp.Format("%s", "main");
	Cbx->AddString(strTemp);
	strTemp.Format("%s", "sub");
	Cbx->AddString(strTemp);
	Cbx->SetCurSel(0);
	Cbx->EnableWindow(true);
	Cbx = (CComboBox *)GetDlgItem(IDC_COMBONETMODE);

	Cbx->ResetContent();//����������������

	strTemp.Format("%s", "tcp");
	Cbx->AddString(strTemp);
	strTemp.Format("%s", "udp");
	Cbx->AddString(strTemp);
	Cbx->SetCurSel(0);
	Cbx->EnableWindow(true);
	
	m_tTab.InsertItem(0, "VIDEO");

	m_tTab.InsertItem(1, "PREVIEW");
	m_tTab.InsertItem(2, "SYSTEM");
	m_tTab.InsertItem(3, "record");

	m_tpage0.Create(IDD_PAGE0, GetDlgItem(IDC_TAB1));
	m_tpage1.Create(IDD_PAGE1, GetDlgItem(IDC_TAB1));
	m_tpage2.Create(IDD_PAGE2, GetDlgItem(IDC_TAB1));
	m_tpage3.Create(IDD_PAGE3, GetDlgItem(IDC_TAB1));

	CRect rs;
	m_tTab.GetClientRect(&rs);
	//�����ӶԻ����ڸ������е�λ��
		rs.top += 20;
	rs.bottom -= 40;
	rs.left += 1;
	rs.right -= 2;
	
		//�����ӶԻ���ߴ粢�ƶ���ָ��λ��
	m_tpage0.MoveWindow(&rs);
	m_tpage1.MoveWindow(&rs);
	m_tpage2.MoveWindow(&rs);
	m_tpage3.MoveWindow(&rs);
		//�ֱ��������غ���ʾ
	m_tpage0.ShowWindow(true);
	m_tpage1.ShowWindow(false);
	m_tpage2.ShowWindow(false);
	m_tpage3.ShowWindow(false);
	m_tpage0.m_cClient = &m_cClient;
	m_tpage0.m_iChn = m_iChnSelect;
	m_tpage1.m_cClient =  &m_cClient;
	m_tpage2.m_cClient = &m_cClient;
	m_tpage3.m_cClient = &m_cClient;
//	m_tpage3.m_iChn = m_iChnSelect;
	m_tLogDlg.m_cclient = &m_cClient;
		//����Ĭ�ϵ�ѡ�
	m_tTab.SetCurSel(0);
	char temp[30];
	sprintf_s(temp, 30, "%d", 0);

	//���� ��ȡ Slider ��ǰλ�� 
	
	CSliderCtrl *CSlider = (CSliderCtrl *)m_tpage0.GetDlgItem(IDC_SLIDERBRIGHTNESS);
	CStatic *Cst = (CStatic *)m_tpage0.GetDlgItem(IDC_STATICBRIGHTNESS);
	CSlider->SetRange(0, 255);
	CSlider->SetTicFreq(1);//ÿ1����λ��һ�̶�
	Cst->SetWindowText(temp);
	CSlider = (CSliderCtrl *)m_tpage0.GetDlgItem(IDC_SLIDERSATURATION);
	Cst = (CStatic *)m_tpage0.GetDlgItem(IDC_STATICSATURATION);
	Cst->SetWindowText(temp);
	CSlider->SetRange(0, 255);
	CSlider->SetTicFreq(1);//ÿ1����λ��һ�̶�
	CSlider = (CSliderCtrl *)m_tpage0.GetDlgItem(IDC_SLIDERCONSTRAST);
	Cst = (CStatic *)m_tpage0.GetDlgItem(IDC_STATICCONSTRAST);
	Cst->SetWindowText(temp);
	CSlider->SetRange(0, 255);
	CSlider->SetTicFreq(1);//ÿ1����λ��һ�̶�
	CSlider = (CSliderCtrl *)m_tpage0.GetDlgItem(IDC_SLIDERHUE);
	Cst = (CStatic *)m_tpage0.GetDlgItem(IDC_STATICHUE);
	Cst->SetWindowText(temp);
	CSlider->SetRange(0, 255);
	CSlider->SetTicFreq(1);//ÿ1����λ��һ�̶�


	Cbx = (CComboBox *)m_tpage1.GetDlgItem(IDC_COMBOVODEV);
	Cbx->SetCurSel(0);

	Cbx = (CComboBox *)m_tpage0.GetDlgItem(IDC_OSDTYPE);
	Cbx->SetCurSel(0);

	DWORD dwErrorCode = 0;
	WSADATA wsaData;
	//     
	// 	wVersionRequested = MAKEWORD( 2, 2 );
	// 	err = WSAStartup( wVersionRequested, &wsaData );
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
	{
		return -1;
	}

	m_cClient.m_tVChn[0].m_tDecoder.initHIH264();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CMy6000DemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMy6000DemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMy6000DemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMy6000DemoDlg::OnBnClickedLogon()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString Cip;
	int iPort;
	CString Cuser;
	CString Cpassword;

	GetDlgItemText(IDC_IPEDIT, Cip);
	iPort = GetDlgItemInt(IDC_PORTEDIT, NULL, TRUE);
	GetDlgItemText(IDC_USEREDIT, Cuser);
	GetDlgItemText(IDC_PASSWORDEDIT, Cpassword);
	TRACE("Cip = %s \n", Cip);
	TRACE("iPort = %d \n", iPort);
	TRACE("Cuser = %s \n", Cuser);
	TRACE("Cpassword = %s \n", Cpassword);
	
	m_cClient.ClientLogonServer(Cip.GetBuffer(0), iPort, Cuser.GetBuffer(0), Cpassword.GetBuffer(0));
	SetTimer(1, 3000, NULL);

	SetTimer(2, 3000, NULL);
	SetTimer(3, 1000, NULL);
}

LRESULT CMy6000DemoDlg::OnMyRefreshChnCom(WPARAM wParam, LPARAM lParam)
{
	CComboBox *Cbx = (CComboBox *)GetDlgItem(IDC_CHNNUMCOMBO);
	if (m_cClient.m_iChnnum > 0)
	{
		CString strTemp;
		Cbx->ResetContent();//����������������
		for (int i = 1; i <= m_cClient.m_iChnnum; i++)
		{
			strTemp.Format("CH%02d", i);
			Cbx->AddString(strTemp);
		}
		Cbx->SetCurSel(0);
		Cbx->EnableWindow(true);
		m_iChnSelect = 0;
	}
	return 0;
}

LRESULT CMy6000DemoDlg::OnMyRefreshVersion(WPARAM wParam, LPARAM lParam)
{
	if (m_cClient.m_strVersion[0] != '\0')
	{
		SetDlgItemText(IDC_VERSIONSTATIC, m_cClient.m_strVersion);

	}
	return 0;
}
LRESULT CMy6000DemoDlg::OnMyRefreshChnPara(WPARAM wParam, LPARAM lParam)
{

	if (m_iChnSelect >= 0)
	{
		if (m_tTab.GetCurSel() == 0)
		{
			m_tpage0.RefreshChnPara();
			
		//	cbx->SelectString(0, );
		}
		else if (m_tTab.GetCurSel() == 3)
		{
//			m_tpage3.RefreshChnPara();
		}
	}
	return 0;
}
LRESULT CMy6000DemoDlg::OnMyRefreshLogItem(WPARAM wParam, LPARAM lParam)
{
	m_tLogDlg.onMyRefreshLogitem(wParam, lParam);
	
	return 0;
}

void CMy6000DemoDlg::OnEnChangePortedit()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CMy6000DemoDlg::OnEnChangeIpedit()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CMy6000DemoDlg::OnStnClickedVersionstatic()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CMy6000DemoDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}




void CMy6000DemoDlg::OnCbnSelchangeChnnumcombo()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
	CComboBox*	Cbx = (CComboBox *)GetDlgItem(IDC_CHNNUMCOMBO);
	 m_iChnSelect = Cbx->GetCurSel();
	 m_tpage0.m_iChn = m_iChnSelect;
//	 m_tpage3.m_iChn = m_iChnSelect;

	 TRACE("m_iChnSelect =%d \n", m_iChnSelect);
	 ::PostMessage(m_cClient.m_cMainHwnd, WM_MY_REFRESHCHNPARA, 0, 0);
}


void CMy6000DemoDlg::OnCbnSelchangeCombo1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CMy6000DemoDlg::OnCbnSelchangePiccombo()
{
	int i;
	int iCnt = 0;
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CComboBox *Cbx = (CComboBox *)GetDlgItem(IDC_PICCOMBO);
	int iIndex = Cbx->GetCurSel();
	iCnt = (iIndex + 1)*(iIndex + 1);
	m_iPicCnt = iCnt;
	for (i = 0; i < MAX_VO; i++)
	{
		m_cVideoVo[i].m_tVo->ShowWindow(false);
	}
	CRect Rect;

	GetDlgItem(IDC_STATICVO)->GetClientRect(Rect);

	int iX = Rect.left;
	int iY = Rect.top;
	int iWidth = Rect.Width();
	int iHeight = Rect.Height();
	TRACE("iX = %d \n", iX);
	TRACE("iY = %d \n", iY);
	TRACE("iWidth = %d \n", iWidth);
	TRACE("iHeight = %d \n", iHeight);

	int iWidthi = iWidth / (iIndex + 1);
	int iHeighti = iHeight / (iIndex + 1);
	int iVox = iX;
	int iVoy = iY;
	TRACE("iWidthi = %d \n", iWidthi);
	TRACE("iHeighti = %d \n", iHeighti);
	for (i = 0; i < iCnt; i++)
	{
		iVox = iX + i % (iIndex + 1) *iWidthi;
		iVoy = iY + i / (iIndex + 1)*iHeighti;

		m_cVideoVo[i].m_tVo->SetWindowPos(NULL, iVox, iVoy, iWidthi, iHeighti, 0);
		TRACE("i = %d \n", i);
		TRACE("iVox = %d \n", iVox);
		TRACE("iVoy = %d \n", iVoy);
		TRACE("iWidthi = %d \n", iWidthi);
		TRACE("iHeighti m_tVo %d \n", iHeighti);
		m_cVideoVo[i].m_tVo->ShowWindow(true);
	}
}




void CMy6000DemoDlg::VideoVoVoctrlRed(UINT _iID, bool _blFlg)
{
	CDC *dc = GetDlgItem(_iID)->GetDC();
	CPen pen;
	if (_blFlg)
	{
		pen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	}
	else
	{
		pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	}

	CBrush *brush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));
	dc->SelectObject(brush);
	CRect Rect;

	GetDlgItem(_iID)->GetClientRect(Rect);

	int iX = Rect.left;
	int iY = Rect.top;
	int iWidth = Rect.Width();
	int iHeight = Rect.Height();
	dc->SelectObject(pen);
	dc->Rectangle(iX, iY, iWidth, iHeight);
	GetDlgItem(_iID)->ReleaseDC(dc);
}

void CMy6000DemoDlg::OnStnClickedVo(UINT _iID)
{
	VideoVoVoctrlRed(m_iPicSelect + IDC_VO0, false);
	VideoVoVoctrlRed(_iID, true);
	m_iPicSelect = _iID - IDC_VO0;
}

int CMy6000DemoDlg::Findafreevo()
{
	if (m_iPicSelect >= 0 && m_cVideoVo[m_iPicSelect].blUsed == FALSE)
	{
		return m_iPicSelect;
	}
	int i;
	for (i = 0; i < m_iPicCnt; i++)
	{
		if (m_cVideoVo[i].blUsed == false)
		{
			return i;
		}
	}
	return -1;
}

void CMy6000DemoDlg::OnBnClickedConnect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CComboBox *Cbx;
	Cbx = (CComboBox *)GetDlgItem(IDC_CHNNUMCOMBO);
	int iChnSel = Cbx->GetCurSel();
	Cbx = (CComboBox *)GetDlgItem(IDC_SUBCOMBO);
	int iSub = Cbx->GetCurSel();
	Cbx = (CComboBox *)GetDlgItem(IDC_COMBONETMODE);

	int iNetMode = Cbx->GetCurSel();
	HWND hWnd = GetDlgItem(IDC_VO0)->m_hWnd;
	if (m_cClient.m_iLoginstat == 0x8)
	{
		m_cClient.ClientConnectChn(iChnSel, iSub, iNetMode, hWnd);
	}

}


void CMy6000DemoDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialogEx::OnTimer(nIDEvent);
	int i;
	switch (nIDEvent)
	{
		case 1:
		
		
		if (m_cClient.m_iLoginstat >= 0x8)
		{
			SocketSendbuf(m_cClient.m_icmdSocket, "", 0, 0);
		}

		
		break;
		case 2:
			for (i = 0; i < MAX_CHN; i++)
			{
				if (m_cClient.m_tVChn[i].m_iDataSocket > 0)
				{
					SocketSendbuf(m_cClient.m_tVChn[i].m_iDataSocket, "", 0, 0);
				}
			}
			
			break;
		case 3:
			if (m_cClient.m_icmdSocket > 0)
			{
				m_cClient.m_iCmdLiveTime++;
//				TRACE("m_cClient.m_iCmdLiveTime = %d \n", m_cClient.m_iCmdLiveTime);
				if (m_cClient.m_iCmdLiveTime > 20)
				{
					m_cClient.ClientLogoffFromServer();
				}

			}
			for (i = 0; i < MAX_CHN; i++)
			{
				if (m_cClient.m_tVChn[i].m_iDataSocket >0)
				{
					m_cClient.m_tVChn[i].m_iDataLiveTime++;
//					TRACE("m_cClient.m_tVChn[i]..m_iDataLiveTime = %d \n", m_cClient.m_tVChn[i].m_iDataLiveTime);
					if (m_cClient.m_tVChn[i].m_iDataLiveTime > 20)
					{
						m_cClient.m_tVChn[i].ChnDisConFromServer();
					}
				}
			}
			
			break;
	}


}


void CMy6000DemoDlg::OnCbnSelchangeCombonetmode()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CMy6000DemoDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialogEx::OnClose();
	
	if (m_cClient.m_icmdSocket > 0)
	{
		m_cClient.ClientLogoffFromServer();
	}

}


void CMy6000DemoDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
	int CurSel = m_tTab.GetCurSel();
	switch (CurSel)
		{
		case 0:
			m_tpage0.ShowWindow(true);
			m_tpage0.RefreshChnPara();
			m_tpage1.ShowWindow(false);
			m_tpage2.ShowWindow(false);
			m_tpage3.ShowWindow(false);
			break;
		case 1:
			m_tpage0.ShowWindow(false);
			m_tpage1.ShowWindow(true);
			m_tpage1.RefreshPara();
			m_tpage2.ShowWindow(false);
			m_tpage3.ShowWindow(false);
			break;
		case 2:
			m_tpage0.ShowWindow(false);
			m_tpage1.ShowWindow(false);
			m_tpage2.ShowWindow(true);
			m_tpage3.ShowWindow(false);
			break;
		case 3:
			m_tpage0.ShowWindow(false);
			m_tpage1.ShowWindow(false);
			m_tpage2.ShowWindow(false);
			m_tpage3.ShowWindow(true);
//			m_tpage3.RefreshChnPara();
			break;
		default:
			break;
			 }
	
		 * pResult = 0;
}


void CMy6000DemoDlg::OnBnClickedDisconnect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CComboBox *Cbx;
	Cbx = (CComboBox *)GetDlgItem(IDC_CHNNUMCOMBO);
	int iChnSel = Cbx->GetCurSel();
	Cbx = (CComboBox *)GetDlgItem(IDC_SUBCOMBO);
	
	

	int iNetMode = Cbx->GetCurSel();
	HWND hWnd = GetDlgItem(IDC_VO0)->m_hWnd;
	if (m_cClient.m_tVChn[iChnSel].m_iConnectStat)
	{
		
		m_cClient.m_tVChn[iChnSel].ChnDisConFromServer();
	}

}





void CMy6000DemoDlg::OnBnClickedLogbutton()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_tLogDlg.DoModal();
}
