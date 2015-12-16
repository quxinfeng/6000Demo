#pragma once
#include "hi_config.h"
#include "hi_h264api.h"


typedef HI_HDL(*pfHi264DecCreate)(H264_DEC_ATTR_S *pDecAttr);

typedef HI_S32(*pfHi264DecFrame) (HI_HDL  hDec,
	HI_U8*  pStream,
	HI_U32  iStreamLen,
	HI_U64  ullPTS,
	H264_DEC_FRAME_S *pDecFrame,
	HI_U32  uFlags);
typedef void(*pfHi264DecDestroy) (HI_HDL hDec);
typedef HI_S32(*pfHi264DecAU)(
	HI_HDL hDec,
	HI_U8 *pStream,
	HI_U32 iStreamLen,
	HI_U64 ullPTS,
	H264_DEC_FRAME_S *pDecFrame,
	HI_U32 uFlags);

#define DLL_API extern "C" __declspec(dllexport)

class Hih264Decoder
{
public:
	HMODULE m_hDll;
	HI_S32 m_iend;
	pfHi264DecCreate  m_pHi264DecCreate;
	pfHi264DecFrame   m_pHi264DecFrame;
	pfHi264DecDestroy m_pHi264DecDestroy;
	pfHi264DecAU		m_pHi264DecAu;
	HI_HDL m_hHandle;
	//HWND hMainFramewnd;
	H264_DEC_FRAME_S m_tdec_frame;
public:
	int  initHIH264();
	int DecoderCreate(int _iWidth, int _iHeight);
	int DecoderDestroy();
public:
	Hih264Decoder();
	~Hih264Decoder();
};

