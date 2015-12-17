#pragma once
#include "videoChn.h"
#define MAX_USER   8
#define USERNAME_LEN  16
#define PASSWORD_LEN  16
#define RECVBUFSIZE 4096
#define MAX_CHN		5
#define MAX_VODEV   3

typedef enum
{
	CHN_TYPE_TEST = 0x000,		//测试用的通道类型
	CHN_TYPE_SDIPCI = 0x50,		    //sdi从片通道
	CHN_TYPE_SDILOCAL = 0x100,		//sdi主片通道
	CHN_TYPE_VC = 0x200,		//VO合成通道
	CHN_TYPE_IP = 0x300,		//数字通道
} CHANNEL_TYPE;

typedef struct  
{
	int iVodev;
	int iPicCnt;
	int iChnlist[MAX_CHN];
}TPreviewPara;

typedef struct
{
	char m_cUserName[USERNAME_LEN];
	char m_cPassword[PASSWORD_LEN];
}TUser;
class Client
{
public:
	char m_cStrServerIp[16];
	int m_iServerPort;
	char m_cUserName[USERNAME_LEN];
	char m_cPassWord[PASSWORD_LEN];

	unsigned char m_cLoginKey[8];

	int m_iLoginstat;
	int m_iConStat;
	int m_iFailReason;

	int m_iChnnum;
	char m_strVersion[32];
	int iProductModel;
	char m_cFatoryId[32];
	int m_iCmdId;
	int m_iHeartIntval;
	BOOL m_blRtmpEnable;
	char m_strRtmpserver[64];
	int m_iVodevSize[2];



	int m_iUserNum;
	int m_iMaxUserNum;
	TUser m_cUser[MAX_USER];
	videoChn m_tVChn[MAX_CHN];

	TPreviewPara m_tVodev[MAX_VODEV];

	int m_iDiskTotalNum;
	TdiskINfo m_tDiskInfo[MAX_DISK];
public:
	Client();

	~Client();
private:
	bool m_blInitFlg;
	bool m_blUpdate;
	// socket句柄
	
	
public:
	//void ClientInit();
	SOCKET  m_icmdSocket;
	int m_iCmdLiveTime;
	
	int m_iLoglevel;
	int m_iLogSize;

	int m_iRecvLen;
	char m_cDataBuf[4096];
	char m_cProductModel[16];
	char m_cFactoryID[32];

	char m_cnetMask[16];
	char m_cGateWay[16];
	char m_cDns[16];
	char m_cBackDns[16];
	
	HWND m_cMainHwnd;
	HWND m_cLogHwnd;
	
	int m_iLogTatalNum;
	int m_iLogShowNum;
	int m_blLang;
	LogItem m_tLogItem[10];
public:
	
	bool GetClientInitFlg();
	bool GetClientUpdateFlg();
	bool SetClientUpdateFlg(bool _blUpdateFlg);


	bool ClientRecevecmd(char *_data, int _iBuflength);
	bool ClientGetpara(char *_data, int _iBuflength);
	bool ClientGetsetPararsp(char *_data, int _iBuflength);
	bool ClientGetCmdRsp(char *_data, int _iBuflength);
	bool ClientGetUserpara(char *_data, int _iBuflength);
	bool ClientLoginRsp(char *_data, int _iBuflengt);
	void ClientParseData(char *_data, int _iBuflength);


	int ClientLogonServer(char *_StrServerIp, int _iServerPort, char *_StrUserName, char *_StrPassWord);
	int ClientLogoffFromServer();
	
	int SendCmdStringtoServer(char *_cMsg, int _iBuflength);


	int ClientSetPreviewpara(int _iVodev,int iCnt, int *iChn);
	int ClientSetVodevSize(int _iVodev, int _iVodevSize);
	int ClientConnectChn(int _iChn, int _iSub, int _iNetMode,HWND _hWnd);


	int ClientSetOsdChnName(int _iChn, char *strtext);
	int ClientSetOsdOverlay(int _iChn, char *strtext);
	int ClientSetOsdDisplay(int _iChn, int _iOsdType, BOOL _blDisplay);
	int ClientSetOsdColor(int _iChn, int _iOsdType, int _iColor);
	int ClientSetOsdPos(int _iChn, int _iOsdType, int _iX, int _iy);

	int ClientSearchLog(long _iStartTm, long _iEndTm, int _ilogType,int _iLang, int _iPagesize, int _iPageNo, int _iChannel);
	int ClientLogClear();

	int ClietnSetFocus(int _iVodev,int _iAudioChn);
};

