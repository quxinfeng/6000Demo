#pragma once
#include <ddraw.h>

#define MAX_RECT_COUNT  396
#define uint8_t unsigned char
#define int32_t long

/*显示时需要的参数结构*/
typedef struct DrawNeed
{
	DWORD          dwDrawMask;
	RECT           DrawRect[32];
}*LPDrawNeed;

struct lookuptable
{
	int32_t m_plY[256];
	int32_t m_plRV[256];
	int32_t m_plGV[256];
	int32_t m_plGU[256];
	int32_t m_plBU[256];
};
class Drawyuv
{
protected:
	
	lookuptable m_lut;
	int     m_iCount;
	int     m_ddWidthCal;
	int     m_iOff;
	int     m_iNum;
	int     m_iLockTimes;
	//	显示视频相关变量
	
	unsigned char *m_bufRGB;

	LPDIRECTDRAW7			m_lpDirectDraw1;
	LPDIRECTDRAWSURFACE7	m_lpDDSPrimary;	    // DirectDraw primary surface
	LPDIRECTDRAWSURFACE7	m_lpDDSOne;	        // Offscreen surface 1
	LPDIRECTDRAWCLIPPER		m_pcClipper;		//	clipper used to set display window
	DDSURFACEDESC2			m_ddSurface;

	CRITICAL_SECTION		m_csDraw;

	DWORD	m_ddColorKey;
	UINT    m_ImageWidth;
	UINT    m_ImageHeight;
	DWORD   m_Voffset;
	DWORD   m_Uoffset;
public:
	//	多屏显示
	HWND	m_hDisplayWnd;
	int		m_iUsed;
	unsigned int     m_uiMonitorIndex;		//  [12/18/2009 ym]multi monitor
	bool             m_blMainMonitor;

public:
	Drawyuv();
	~Drawyuv();

	BOOL  DirectDrawYUV(unsigned char * src);//,DecoderNeed* _Param);
											 //    BOOL  DirectDrawYUVEx(unsigned char * src,DecoderNeed* _Param, unsigned char* dst,int _iDist,int _iPixMode);
	void  ReleaseDirectDraw(void);
	int   InitDraw(UINT Width, UINT Height, DWORD ColorKey = 24);
	int   CreateDirectDraw(HWND hwnd/*,DWORD ColorKey*/, unsigned int _uiMonitorIndex = 0);

	int   SetDrawWnd(HWND hwnd, bool _blReplace = true);
	int	  GetDrawWnd(HWND& _hWnd) { _hWnd = m_hDisplayWnd; return 0; }
	int   SetDrawMonitor(unsigned int _uiMonitorIndex) { m_uiMonitorIndex = _uiMonitorIndex;	return 0; }

	
	int		ChangeDrawWnd(HWND _hWnd);
	int		IsValidVideoRect(const RECT & _rctVideo) const;

	int		SetDrawPolygon(POINT* _ptArray, int _iCount, int _iEnableArrow);

public:
	void DrawI420(BYTE  * src, BYTE *dst);
	void yuv2rgb(uint8_t *puc_y, int stride_y,
		uint8_t *puc_u, uint8_t *puc_v, int stride_uv,
		uint8_t *puc_out, int width_y, int height_y,
		unsigned int _stride_out);
	int DrawDotRect(int _x, int _y, unsigned long u32Color, BYTE* btSurface);
	void DrawDot(int x, int y, unsigned long u32Color, BYTE* btSurface);
	void DrawDotEx(int x, int y, unsigned long u32Color, BYTE* btSurface, int _iDist, int _iPixMode);

	void DrawString(int x, int y, int FontSytle, unsigned long FontColor,
		unsigned long BackColor, char* cChar, BYTE* btSurface);
	void DrawStringEx(int x, int y, int FontSytle, unsigned long FontColor,
		unsigned long BackColor, char* cChar, BYTE* btSurface, int _iDist, int _iPixMode);
	void DrawRect(RECT rcRect, unsigned long FrontColor,
		unsigned long BackColor, BYTE* btSurface);

	//  画线，2010-7-22-10:37 @yys@
	int DrawLine(const int iXStart, const int iYStart, const int iXEnd, const int iYEnd, const unsigned long ulFrontColor, const unsigned long ulBackColor, BYTE* btSurface);
	//	画多边形
	int DrawPolygon(const POINT* _ptArray, const int _iCount, const unsigned long ulFrontColor, const unsigned long ulBackColor, BYTE* btSurface, const int _iEnableArrow);
	//	画箭头
	int DrawArrow(const int iXStart, const int iYStart, const int iXEnd, const int iYEnd, const unsigned long ulFrontColor, const unsigned long ulBackColor, BYTE* btSurface);

	void DrawI420_rgb(BYTE  * src, BYTE *dst);
	void DrawI420_yuv(BYTE  * src, BYTE *dst);
	void DrawI420_yuvEx(BYTE  * src, BYTE *dst, int _iDist, int _iPixMode);
	void init_yuv2rgb();
	void yuv2rgb_32(uint8_t *puc_y, int stride_y,
		uint8_t *puc_u, uint8_t *puc_v, int stride_uv,
		uint8_t *puc_out, int width_y, int height_y,
		unsigned int _stride_out);

	void yuv2rgb_565(uint8_t *puc_y, int stride_y,
		uint8_t *puc_u, uint8_t *puc_v, int stride_uv,
		uint8_t *puc_out, int width_y, int height_y,
		unsigned int _stride_out);

	void    cDrawDot(int _ix, int _iy, unsigned long _u32Color,
		BYTE* _btDst, int _iDisp, int _iBits, int _bFlag = 0);
	void    cDrawString(int _iImageWidth, int _iImageHeight, int _x, int _y,
		unsigned long _FontColor, char* _cChar, char* _u8Dst, int _iDist);
	int     GetChnTitlePixel(int _iWidth, int _iHeight, int _x, int _y,
		char* _cChar, int *_u32Info, int *_iCount);
	void    cDrawOSD(int _iVideoWidth, int _iVideoHeight, int* _u32Dot,
		unsigned long _FontColor, char* _u8Dst, int _iCount);

	int		Inner_DrawExtraInfo(DrawNeed* _Param, BYTE* btSurface);

	void AdjustDrawScale(int _imgWidth, int _imgHeight, RECT* _pDrawRect);

	void yuv2rgb_24(uint8_t *puc_y, int stride_y,
		uint8_t *puc_u, uint8_t *puc_v, int stride_uv,
		uint8_t *puc_out, int width_y, int height_y,
		unsigned int _stride_out);
};

