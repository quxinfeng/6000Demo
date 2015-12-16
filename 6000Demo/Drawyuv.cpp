#include "stdafx.h"
#include "Drawyuv.h"



#include "stdafx.h"
#include <ddraw.h>
#include <memory.h>
#include <stdio.h>
#include "Resource.h"
#include "ddmm.h"



#define MIN_VIDEO_WIDTH		320
#define MIN_VIDEO_HEIGHT	240

#define log_debug
#define log_info
#define log_err

#pragma comment(lib,"ddraw.lib")
#pragma comment(lib,"Dxguid.lib")

#define _S(a)		(a)>255 ? 255 : (a)<0 ? 0 : (a)
#define _R(y,u,v) (0x2568*(y) + 0x3343*(u)) /0x2000
#define _G(y,u,v) (0x2568*(y) - 0x0c92*(v) - 0x1a1e*(u)) /0x2000
#define _B(y,u,v) (0x2568*(y) + 0x40cf*(v))	/0x2000
#define _Ps565(r,g,b) ( ((r & 0xF8) >> 3) | (((g & 0xF8) << 3)) | (((b & 0xF8) << 8)) )



DDPIXELFORMAT _ddpfOverlayFormat = { sizeof(DDPIXELFORMAT), DDPF_FOURCC,MAKEFOURCC('Y','U','Y','2'),0,0,0,0,0 };
//---------------------------------------------------------------------------

bool __stdcall IsRectInRect(RECT* _pRect1, RECT* _pRect2)
{
	if (_pRect1 == NULL || _pRect2 == NULL)
		return false;

	return ((_pRect1->left > _pRect2->left) && (_pRect1->left < _pRect2->right)
		&& (_pRect1->top > _pRect2->top) && (_pRect1->top < _pRect2->bottom)
		&& (_pRect1->right > _pRect2->left) && (_pRect1->right < _pRect2->right)
		&& (_pRect1->bottom > _pRect2->top) && (_pRect1->bottom < _pRect2->bottom));
}




static bool IsEqualRect(const RECT& _rct1, const RECT& _rct2)
{
	return (_rct1.left == _rct2.left && _rct1.right == _rct2.right && _rct1.top == _rct2.top && _rct1.bottom == _rct2.bottom);
}



//---------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////
Drawyuv::Drawyuv()
{

	m_iCount = 0;
	m_iOff = 0;
	m_iNum = 0;

	m_hDisplayWnd = NULL;
	m_bufRGB = NULL;

	m_lpDirectDraw1 = NULL;
	m_lpDDSPrimary = NULL;
	m_lpDDSOne = NULL;
	//m_lpDirectDraw2 = NULL;

	m_ddColorKey = 0;
	m_ImageWidth = 0;
	m_ImageHeight = 0;
	m_Voffset = 0;
	m_Uoffset = 0;
	
	m_iLockTimes = 0;
	m_iUsed = 0;

	m_uiMonitorIndex = 0;
	m_blMainMonitor = true;

	m_pcClipper = NULL;

	

	InitializeCriticalSection(&m_csDraw);
}
//---------------------------------------------------------------------------
Drawyuv::~Drawyuv()
{
	ReleaseDirectDraw();
	if (m_bufRGB)
	{
		delete[] m_bufRGB;
		m_bufRGB = NULL;
	}

	DeleteCriticalSection(&m_csDraw);

	
}
//---------------------------------------------------------------------------
int  Drawyuv::InitDraw(UINT Width, UINT Height, DWORD ColorKey /*= 24*/)
{
	

	init_yuv2rgb();
	HDC hDC = GetDC(0);
	int iColorBit = GetDeviceCaps(hDC, BITSPIXEL);
	ReleaseDC(0, hDC);

	if (iColorBit == 32) {
		//yuv2rgb=&CLS_Draw::yuv2rgb_32;
		m_iCount = 4;
	}
	if (iColorBit == 16) {
		//yuv2rgb=&CLS_Draw::yuv2rgb_565;
		m_iCount = 2;
	}
	if (iColorBit == 24) {
		//yuv2rgb=&CLS_Draw::yuv2rgb_24;
		m_iCount = 3;
	}

	m_lpDirectDraw1 = NULL;
	//m_lpDirectDraw2=NULL;
	m_lpDDSPrimary = NULL;

	m_ddColorKey = 0;
	Width += 15;
	Width &= 0xfff0;
	Height += 3;
	Height &= 0xfffc;

	m_ImageWidth = Width;
	m_ImageHeight = Height;
	m_Voffset = Width*Height;
	m_Uoffset = Width*Height * 5 / 4;

	if (Width % 64 == 0)
		m_ddWidthCal = Width;
	else
		m_ddWidthCal = (Width / 64 + 1) * 64;

	if (!m_bufRGB)
	{
		m_bufRGB = new unsigned char[4 * Width * Height];
	}
	m_lpDDSOne = NULL;
	return 1;
}

void  Drawyuv::ReleaseDirectDraw(void)
{
	//m_iUsed = 0;//modify by liujia 2009-3-6
	EnterCriticalSection(&m_csDraw);
	if (m_pcClipper != NULL)
	{
		m_pcClipper->Release();
		m_pcClipper = NULL;
	}

	if (m_lpDDSOne)
	{
		m_lpDDSOne->Unlock(NULL);
		m_lpDDSOne->Release();
		m_lpDDSOne = NULL;
	}

	if (m_lpDDSPrimary)
	{
		m_lpDDSPrimary->Release();
		m_lpDDSPrimary = NULL;
	}

	if (m_lpDirectDraw1)
	{
		m_lpDirectDraw1->Release();
		m_lpDirectDraw1 = NULL;
	}
	LeaveCriticalSection(&m_csDraw);
}

int  Drawyuv::CreateDirectDraw(HWND  hwnd/*, DWORD ColorKey*/, unsigned int _uiMonitorIndex)
{
	int retVal = DD_FALSE;
	DDSURFACEDESC2	ddsd;
	HRESULT		    ddrval;
	LPDIRECTDRAW    lpDD1;
	m_uiMonitorIndex = _uiMonitorIndex;

	//GUID idd2={0xB3A6F3E0,0x2B43,0x11CF,{0xA2,0xDE,0x00,0xAA,0x00,0xB9,0x33,0x56}};

	m_hDisplayWnd = hwnd;

	EnterCriticalSection(&m_csDraw);
	__try
	{
		lpDD1 = DirectDrawCreateFromWindow(m_hDisplayWnd, NULL);//g_vecDrawDeviceList[m_uiMonitorIndex]->lpGUID);
		if (NULL != lpDD1)
		{
			ddrval = DD_OK;
			if (NULL == m_lpDirectDraw1)
			{
				ddrval = lpDD1->QueryInterface(IID_IDirectDraw7, (void **)&m_lpDirectDraw1);
			}
			//else continue

			lpDD1->Release();

			if (DD_OK == ddrval)
			{
				if (NULL != m_lpDirectDraw1)
				{
					ddrval = m_lpDirectDraw1->SetCooperativeLevel(NULL, DDSCL_NORMAL);
					if (DD_OK == ddrval)
					{
						ZeroMemory(&ddsd, sizeof(ddsd));
						ddsd.dwSize = sizeof(ddsd);
						ddsd.dwFlags = DDSD_CAPS;
						ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

						ddrval = m_lpDirectDraw1->CreateSurface(&ddsd, &m_lpDDSPrimary, NULL);
						if (DD_OK == ddrval)
						{
							if (SUCCEEDED(m_lpDirectDraw1->CreateClipper(0, &m_pcClipper, NULL)))
							{
								if (SUCCEEDED(m_pcClipper->SetHWnd(0, m_hDisplayWnd)))
								{
									if (SUCCEEDED(m_lpDDSPrimary->SetClipper(m_pcClipper)))
									{
										//Second Surface
										memset(&ddsd, 0, sizeof(ddsd));
										ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
										ddsd.dwHeight = m_ImageHeight;
										ddsd.dwWidth = m_ddWidthCal;
										ddsd.ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY;
										ddsd.ddpfPixelFormat = _ddpfOverlayFormat;
										ddsd.dwSize = sizeof(ddsd);

										ddrval = m_lpDirectDraw1->CreateSurface(&ddsd, &m_lpDDSOne, NULL);

										if (ddrval != DD_OK)
										{
											log_debug(1, "CLS_Draw::CreateDirectDraw : m_lpDirectDraw1->CreateSurface failed, result = %08x!!!!!!", ddrval);
										}

										if (DD_OK == ddrval)
										{
											if (m_ImageWidth % 64 == 0)
												m_ddWidthCal = m_ImageWidth;
											else
												m_ddWidthCal = (m_ImageWidth / 64 + 1) * 64;

											m_iCount = 2;

											m_iOff = m_ddWidthCal*m_ImageHeight*m_iCount;
											m_iNum = 9;

											retVal = m_iNum;
										}
										else//m_lpDirectDraw1->CreateSurface failed
										{
											return -1;
										}

										ZeroMemory(&m_ddSurface, sizeof m_ddSurface);
										m_ddSurface.dwSize = sizeof(m_ddSurface);
										m_iUsed = 1;//modify by liujia 2009-3-6
									}
									else//m_lpDDSPrimary->SetClipper failed
									{
										log_debug(1, "m_lpDDSPrimary->SetClipper failed");
										retVal = 0;
									}
								}
								else//pcClipper->SetHWnd failed
								{
									log_debug(1, "pcClipper->SetHWnd failed");
									retVal = 0;
								}
							}
							else//m_lpDirectDraw1->CreateClipper failed
							{
								log_debug(1, "m_lpDirectDraw1->CreateClipper failed");
								retVal = 0;
							}
						}
						else//m_lpDirectDraw1->CreateSurface failed
						{
							log_debug(1, "m_lpDirectDraw1->CreateSurface failed");
							retVal = 0;
						}
					}
					else//m_lpDirectDraw1->SetCooperativeLevel failed
					{
						log_debug(1, "m_lpDirectDraw1->SetCooperativeLevel failed");
						retVal = 0;
					}
				}
				else//m_lpDirectDraw1 == NULL
				{
					log_debug(1, "m_lpDirectDraw1 == NULL");
					retVal = 0;
				}
			}
			else//lpDD1->QueryInterface failed
			{
				log_debug(1, "lpDD1->QueryInterface failed");
				retVal = 0;
			}
		}
		else//lpDD1 == NULL
		{
			log_debug(1, "lpDD1 == NULL");
			retVal = 0;
			log_debug(1, "DirectDrawCreateFromWindow error,this=0x%08x", this);
		}
	}
	__finally
	{
		LeaveCriticalSection(&m_csDraw);
	}

	if (0 == retVal)
	{
		ReleaseDirectDraw();
	}
	//else continue

	return retVal;
}
//---------------------------------------------------------------------------
int Drawyuv::SetDrawWnd(HWND _hwnd, bool _blReplace)
{
	LPDIRECTDRAWCLIPPER pcClipper = NULL;
	if (FAILED(m_lpDDSPrimary->GetClipper(&pcClipper)))
		return E_FAIL;
	if (FAILED(pcClipper->SetHWnd(0, _hwnd)))
		return E_FAIL;

	if (_blReplace)
		m_hDisplayWnd = _hwnd;

	log_debug(0, "[CLS_Draw::SetDrawWnd] this(0x%08x), hWnd(0x%08x)", this, _hwnd);
	return 0;
}

void Drawyuv::AdjustDrawScale(int _imgWidth, int _imgHeight, RECT* _pDrawRect)
{
	int l = _pDrawRect->left;
	int r = _pDrawRect->right;
	int t = _pDrawRect->top;
	int b = _pDrawRect->bottom;
	int width = r - l;
	int height = b - t;
	if (((_imgWidth == 1280) && (_imgHeight == 720))
		|| ((_imgWidth == 1920) && (_imgHeight == 1080)))
	{
		int m = _imgWidth*height;
		int n = _imgHeight*width;
		if (m < n) 		//水平留边
		{
			l += (width - _imgWidth*height / _imgHeight) / 2;
			r -= (width - _imgWidth*height / _imgHeight) / 2;
		}
		else if (m > n) //垂直留边
		{
			t += (height - _imgHeight*width / _imgWidth) / 2;
			b -= (height - _imgHeight*width / _imgWidth) / 2;
		}
	}
	_pDrawRect->left = l;
	_pDrawRect->right = r;
	_pDrawRect->top = t;
	_pDrawRect->bottom = b;
}

inline int Drawyuv::Inner_DrawExtraInfo(DrawNeed* _pDrawParam, BYTE* _pbtSurface)
{
	if (NULL != _pDrawParam)
	{

	}

	

	return 0;
}

//---------------------------------------------------------------------------
BOOL  Drawyuv::DirectDrawYUV(unsigned char * src)//,DecoderNeed* _Param)
{
	BOOL retVal = FALSE;

	if (NULL != m_hDisplayWnd)
	{
		EnterCriticalSection(&m_csDraw);

		__try
		{
			if ((NULL != m_lpDDSOne) || (CreateDirectDraw(m_hDisplayWnd, m_uiMonitorIndex) != 0))
			{
				BYTE* btSurface;
				int iCount = 0;
				DWORD dwMask = 1;

				//		char cDateTime[32];

				RECT rctZero = { 0 };
				RECT* rcDraw = &rctZero;//_Param->m_rcDraw;
										//BOOL ret = TRUE;
				HRESULT ddrval = DD_OK;

				if (NULL != m_lpDDSOne)
				{
					ddrval = m_lpDDSOne->Lock(NULL, &m_ddSurface, 0, NULL);
				}
				//else continue

				HRESULT hr = DD_OK;
				while ( /*(ddrval != DD_OK)*/!SUCCEEDED(ddrval) && (iCount++ < 2))
				{
					log_info(1, "[CLS_Draw::DirectDrawYUV] ddrval(0x%08x), iCount(%d), Line(%d)", ddrval, iCount, __LINE__);
					//此处代码为解决Ctrl+Alt+Del锁定屏幕再恢复,视频不能恢复的情况
					//注意以下两个Restore()可以不重复,但顺序不能颠倒!!!
					if (m_lpDDSOne && m_lpDDSOne->IsLost())
					{
						hr = m_lpDDSOne->Restore();
						if (!SUCCEEDED(hr))
						{
							log_info(1, "[CLS_Draw::DirectDrawYUV] (m_lpDDSOne->Restore) ddrval(0x%08x), iCount(%d), Line(%d)", hr, iCount, __LINE__);
						}
					}
					//else continue

					if (m_lpDDSPrimary && m_lpDDSPrimary->IsLost())
					{
						hr = m_lpDDSPrimary->Restore();
						if (!SUCCEEDED(hr))
						{
							log_info(1, "[CLS_Draw::DirectDrawYUV] (m_lpDDSPrimary->Restore) ddrval(0x%08x), iCount(%d), Line(%d)", hr, iCount, __LINE__);
						}
					}
					//else continue
					Sleep(5);    //不能删

					if (m_lpDDSOne)
					{
						ddrval = m_lpDDSOne->Lock(NULL, &m_ddSurface, 0, NULL);
						if (!SUCCEEDED(ddrval))
						{
							log_info(1, "[CLS_Draw::DirectDrawYUV] (m_lpDDSOne->Lock) ddrval(0x%08x), iCount(%d), Line(%d)",
								ddrval, iCount, __LINE__);
						}
					}
					//else continue
				}

				if (DD_OK == ddrval)
				{
					m_iLockTimes = 0;
					btSurface = (BYTE *)m_ddSurface.lpSurface;//+m_iOff*ich;
					DrawI420(src, btSurface);
					Inner_DrawExtraInfo(NULL, btSurface);
					ddrval = m_lpDDSOne->Unlock(NULL);

					RECT rcSrc = { 0, 0, (long)m_ImageWidth, long(rcSrc.top + m_ImageHeight) };

					//实时调整图象显示位置 zc add 2007.4.24
					if (dwMask == 1 && m_lpDDSPrimary != NULL && m_lpDDSOne != NULL)
					{
						ChangeDrawWnd(m_hDisplayWnd);
						RECT rctWindow = { 0 };
						GetWindowRect(m_hDisplayWnd, &rctWindow);

						const RECT rctZero = { 0 };
						if (memcmp(&rctZero, rcDraw, sizeof(RECT)) == 0)		//	全为0, 就不偏移。(为兼容以前的功能，使用AdjustDrawScale)
						{
							memset(rcDraw, 0, sizeof(RECT));			//	把显示区域标记为0，表示每次都需要调整显示比例
							AdjustDrawScale(m_ImageWidth, m_ImageHeight, &rctWindow);
						}
						else if (IsRectInRect(rcDraw, &rctWindow))
						{
							rctWindow = *rcDraw;
						}

						//计算当前显示器的相对位置进行偏移
						RECT rcMonitor = { 0 };
						rctWindow.left -= rcMonitor.left;
						rctWindow.right -= rcMonitor.left;
						rctWindow.top -= rcMonitor.top;
						rctWindow.bottom -= rcMonitor.top;
						ddrval = m_lpDDSPrimary->Blt(&rctWindow, m_lpDDSOne, &rcSrc, DDBLT_WAIT, NULL);
						if (FAILED(hr))
							log_debug(0, "[CLS_Draw::DirectDrawYUV] m_lpDDSPrimary->Blt() failed, hr(0x%08x)", ddrval);
					}

				
					//else continue
				}
				else if (m_iLockTimes <= 50)
				{
					m_iLockTimes++;
					retVal = FALSE;
				}
				else//ddrval == NULL and m_iLockTimes > 50
				{
					m_iLockTimes = 0;
					ReleaseDirectDraw();
					Sleep(15);
					retVal = FALSE;
				}
			}
			else//m_lpDDSOne == NULL and create ddraw failed
			{
				retVal = FALSE;
			}
		}
		__finally
		{
			LeaveCriticalSection(&m_csDraw);
		}
	}
	else//m_hDisplayWnd == NULL
	{
		retVal = FALSE;
	}


	//__asm{EMMS}

	return retVal;
}
//---------------------------------------------------------------------------//---------------------------------------------------------------------------
void Drawyuv::DrawI420_rgb(BYTE  * src, BYTE *dst)
{
	yuv2rgb(src, m_ImageWidth, src + m_ImageWidth*m_ImageHeight,
		src + m_ImageWidth*m_ImageHeight * 5 / 4, m_ImageWidth / 2,
		m_bufRGB, m_ImageWidth, m_ImageHeight, m_ddWidthCal);
	memcpy(dst, m_bufRGB, m_ImageWidth*m_ImageHeight*m_iCount);
}
//---------------------------------------------------------------------------
void Drawyuv::DrawI420_yuvEx(BYTE  * src, BYTE *dst, int _iDist, int _iPixMode)
{
	BYTE *Yeven = src;
	BYTE *Yodd = src + m_ImageWidth;
	BYTE *U = src + m_Voffset;
	BYTE *V = src + m_Uoffset;
	BYTE *destEven = dst;
	BYTE *destOdd = dst + (_iDist << 1);
	UINT i, j;
	BYTE curintu, curintv;
	int dist = (_iDist + _iDist - m_ImageWidth) << 1;
	for (j = 0; j<(m_ImageHeight >> 1); j++)
	{
		for (i = 0; i < (m_ImageWidth >> 1); i++)
		{
			curintu = *(U++);
			curintv = *(V++);

			*(destEven++) = *(Yeven++);
			*(destEven++) = curintu;
			*(destEven++) = *(Yeven++);
			*(destEven++) = curintv;
			*(destOdd++) = *(Yodd++);
			*(destOdd++) = curintu;
			*(destOdd++) = *(Yodd++);
			*(destOdd++) = curintv;
		}
		Yeven += m_ImageWidth;
		Yodd += m_ImageWidth;
		destEven += dist;
		destOdd += dist;
	}
}
//---------------------------------------------------------------------------
void Drawyuv::DrawI420_yuv(BYTE  * src, BYTE *dst)
{
	BYTE *Yeven = src;
	BYTE *Yodd = src + m_ImageWidth;
	BYTE *U = src + m_ImageWidth*m_ImageHeight;
	BYTE *V = src + m_ImageWidth*m_ImageHeight * 5 / 4;
	UINT i, j;
	BYTE curintu, curintv;
	BYTE *destEven = dst;
	BYTE *destOdd = destEven + m_ddSurface.lPitch;
	for (j = 0; j<(m_ImageHeight >> 1); j++)
	{
		for (i = 0; i < (m_ImageWidth >> 1); i++)
		{
			curintu = *(U++);
			curintv = *(V++);

			*(destEven++) = *(Yeven++);
			*(destEven++) = curintu;
			*(destEven++) = *(Yeven++);
			*(destEven++) = curintv;
			*(destOdd++) = *(Yodd++);
			*(destOdd++) = curintu;
			*(destOdd++) = *(Yodd++);
			*(destOdd++) = curintv;
		}
		Yeven += m_ImageWidth;
		Yodd += m_ImageWidth;
		destEven += (m_ddSurface.lPitch - m_ImageWidth) * 2;
		destOdd += (m_ddSurface.lPitch - m_ImageWidth) * 2;
	}
}
//---------------------------------------------------------------------------
void Drawyuv::DrawDot(int _x, int _y, unsigned long u32Color, BYTE* btSurface)
{
	int x = _x, y = _y;
	int dist = m_ddSurface.lPitch;
	BYTE* btOff = btSurface + dist*y + x*m_iCount;
	unsigned long u32ColorY = ((u32Color & 0x00ff0000) >> 16);
	if (m_iNum == 9)
	{
		btOff[0] = (BYTE)u32ColorY;
		btOff[1] = (BYTE)((u32Color & 0x0000ff00) >> 8);//(((u32Color&0x0000ff00)>>8)+btOff[1])>>1;
		btOff[2] = 0;
		btOff[dist] = 0;
		btOff[3] = (BYTE)(u32Color & 0x000000ff);//((u32Color&0x000000ff)+btOff[3])>>1;
	}
	else
	{
		if (m_iCount == 4)
		{
			btOff[0] = (BYTE)u32ColorY;
			btOff[1] = (BYTE)u32ColorY;
			btOff[2] = (BYTE)u32ColorY;
			btOff[3] = (BYTE)u32ColorY;
			btOff[4] = 0;
			btOff[5] = 0;
			btOff[6] = 0;
			btOff[7] = 0;
			btOff[dist + 0] = 0;
			btOff[dist + 1] = 0;
			btOff[dist + 2] = 0;
			btOff[dist + 3] = 0;
		}
		else if (m_iCount == 3)
		{
			btOff[0] = (BYTE)u32ColorY;
			btOff[1] = (BYTE)u32ColorY;
			btOff[2] = (BYTE)u32ColorY;
			btOff[4] = 0;
			btOff[5] = 0;
			btOff[6] = 0;
			btOff[dist + 0] = 0;
			btOff[dist + 1] = 0;
			btOff[dist + 2] = 0;
		}
		else if (m_iCount == 2)
		{
			btOff[0] = (BYTE)u32ColorY;
			btOff[1] = (BYTE)u32ColorY;
			btOff[2] = 0;
			btOff[3] = 0;
			btOff[dist + 0] = 0;
			btOff[dist + 1] = 0;
		}
	}
}

//---------------------------------------------------------------------------
void Drawyuv::init_yuv2rgb()
{
	int i;
	for (i = 0; i<256; i++)
	{
		if (i >= 16)
			if (i>240)
				m_lut.m_plY[i] = m_lut.m_plY[240];
			else
				m_lut.m_plY[i] = 298 * (i - 16);
		else
			m_lut.m_plY[i] = 0;
		if ((i >= 16) && (i <= 240))
		{
			m_lut.m_plRV[i] = 408 * (i - 128);
			m_lut.m_plGV[i] = -208 * (i - 128);
			m_lut.m_plGU[i] = -100 * (i - 128);
			m_lut.m_plBU[i] = 517 * (i - 128);
		}
		else if (i<16)
		{
			m_lut.m_plRV[i] = 408 * (16 - 128);
			m_lut.m_plGV[i] = -208 * (16 - 128);
			m_lut.m_plGU[i] = -100 * (16 - 128);
			m_lut.m_plBU[i] = 517 * (16 - 128);
		}
		else
		{
			m_lut.m_plRV[i] = m_lut.m_plRV[240];
			m_lut.m_plGV[i] = m_lut.m_plGV[240];
			m_lut.m_plGU[i] = m_lut.m_plGU[240];
			m_lut.m_plBU[i] = m_lut.m_plBU[240];
		}
	}
}
//---------------------------------------------------------------------------
void Drawyuv::yuv2rgb_565(uint8_t *puc_y, int stride_y,
	uint8_t *puc_u, uint8_t *puc_v, int stride_uv,
	uint8_t *puc_out, int width_y, int height_y,
	unsigned int _stride_out)
{

	int x, y;
	unsigned short *pus_out;
	int stride_diff = _stride_out - width_y; // expressed in short

	if (height_y < 0) {
		/* we are flipping our output upside-down */
		height_y = -height_y;
		puc_y += (height_y - 1) * stride_y;
		puc_u += (height_y / 2 - 1) * stride_uv;
		puc_v += (height_y / 2 - 1) * stride_uv;
		stride_y = -stride_y;
		stride_uv = -stride_uv;
	}
	pus_out = (unsigned short *)puc_out;

	for (y = 0; y<height_y; y++)
	{
		for (x = 0; x<width_y; x++)
		{
			signed int _r, _g, _b;
			signed int r, g, b;
			signed int y, u, v;

			y = puc_y[x] - 16;
			u = puc_u[x >> 1] - 128;
			v = puc_v[x >> 1] - 128;

			_r = _R(y, u, v);
			_g = _G(y, u, v);
			_b = _B(y, u, v);

			r = _S(_r);
			g = _S(_g);
			b = _S(_b);

			pus_out[0] = (unsigned short)_Ps565(r, g, b);

			pus_out++;
		}

		puc_y += stride_y;
		if (y % 2) {
			puc_u += stride_uv;
			puc_v += stride_uv;
		}
		pus_out += stride_diff;
	}
}
//---------------------------------------------------------------------------
void Drawyuv::yuv2rgb_32(uint8_t *puc_y, int stride_y,
	uint8_t *puc_u, uint8_t *puc_v, int stride_uv,
	uint8_t *puc_out, int width_y, int height_y,
	unsigned int _stride_out)
{

	int x, y;
	int stride_diff = 4 * (_stride_out - width_y);

	if (height_y < 0) {
		/* we are flipping our output upside-down */
		height_y = -height_y;
		puc_y += (height_y - 1) * stride_y;
		puc_u += (height_y / 2 - 1) * stride_uv;
		puc_v += (height_y / 2 - 1) * stride_uv;
		stride_y = -stride_y;
		stride_uv = -stride_uv;
	}

	for (y = 0; y<height_y; y++)
	{
		for (x = 0; x<width_y; x++)
		{
			signed int _r, _g, _b;
			signed int r, g, b;
			signed int y, u, v;

			y = puc_y[x] - 16;
			u = puc_u[x >> 1] - 128;
			v = puc_v[x >> 1] - 128;

			_r = _R(y, u, v);
			_g = _G(y, u, v);
			_b = _B(y, u, v);

			r = _S(_r);
			g = _S(_g);
			b = _S(_b);

			puc_out[0] = r;
			puc_out[1] = g;
			puc_out[2] = b;
			puc_out[3] = 0;

			puc_out += 4;
		}

		puc_y += stride_y;
		if (y % 2) {
			puc_u += stride_uv;
			puc_v += stride_uv;
		}
		puc_out += stride_diff;
	}
}
//---------------------------------------------------------------------------
void Drawyuv::yuv2rgb_24(uint8_t *puc_y, int stride_y,
	uint8_t *puc_u, uint8_t *puc_v, int stride_uv,
	uint8_t *puc_out, int width_y, int height_y,
	unsigned int _stride_out)
{

	int x, y;
	int stride_diff = 6 * _stride_out - 3 * width_y;

	if (height_y < 0) {
		/* we are flipping our output upside-down */
		height_y = -height_y;
		puc_y += (height_y - 1) * stride_y;
		puc_u += (height_y / 2 - 1) * stride_uv;
		puc_v += (height_y / 2 - 1) * stride_uv;
		stride_y = -stride_y;
		stride_uv = -stride_uv;
	}

	for (y = 0; y<height_y; y += 2)
	{
		uint8_t* pY = puc_y;
		uint8_t* pY1 = puc_y + stride_y;
		uint8_t* pU = puc_u;
		uint8_t* pV = puc_v;
		uint8_t* pOut2 = puc_out + 3 * _stride_out;
		for (x = 0; x<width_y; x += 2)
		{
			int R, G, B;
			int Y;
			unsigned int tmp;
			R = m_lut.m_plRV[*pU];
			G = m_lut.m_plGV[*pU];
			pU++;
			G += m_lut.m_plGU[*pV];
			B = m_lut.m_plBU[*pV];
			pV++;
#define PUT_COMPONENT(p,v,i) 	\
    tmp=(unsigned int)(v); 	\
    if(tmp < 0x10000) 		\
	p[i]=tmp>>8; 		\
    else			\
	p[i]=(tmp >> 24) ^ 0xff; 
			Y = m_lut.m_plY[*pY];
			pY++;
			PUT_COMPONENT(puc_out, R + Y, 0);
			PUT_COMPONENT(puc_out, G + Y, 1);
			PUT_COMPONENT(puc_out, B + Y, 2);
			Y = m_lut.m_plY[*pY];
			pY++;
			PUT_COMPONENT(puc_out, R + Y, 3);
			PUT_COMPONENT(puc_out, G + Y, 4);
			PUT_COMPONENT(puc_out, B + Y, 5);
			Y = m_lut.m_plY[*pY1];
			pY1++;
			PUT_COMPONENT(pOut2, R + Y, 0);
			PUT_COMPONENT(pOut2, G + Y, 1);
			PUT_COMPONENT(pOut2, B + Y, 2);
			Y = m_lut.m_plY[*pY1];
			pY1++;
			PUT_COMPONENT(pOut2, R + Y, 3);
			PUT_COMPONENT(pOut2, G + Y, 4);
			PUT_COMPONENT(pOut2, B + Y, 5);
			puc_out += 6;
			pOut2 += 6;
		}

		puc_y += 2 * stride_y;
		puc_u += stride_uv;
		puc_v += stride_uv;
		puc_out += stride_diff;
	}
}
//---------------------------------------------------------------------------
#if 0

#else
#endif
//---------------------------------------------------------------------------
#if 0
void CLS_Draw::DrawStringEx(int x, int y, int FontSytle, unsigned long FontColor,
	unsigned long BackColor, char* _cChar, BYTE* btSurface, int _iDist, int _iPixMode)
{
	//放大系数iPx, iPy
	char cChar[3] = { 0 };
	int iTemWidth = 0;
	int iChNum = 0;
	int ix, iy;
	int iPx = m_ImageWidth / 352;
	int iPy = m_ImageHeight / 288;
	iPx = (iPx) ? iPx : 1;
	iPy = (iPy) ? iPy : 1;

	int i = 0;

	int iPos = x;         /*点阵显示的位置*/

	unsigned char *buf;
	const int Mask[] = { 0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01 };

	buf = (UCHAR*)_cChar;
	int iSum = 0;

	RECT rc;
	rc.bottom = 16;
	rc.left = 0;
	rc.right = 16;
	rc.top = 0;
	CString strText;
	HDC hDC = ::GetDC(NULL);
	HDC   memDC = CreateCompatibleDC(hDC);
	HBITMAP   memBM = CreateCompatibleBitmap(hDC, rc.right, rc.bottom);
	SelectObject(memDC, memBM);
	HBRUSH  hBrush = CreateSolidBrush(RGB(255, 255, 255));
	SelectObject(memDC, hBrush);
	//增加字体设置
	HFONT   hFont = CreateFont(0, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "宋体");
	HFONT   hOldFont = (HFONT)SelectObject(memDC, hFont);

	/*一个字一个字的显示，首先判断是全角或半角，如为全角则读全角字库，如为半角则
	读半角字库*/
	while (buf[i])
	{
		iSum++;
		if (iSum>64)break;
		if ((buf[i] >= 0xa0) && (buf[i + 1] >= 0xa0))
		{
			iTemWidth = 16;
			iChNum = 2;

			cChar[0] = buf[i];
			cChar[1] = buf[i + 1];

		}
		else if (buf[i]<0xa0)
		{
			iTemWidth = 8;
			iChNum = 1;

			cChar[0] = buf[i];
		}

		strText = cChar;

		::FillRect(memDC, &rc, hBrush);//填充内存DC上的图象
		TextOut(memDC, 0, 0, strText, strText.GetLength());

		for (int iWidth = 0; iWidth<iTemWidth; iWidth++)
		{
			for (int iLength = 0; iLength<16; iLength++)
			{
				if (GetPixel(memDC, iWidth, iLength) == RGB(0, 0, 0) || BackColor != 0)
					for (ix = 0; ix<iPx; ix++)
					{
						for (iy = 0; iy<iPy; iy++)
						{
							DrawDotEx(iPos + iWidth*iPx + ix, y + iLength*iPy + 2 * iChNum, FontColor, btSurface, _iDist, _iPixMode);
						}
					}
			}
		}
		iPos += iTemWidth*iPx;
		i += iChNum;
	}
	DeleteObject(hFont);
	DeleteObject(hBrush);
	DeleteObject(memBM);
	DeleteDC(memDC);
	::ReleaseDC(NULL, hDC);
}
#else

#endif
//---------------------------------------------------------------------------
void Drawyuv::DrawDotEx(int _x, int _y, unsigned long u32Color, BYTE* btSurface,
	int _iDist, int _iPixMode)
{
	int x = _x, y = _y;
	int dist = _iDist*_iPixMode;
	BYTE* btOff = btSurface + dist*y + x*_iPixMode;
	btOff[0] = (BYTE)((u32Color & 0x00ff0000) >> 16);
	btOff[1] = (BYTE)((u32Color & 0x0000ff00) >> 8);
	btOff[2] = 0;
	btOff[dist] = 0;
	btOff[3] = (BYTE)(u32Color & 0x000000ff);
}
void Drawyuv::DrawI420(BYTE  * src, BYTE *dst)
{
	if (src == NULL || dst == NULL)
		return;
	if (m_iNum == 1)
	{
		DrawI420_rgb(src, dst);
	}
	else if (m_iNum == 9)
	{
		DrawI420_yuv(src, dst);
	}
}

void Drawyuv::yuv2rgb(uint8_t *puc_y, int stride_y,
	uint8_t *puc_u, uint8_t *puc_v, int stride_uv,
	uint8_t *puc_out, int width_y, int height_y,
	unsigned int _stride_out)
{
	if (m_iCount == 2)
	{
		yuv2rgb_565(puc_y, stride_y, puc_u, puc_v, stride_uv, puc_out, width_y, height_y, _stride_out);
	}
	else if (m_iCount == 3)
	{
		yuv2rgb_24(puc_y, stride_y, puc_u, puc_v, stride_uv, puc_out, width_y, height_y, _stride_out);
	}
	else if (m_iCount == 4)
	{
		yuv2rgb_32(puc_y, stride_y, puc_u, puc_v, stride_uv, puc_out, width_y, height_y, _stride_out);
	}
}

/************************************************************************
*	DrawLine + DrawArrow + DrawPolygon, for VCA
************************************************************************/
#define PI 3.1415926
#define A2R(x)	(x*PI/180.0)
#define R2A(x)	(x*180.0/PI)

int Drawyuv::DrawLine(const int iXStart, const int iYStart, const int iXEnd, const int iYEnd,
	const unsigned long ulFrontColor, const unsigned long ulBackColor, BYTE* btSurface)
{
	return 0;
}

int Drawyuv::DrawArrow(const int iXStart, const int iYStart, const int iXEnd, const int iYEnd,
	const unsigned long ulFrontColor, const unsigned long ulBackColor, BYTE* btSurface)
{
	return 0;
}

int Drawyuv::DrawPolygon(const POINT* _ptArray, const int _iCount,
	const unsigned long ulFrontColor, const unsigned long ulBackColor, BYTE* btSurface, const int _iEnableArrow)
{
	return 0;
}

void Drawyuv::DrawRect(RECT rcRect, unsigned long FrontColor,
	unsigned long BackColor, BYTE* btSurface)
{
	if (rcRect.bottom < 0 || rcRect.left < 0 || rcRect.right < 0 || rcRect.top < 0)            //zyp add 2009-11-25
		return;

	if (0 == rcRect.bottom && 0 == rcRect.left && 0 == rcRect.right && 0 == rcRect.top)       //zyp add 2009-11-25
		return;

	int i = 0, j = 0;
	for (j = rcRect.top; j<rcRect.bottom; j++)
	{   //左
		int iRet = DrawDotRect(rcRect.left, j, FrontColor, btSurface);
		if (iRet < 0)
			break;
	}
	for (i = rcRect.left + 1; i<rcRect.right; i++)
	{   //上
		int iRet = DrawDotRect(i, rcRect.top, FrontColor, btSurface);
		if (iRet < 0)
			break;
	}
	for (j = rcRect.top; j<rcRect.bottom + 1; j++)
	{   //右
		int iRet = DrawDotRect(rcRect.right, j, FrontColor, btSurface);
		if (iRet < 0)
			break;
	}
	for (i = rcRect.left; i<rcRect.right + 1; i++)
	{   //下
		int iRet = DrawDotRect(i, rcRect.bottom, FrontColor, btSurface);
		if (iRet < 0)
			break;
	}
}
//---------------------------------------------------------------------------
int Drawyuv::DrawDotRect(int _x, int _y, unsigned long u32Color, BYTE* btSurface)
{
	if (_x < 0 || _y < 0 || (0 == _x && 0 == _y))
		return 0;

	if ((DWORD)_x > m_ddSurface.dwWidth || (DWORD)_y > m_ddSurface.dwHeight || 0x0077FFFF != u32Color || m_ddSurface.lpSurface != btSurface)       //zyp add 2009-11-23
	{
		log_err(1, "[CLS_Draw::DrawDotRect] x=%d,y=%d,u32Color=%d,btSurface=0x%08x,this=0x%08x", _x, _y, u32Color, btSurface, this);
		return 0;
	}

	__try
	{
		int x = _x;
		int y = _y;
		int dist = m_ddSurface.lPitch;
		int iOffSet = dist * y + x * m_iCount;                                    //起始内存偏移量  	
		BYTE* btOff = btSurface + iOffSet;
		unsigned int u32ColorY = ((u32Color & 0x00ff0000) >> 16);

		//int iMaxOffSet = m_ddSurface.dwWidth * m_ddSurface.dwHeight * m_iCount;   //最大的内存偏移量             zyp add 2009-11-23

		if (m_iNum == 9)
		{
			// 		int iFirstIdlePos = iOffSet + dist;                                    //zyp add 2009-11-23 检测内存是否越界
			// 		if(iFirstIdlePos >= iMaxOffSet)
			// 			return;

			btOff[0] = u32ColorY;
			btOff[1] = (BYTE)((u32Color & 0x0000ff00) >> 8);
			btOff[2] = (BYTE)btOff[0];
			btOff[dist] = 0;
			btOff[3] = (BYTE)(u32Color & 0x000000ff);
		}
		else
		{
			if (m_iCount == 4)
			{
				// 			int iFirstIdlePos = iOffSet + dist + 3;
				// 			if(iFirstIdlePos >= iMaxOffSet)
				// 				return;

				btOff[0] = (BYTE)u32ColorY;
				btOff[1] = (BYTE)u32ColorY;
				btOff[2] = (BYTE)u32ColorY;
				btOff[3] = (BYTE)u32ColorY;
				btOff[4] = 0;
				btOff[5] = 0;
				btOff[6] = 0;
				btOff[7] = 0;
				btOff[dist + 0] = 0;
				btOff[dist + 1] = 0;
				btOff[dist + 2] = 0;
				btOff[dist + 3] = 0;
			}
			else if (m_iCount == 3)
			{
				// 			int iFirstIdlePos = iOffSet + dist + 2;
				// 			if(iFirstIdlePos >= iMaxOffSet)
				// 				return;

				btOff[0] = u32ColorY;
				btOff[1] = u32ColorY;
				btOff[2] = u32ColorY;
				btOff[4] = 0;
				btOff[5] = 0;
				btOff[6] = 0;
				btOff[dist + 0] = 0;
				btOff[dist + 1] = 0;
				btOff[dist + 2] = 0;
			}
			else if (m_iCount == 2)
			{
				// 			int iFirstIdlePos = iOffSet + dist + 1;
				// 			if(iFirstIdlePos >= iMaxOffSet)
				// 				return;

				btOff[0] = u32ColorY;
				btOff[1] = u32ColorY;
				btOff[2] = 0;
				btOff[3] = 0;
				btOff[dist + 0] = 0;
				btOff[dist + 1] = 0;
			}
		}
	}
	__except (1)
	{
		log_info(1, "[CLS_Draw::DrawDotRect] failed, x=%d,y=%d,u32Color=%d,btSurface=0x%08x,this=0x%08x", _x, _y, u32Color, btSurface, this);
		return -1;
	}

	return 0;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------


/*---------------------------------------------------------------------------
以下是新的叠加字符算法，可以降低运算次数，使用时需要在解码端判断是否需要
重新取点整信息（信息内容、位置改变，视频大小改变都要重新获取点整信息
//---------------------------------------------------------------------------*/
//新方法叠加字符,_bFlag=1只记录点的位置，=0则画点(默认)
void Drawyuv::cDrawDot(int _ix, int _iy, unsigned long _u32Color,
	BYTE* _btDst, int _iDisp, int _iBits, int _bFlag)
{
	int x = _ix, y = _iy;
	BYTE* btOff = _btDst + _iDisp*y + x*_iBits;
	unsigned int u32Color = ((_u32Color & 0x00ff0000) >> 16);

	if (_bFlag)
	{
		btOff[0] = u32Color;
		btOff[1] = 0;
		btOff[_iDisp] = 0;
		return;
	}

	if (m_iNum == 9)
	{
		btOff[0] = u32Color;
		btOff[1] = (BYTE)((_u32Color & 0x0000ff00) >> 8); //(((_u32Color&0x0000ff00)>>8)+btOff[1])>>1;
		btOff[3] = (BYTE)(_u32Color & 0x000000ff);    //((_u32Color&0x000000ff)+btOff[3])>>1;
	}
	else
	{
		if (m_iCount == 4)
		{
			btOff[0] = u32Color;
			btOff[1] = u32Color;
			btOff[2] = u32Color;
			btOff[3] = u32Color;
		}
		else if (m_iCount == 3)
		{
			btOff[0] = u32Color;
			btOff[1] = u32Color;
			btOff[2] = u32Color;
		}
		else if (m_iCount == 2)
		{
			btOff[0] = u32Color;
			btOff[1] = u32Color;
		}
	}
}

//---------------------------------------------------------------------------
#if 0

#else

#endif
//---------------------------------------------------------------------------
//获得叠加字符的点阵信息


//---------------------------------------------------------------------------
void Drawyuv::cDrawOSD(int _iVideoWidth, int _iVideoHeight, int* _u32Dot, unsigned long _FontColor, char* _u8Dst, int _iCount)
{
	int i;
	int count;

	int u32Info, u32Pos;
	int x, y;
	unsigned int u32Color;

	count = (_iCount >> 1) << 1;
	for (i = 0; i<count; i++)
	{
		u32Info = _u32Dot[i];
		u32Color = u32Info >> 30;
		u32Pos = u32Info & 0x3FFFFFFF;
		x = u32Pos >> 15;
		y = u32Pos & 0x7FFF;

		u32Color *= (_FontColor & 0x00ff0000);
		u32Color |= _FontColor & 0x0FFFF;
		cDrawDot(x, y, u32Color, (BYTE*)_u8Dst, m_ddSurface.lPitch, m_iCount);
	}
}


int	Drawyuv::IsValidVideoRect(const RECT & _rctVideo) const
{
	return (_rctVideo.left >= 0 && _rctVideo.right <= (int)m_ImageWidth && _rctVideo.top >= 0 && _rctVideo.bottom <= (int)m_ImageHeight);
}




int Drawyuv::ChangeDrawWnd(HWND _hWnd)
{
	if (FAILED(m_pcClipper->SetHWnd(0, _hWnd)))
	{
		log_debug(0, "[CLS_Draw::ChangeDrawWnd] failed, HWND(%d)", _hWnd);
		return -1;
	}

	m_lpDDSPrimary->SetClipper(m_pcClipper);

	return 0;
}

int Drawyuv::SetDrawPolygon(POINT* _ptArray, int _iCount, int _iEnableArrow)
{
	return 0;//m_polygon.Set(_ptArray, _iCount, _iEnableArrow);
}