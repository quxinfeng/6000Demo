#pragma once
#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")

class d3ddraw
{
private:
	HWND				m_iWnd;
	unsigned long		m_iWidth;
	unsigned long		m_iHeight;
	CRITICAL_SECTION	m_critial;
	IDirect3D9			*m_pDirect3D9;
	IDirect3DDevice9	*m_pDirect3DDevice;
	IDirect3DSurface9	*m_pDirect3DSurfaceRender;
	RECT				m_rtViewport;
public:
	int InitD3D(HWND hwnd, unsigned long lWidth, unsigned long lHeight);
	void Cleanup();
	int d3dDrawyuv(unsigned char *yuv);
	d3ddraw(void);
	~d3ddraw(void);
};



