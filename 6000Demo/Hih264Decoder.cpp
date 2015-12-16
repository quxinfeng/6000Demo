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
	
	
	m_hDll = NULL;                                                  //成员变量初始化
	m_pHi264DecCreate = NULL;
	m_pHi264DecFrame = NULL;
	m_pHi264DecDestroy = NULL;                                      //实现加载动态库

	HMODULE m_hDll = LoadLibrary("hi_h264dec_w.dll");
	if (m_hDll)
	{
		m_pHi264DecCreate = (pfHi264DecCreate)GetProcAddress(m_hDll, "Hi264DecCreate");
		m_pHi264DecFrame = (pfHi264DecFrame)GetProcAddress(m_hDll, "Hi264DecFrame");
		m_pHi264DecDestroy = (pfHi264DecDestroy)GetProcAddress(m_hDll, "Hi264DecDestroy");
		m_pHi264DecAu = (pfHi264DecAU)GetProcAddress(m_hDll, "Hi264DecAU");
		//判断是否成功
		if (NULL == m_pHi264DecCreate || NULL == m_pHi264DecFrame || NULL == m_pHi264DecDestroy)
		{
			AfxMessageBox("load dll func failed");
			return -1;
			// HWND hWnd = GetDlgItem(NULL,IDD_DEMO02_DIALOG);  dlg使用
			// ::SendMessage(hWnd,WM_CLOSE,MB_OK,0);
		}
		// TRACE("fafea");
	}

	return 0;

}


int Hih264Decoder::DecoderCreate(int _iWidth, int _iHeight)
{

	//FILE *yuv = NULL; /* 输出yuv文件 */
	H264_DEC_ATTR_S dec_attrbute;
	/* 解码器最大参考帧数: 16 */
	dec_attrbute.uBufNum = 16;
	/* 解码器最大图像宽高, D1图像(720x576) */
	dec_attrbute.uPicHeightInMB = (_iHeight + 16 -1) /16;
	dec_attrbute.uPicWidthInMB = (_iWidth + 16 - 1) / 16;
	/* 没有用户数据 */
	dec_attrbute.pUserData = NULL;
	/* 以 "00 00 01" 或 "00 00 00 01" 开始的码流 */
	dec_attrbute.uStreamInType = 0x00;
	/* bit0 = 1: 标准输出模式; bit0 = 0: 快速输出模式 */
	/* bit4 = 1: 启动内部Deinterlace; bit4 = 0: 不启动内部Deinterlace */
	dec_attrbute.uWorkMode = 0x10;
	/* 销毁解码器 */
	if (m_hHandle != NULL)
	{
		m_pHi264DecDestroy(m_hHandle);       //首先解决
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
		m_pHi264DecDestroy(m_hHandle);       //首先解决
		m_hHandle = NULL;
	}

	return 0;
}


