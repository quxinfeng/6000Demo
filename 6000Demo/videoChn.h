#pragma once
#include "Hih264Decoder.h"
#include "com_fun.h"
#include "d3ddraw.h"

class videoChn
{
public:
	int				m_iChnNo;  
	int				m_iChnType;
	TChnOverlay		m_TOverLay[3];
	VideoParam		m_tVideopara;
	

	SOCKET			*m_pCmdSocket;
	SOCKET			m_iDataSocket;
	int				m_iDataLiveTime;
	int				m_iConnectStat;
	int				m_iDataSize;			// ��ʵ�����ݳ���
	int				m_iType;				// ��������
	int				m_bNewFrame; 			//�Ƿ�Ϊ�µ�һ֡����
	int             m_iDataRecvLen;                         //��֡�յ������ݳ���
	int             m_bStartLost;                       //�Ƿ��ѿ�ʼ��֡
	int             m_iFrameType;			//��ǰ֡����
	int				m_ulFrameNo;			//��ǰ֡���
	int 			m_iRcvFramNum;
	int 			m_iFramLostNum;
	char			*m_cData;
	char			*m_RcvVideoData;
	int				m_iRecvLen;
	
	char			*m_cVideoData;
	int				m_iVideoDatalen;
	int				m_iVideoRecFramNo;
	int				m_iVideoDecFramNo;
	CCriticalSection m_iLock;
	char			*m_cServerIp;
	
	Hih264Decoder	m_tDecoder;
	unsigned char	*yuv;

	TChnvo			m_tVo;
	
	d3ddraw			m_tD3d;
public:
	bool ChnRecevedata(char *_data, int _iBuflength);
	int ChnSendConnect(int _iChn, int _iSub, int _iNetMode, HWND hMainwnd);
	void AnalysisData(char *_ucData, int _iLen);
	int  AnalyzeCmd(char * _cBuf, int _iLen);
	int  Recv_Command(char * _cBuf, int _iLen);
	
	int AVData_WriteBuffer(char *_ucData, int _iLen);
	int ChnDisConFromServer();
	int cSendMsg(SOCKET _iSocket, char *_cMsg, int _iLen);
	void ChnForceIframe();
	void SetChnVideosize(int _iVideosize);
	void SetChnFramerate(int _iFramerate);
	void SetChnBitrate(int _iBitrate);
	void SetChnIframerate(INT _iIFramerate);
	void SetChnVideoquality(int _iVideoQuality);
	void SetChnEncodetype(int _iEncodeType);
	void SetChnVideopara(int _iBrightness, int _iConstant, int _iHue, int _isaturation);
	videoChn();
	~videoChn();


};

