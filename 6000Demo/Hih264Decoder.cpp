#include "stdafx.h"
#include "Hih264Decoder.h"


Hih264Decoder::Hih264Decoder()
{
	m_hDll = NULL;
	m_iend = 0;
	m_hHandle = NULL;
}


Hih264Decoder::~Hih264Decoder()
{
}
int  Hih264Decoder::initHIH264()
{
	
	
	m_hDll = NULL;                                                  //��Ա������ʼ��
	m_pHi264DecCreate = NULL;
	m_pHi264DecFrame = NULL;
	m_pHi264DecDestroy = NULL;                                      //ʵ�ּ��ض�̬��

	HMODULE m_hDll = LoadLibrary("hi_h264dec_w.dll");
	if (m_hDll)
	{
		m_pHi264DecCreate = (pfHi264DecCreate)GetProcAddress(m_hDll, "Hi264DecCreate");
		m_pHi264DecFrame = (pfHi264DecFrame)GetProcAddress(m_hDll, "Hi264DecFrame");
		m_pHi264DecDestroy = (pfHi264DecDestroy)GetProcAddress(m_hDll, "Hi264DecDestroy");
		m_pHi264DecAu = (pfHi264DecAU)GetProcAddress(m_hDll, "Hi264DecAU");
		//�ж��Ƿ�ɹ�
		if (NULL == m_pHi264DecCreate || NULL == m_pHi264DecFrame || NULL == m_pHi264DecDestroy)
		{
			AfxMessageBox("load dll func failed");
			return -1;
			// HWND hWnd = GetDlgItem(NULL,IDD_DEMO02_DIALOG);  dlgʹ��
			// ::SendMessage(hWnd,WM_CLOSE,MB_OK,0);
		}
		// TRACE("fafea");
	}

	return 0;

}


int Hih264Decoder::DecoderCreate(int _iWidth, int _iHeight)
{

	//FILE *yuv = NULL; /* ���yuv�ļ� */
	H264_DEC_ATTR_S dec_attrbute;
	/* ���������ο�֡��: 16 */
	dec_attrbute.uBufNum = 16;
	/* ���������ͼ����, D1ͼ��(720x576) */
	dec_attrbute.uPicHeightInMB = (_iHeight + 16 -1) /16;
	dec_attrbute.uPicWidthInMB = (_iWidth + 16 - 1) / 16;
	/* û���û����� */
	dec_attrbute.pUserData = NULL;
	/* �� "00 00 01" �� "00 00 00 01" ��ʼ������ */
	dec_attrbute.uStreamInType = 0x00;
	/* bit0 = 1: ��׼���ģʽ; bit0 = 0: �������ģʽ */
	/* bit4 = 1: �����ڲ�Deinterlace; bit4 = 0: �������ڲ�Deinterlace */
	dec_attrbute.uWorkMode = 0x10;
	/* ���ٽ����� */
	if (m_hHandle != NULL)
	{
		m_pHi264DecDestroy(m_hHandle);       //���Ƚ��
		m_hHandle = NULL;
	}

	m_hHandle = m_pHi264DecCreate(&dec_attrbute);

	

	Sleep(1);
	return 0;

}

int Hih264Decoder::DecoderDestroy()
{
	if (m_hHandle != NULL)
	{
		m_pHi264DecDestroy(m_hHandle);       //���Ƚ��
		m_hHandle = NULL;
	}

	return 0;
}


