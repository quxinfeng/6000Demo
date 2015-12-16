#include "stdafx.h"
#include "Client.h"
#include "socket.h"
#include "com_fun.h"
#include "encrypt.h"
#include "videoChn.h"


Client::Client()
	: m_icmdSocket(0)
{
	m_blInitFlg = false;
	m_blUpdate = false;
	m_iLoginstat = 0;
	m_iFailReason = 0;
	m_iChnnum = 0;
	m_iRecvLen = 0;
	m_iCmdId = 0;
	m_iHeartIntval = 0;
	
	m_icmdSocket = -1;
	
	m_iCmdLiveTime = 0;
	memset(m_cDataBuf, 0, sizeof(m_cDataBuf));
	memset(m_tVodev, 0, 3*sizeof(TPreviewPara));
	
	
	m_iConStat = 0;
	
	int i;
	for (i = 0; i < MAX_CHN; i++)
	{
		
		m_tVChn[i].m_iChnNo = i;
		m_tVChn[i].m_pCmdSocket = &m_icmdSocket;
		m_tVChn[i].m_cServerIp = m_cStrServerIp;
		
	}
}


Client::~Client()
{
}


int Client::ClientLogonServer(char *_StrServerIp, int _iServerPort, char *_StrUserName, char *_StrPassWord)
{
	m_blInitFlg = true;
	char cMsg[2048] = { 0 };
	if (_StrServerIp != NULL && strlen(_StrServerIp) < 15)
	{
		strcpy_s(m_cStrServerIp, _StrServerIp);
	}
	if (_iServerPort > 0 && _iServerPort < 65535)
	{
		m_iServerPort = _iServerPort;
	}

	if (_StrUserName != NULL && strlen(_StrUserName) < 15)
	{
		strcpy_s(m_cUserName, _StrUserName);
	}
	if (_StrPassWord != NULL && strlen(_StrPassWord) < 15)
	{
		strcpy_s(m_cPassWord, _StrPassWord);
	}
	
	m_icmdSocket = CreateSocket(m_cStrServerIp, m_iServerPort + 1);
	if (m_icmdSocket <= 0)
	{
		m_blInitFlg = false;
		return -1;
	}

	CWinThread* pThread = AfxBeginThread(ThreadReceiveData, this,0,0,0);
	if (pThread == NULL)
	{
		DWORD dw = GetLastError();
		TRACE("ThreadReceiveData failed dw =%d \n", dw);
	}

	Sleep(10);
	
	sprintf_s(cMsg, "%s%s%s%s%s%s%s%s%d%s%s%s", m_cStrServerIp, "\tIP\t", "USER",TAB,"LOGON", TAB, "DIRECT", TAB, 0, TAB, "GB2312", END);

	int iRet = SocketSendbuf(m_icmdSocket, cMsg, strlen(cMsg), 0);
	if (0 >= iRet)
	{
		TRACE("send error\n");
		return -1;
		//m_ClientSock[j].m_RebootContion = 0;
		_asm int 3;
		int iError = WSAGetLastError();  
		return false;//

	}
	else
	{
		TRACE("send sucedd\n");
	}
	
	return true;
}

bool Client::GetClientInitFlg()
{
	return m_blInitFlg;
}
bool  Client::GetClientUpdateFlg()
{
	return m_blUpdate;
}

bool Client::SetClientUpdateFlg(bool _blUpdateFlg)
{
	m_blUpdate = _blUpdateFlg;
	return true;
}
void Client::ClientParseData(char *_data, int _iBuflength)
{
	unsigned int 	ui32Head;
	STREAMHEAD  dataHeader;
	COMMANDHEAD cmdHeader;
	int iLen = 0;
	
	//首先将数据拷贝到接收缓冲中
	if ((m_iRecvLen + _iBuflength)>RECVBUFSIZE)
	{

		memcpy(m_cDataBuf, _data, _iBuflength);
		m_iRecvLen = _iBuflength;
	}
	else
	{

		memcpy(m_cDataBuf + m_iRecvLen, _data, _iBuflength);
		m_iRecvLen += _iBuflength;
	}
	while ((m_iRecvLen - iLen)> sizeof(unsigned int))
	{
		/*
		*可能出现"CONNECT" "DISCONNECT" 或"LOGON""LOGOFF"命令短时间间隔发送
		*这样命令会存在此缓冲里循环解析,造成逻辑错误,所以在每条命令
		*解析之前,判断任务删除标志
		*/
		
		memcpy(&ui32Head, m_cDataBuf + iLen, 4);
		if (ui32Head == COMMPACKHEAD)
		{
			if ((m_iRecvLen - iLen) < sizeof(COMMANDHEAD))
			{
				break;
			}
			memset(&cmdHeader, 0, sizeof(COMMANDHEAD));
			memcpy(&cmdHeader, m_cDataBuf + iLen, sizeof(COMMANDHEAD));
			if (cmdHeader.m_ui16CommandSize > (m_iRecvLen - iLen))
			{
				break;
			}
			if (cmdHeader.m_ui16CommandSize <= 0)
			{
				iLen += 1;
				continue;
			}
			
			if ((cmdHeader.m_ui16CommandSize - sizeof(COMMANDHEAD))>0)
			{
				ClientRecevecmd(m_cDataBuf + iLen + sizeof(COMMANDHEAD),
					cmdHeader.m_ui16CommandSize - sizeof(COMMANDHEAD));
			}
			
			iLen += cmdHeader.m_ui16CommandSize;
		}
		else if (ui32Head == DATAPACKHEAD)
		{
			if ((m_iRecvLen - iLen) < sizeof(STREAMHEAD))
			{
				break;
			}
			memset(&dataHeader, 0, sizeof(STREAMHEAD));
			memcpy(&dataHeader, m_cDataBuf + iLen, sizeof(STREAMHEAD));
			if (dataHeader.m_ui16Size > (m_iRecvLen - iLen))
			{
				break;
			}
			if (dataHeader.m_ui16Size <= 0)
			{
				iLen += 1;
				continue;
			}
			
			if ((dataHeader.m_ui16Size - sizeof(STREAMHEAD))>0)
			{
			//	ClientRecevedata(m_cDataBuf + iLen + sizeof(STREAMHEAD),
			//		dataHeader.m_ui16Size - sizeof(STREAMHEAD));
			}
			
			iLen += dataHeader.m_ui16Size;
		}
		else
		{
			iLen += 1;
		}
	}
	//将剩余数据前移,并更新数据指针
	if (m_iRecvLen > iLen)
		memmove(m_cDataBuf, m_cDataBuf + iLen, m_iRecvLen - iLen);
	m_iRecvLen -= iLen;
	if (m_iRecvLen < 0) m_iRecvLen = 0;

}

bool Client::ClientRecevecmd(char *_data, int _iBuflength)
{
	
	int i = 0;
	char cMsg[2048] = { 0 };
	memcpy(cMsg, _data, _iBuflength);
	TRACE("%s \n", cMsg);
	OnCharRight(cMsg, "\t", cMsg);

	OnCharRight(cMsg, "\t", cMsg);
	OnCharLeft(cMsg, "\n\n\n", cMsg);
	char* cInnerCmd[48] = { NULL };
	char  cTmp[48 * 48] = { 0 };
	if (cInnerCmd[0] == NULL)
	{
		for (i = 0; i < 48; i++)
		{
			cInnerCmd[i] = &cTmp[i * 48];
		}
	}
	//	int iRet;
	for (i = 0; i < 32; i++)
	{
		memset(cInnerCmd[i], 0, 32);
	}

	strcat_s(cMsg, "\t");
	if (SplitString((unsigned char*)cMsg, (unsigned char*)"\t", (unsigned char**)cInnerCmd, 32, 32) < 0)
		return false;
	TRACE("cInnerCmd = %s %s %s \n", cInnerCmd[0], cInnerCmd[1], cInnerCmd[2], cInnerCmd[3]);
	if (strcmp(cInnerCmd[0], "INNER") == 0)
	{
		if (strcmp(cInnerCmd[1], "LOGON") == 0)
		{
			OnCharRight(cMsg, "\t", cMsg);
			OnCharRight(cMsg, "\t", cMsg);
			ClientLoginRsp(cMsg, _iBuflength);
			

		}
		else if(strcmp(cInnerCmd[1], "PARAGET") == 0)
		{
			OnCharRight(cMsg, "\t", cMsg);
			OnCharRight(cMsg, "\t", cMsg);
			ClientGetpara(cMsg, _iBuflength);
			
		}
		else if(strcmp(cInnerCmd[1], "SUPER") == 0)
		{
			OnCharRight(cMsg, "\t", cMsg);
			OnCharRight(cMsg, "\t", cMsg);
			ClientGetUserpara(cMsg, _iBuflength);
			
		}
		else if (strcmp(cInnerCmd[1], "VERSION") == 0)
		{
			strcpy_s(m_strVersion, cInnerCmd[2]);
			::PostMessage(m_cHwnd, WM_MY_REFRESHVERSION, 0, 0);
		}
		else if (strcmp(cInnerCmd[1], "CMDID") == 0)
		{
			m_iCmdId = atoi(cInnerCmd[2]);
			TRACE("m_iCmdId = %d \n", m_iCmdId);

			
		}
		else  if(strcmp(cInnerCmd[1], "HEADER") == 0)
		{
			int iChannelNo = atoi(cInnerCmd[2]);
			m_tVChn[iChannelNo].m_tVideopara.m_iFrameRate = atoi(cInnerCmd[3]);
			m_tVChn[iChannelNo].m_tVideopara.m_VideoDim.m_iWidth = atoi(cInnerCmd[4]);
			m_tVChn[iChannelNo].m_tVideopara.m_VideoDim.m_iHeight = atoi(cInnerCmd[5]);
			m_tVChn[iChannelNo].m_iConnectStat = 3;
		}
		else if(strcmp(cInnerCmd[1], "PARASET") == 0)
		{
			OnCharRight(cMsg, "\t", cMsg);
			OnCharRight(cMsg, "\t", cMsg);
			ClientGetsetPararsp(cMsg, _iBuflength);
		}
		else
		{
			TRACE("cInnerCmd[1] = %s \n", cInnerCmd[1]);
			TRACE("INNER unrecgnised \n");
		}
	}
	else
	{
		//TO ADD
		TRACE("cInnerCmd[0] = %s \n", cInnerCmd[0]);
		TRACE("recv unrecgnised \n");
	}
	return 0;
}

bool Client::ClientGetpara(char * _data, int _iBuflength)
{
	int i;
	char cMsg[2048] = { 0 };
	char* cInnerCmd[48] = { NULL };
	char  cTmp[48 * 48] = { 0 };
	if (cInnerCmd[0] == NULL)
	{
		for (i = 0; i < 48; i++)
		{
			cInnerCmd[i] = &cTmp[i * 48];
		}
	}
	//	int iRet;
	for (i = 0; i < 32; i++)
	{
		memset(cInnerCmd[i], 0, 32);
	}
	if (SplitString((unsigned char*)_data, (unsigned char*)"\t", (unsigned char**)cInnerCmd, 32, 32) < 0)
		return false;
	if (strcmp(cInnerCmd[0], "PRODUCTMODEL") == 0)
	{
		strcpy_s(m_cProductModel, cInnerCmd[1]);
	}
	else if (strcmp(cInnerCmd[0], "FACTORYID") == 0)
	{
		strcpy_s(m_cFactoryID, cInnerCmd[1]);
	}
	else if (strcmp(cInnerCmd[0], "DEVICEABSTRACT") == 0)
	{
		m_iChnnum = atoi(cInnerCmd[1]);
		::PostMessage(m_cHwnd, WM_MY_REFRESHCHNCOM, 0, 0);
	}
	else if (strcmp(cInnerCmd[0], "LANPARAM") == 0)
	{
		TRACE("LANPARAM TODO ADD\n");
	}
	else if (strcmp(cInnerCmd[0], "CHNTYPE") == 0)
	{
		for (i = 0; i < m_iChnnum; i++)
		{
			m_tVChn[i].m_iChnType = atoi(cInnerCmd[1 + i]);
		}

	}

	else if (strcmp(cInnerCmd[0], "IFRAMERATE") == 0)
	{
		for (i = 0; i < m_iChnnum; i++)
		{
			m_tVChn[i].m_tVideopara.m_iIFrameRate = atoi(cInnerCmd[1 + i]);
		}
	}
	else if (strcmp(cInnerCmd[0], "FRAMERATE") == 0)
	{
		for (i = 0; i < m_iChnnum; i++)
		{
			m_tVChn[i].m_tVideopara.m_iFrameRate = atoi(cInnerCmd[1 + i]);
		}
	}
	else if (strcmp(cInnerCmd[0], "BITRATE") == 0)
	{
		for (i = 0; i < m_iChnnum; i++)
		{
			m_tVChn[i].m_tVideopara.m_iBitRate = 8 * atoi(cInnerCmd[1 + i]);
		}
	}
	else if (strcmp(cInnerCmd[0], "ENCODETYPE") == 0)
	{
		for (i = 0; i < m_iChnnum; i++)
		{
			m_tVChn[i].m_tVideopara.m_iEncodeType = atoi(cInnerCmd[1 + i]);

		}
	}
	else if (strcmp(cInnerCmd[0], "VIDEOSIZE") == 0)
	{
		for (i = 0; i < m_iChnnum; i++)
		{
			m_tVChn[i].m_tVideopara.m_iVideoSize = atoi(cInnerCmd[1 + i]);

		}
	}
	else if (strcmp(cInnerCmd[0], "VIDEOQUALITY") == 0)
	{
		for (i = 0; i < m_iChnnum; i++)
		{
			m_tVChn[i].m_tVideopara.m_iVideoQuality = atoi(cInnerCmd[1 + i]);
			//m_cChn[i].m_iVideoSize =atoi(cInnerCmd[3 + i]);
		}

	}
	else if (strcmp(cInnerCmd[0], "VIDEOPARA") == 0)
	{
		for (i = 0; i < m_iChnnum; i++)
		{
			m_tVChn[i].m_tVideopara.m_ViCnf.m_iLum = atoi(cInnerCmd[i * 4 + 1]);
			m_tVChn[i].m_tVideopara.m_ViCnf.m_iSaturation = atoi(cInnerCmd[i * 4 + 2]);
			m_tVChn[i].m_tVideopara.m_ViCnf.m_iContrast = atoi(cInnerCmd[i * 4 + 3]);
			m_tVChn[i].m_tVideopara.m_ViCnf.m_iHue = atoi(cInnerCmd[i * 4 + 4]);
		}
	}
	else if (strcmp(cInnerCmd[0], "AUDIOSAMPLE") == 0)
	{
		for (i = 0; i < m_iChnnum; i++)
		{
			m_tVChn[i].m_tVideopara.m_iAudioSample = atoi(cInnerCmd[i + 2]);
		}
	}
	else if (strcmp(cInnerCmd[0], "OSD_COLOR") == 0)
	{
		int iOsdType = atoi(cInnerCmd[1]);
		for (i = 0; i < m_iChnnum; i++)
		{
			m_tVChn[i].m_TOverLay[iOsdType].m_iColor = atoi(cInnerCmd[i + 2]);
		}
	}
	else if (strcmp(cInnerCmd[0], "OSD_DISPLAY") == 0)
	{
		int iOsdType = atoi(cInnerCmd[1]);
		for (i = 0; i < m_iChnnum; i++)
		{
			m_tVChn[i].m_TOverLay[iOsdType].m_iDispaly = atoi(cInnerCmd[i + 2]);
		}
	}
	else if (strcmp(cInnerCmd[0], "OSD_POS") == 0)
	{
		int iOsdType = atoi(cInnerCmd[1]);
		int iX, iY;
		for (i = 0; i < m_iChnnum; i++)
		{
			sscanf(cInnerCmd[i + 2], "%d:%d", &iX, &iY);
			m_tVChn[i].m_TOverLay[iOsdType].m_iX = iX;
			m_tVChn[i].m_TOverLay[iOsdType].m_iY = iY;
		}
	}
	else if (strcmp(cInnerCmd[0], "OSDTEXT") == 0)
	{
		int iChn = atoi(cInnerCmd[1]);
		strcpy(m_tVChn[iChn].m_TOverLay[0].m_strWord, cInnerCmd[2]);
	}
	else if (strcmp(cInnerCmd[0], "OSDOVERLAY") == 0)
	{
		int iChn = atoi(cInnerCmd[1]);
		strcpy(m_tVChn[iChn].m_TOverLay[2].m_strWord, cInnerCmd[2]);
	}
	else if (strcmp(cInnerCmd[0], "VOSIZELIST") == 0)
	{
		TRACE("VOSIZELIST TO ADD\n");
	}
	else if (strcmp(cInnerCmd[0], "RTMP") == 0)
	{
		if (strcmp(cInnerCmd[1], "ENABLE") == 0)
		{
			m_blRtmpEnable = atoi(cInnerCmd[2]);
		}
		else if (strcmp(cInnerCmd[1], "SERVER") == 0)
		{
			strcpy(m_strRtmpserver, cInnerCmd[2]);
		}
	}
	else if (strcmp(cInnerCmd[0], "VODEVSIZE") == 0)
	{
		int iVodev = atoi(cInnerCmd[1]);
		m_iVodevSize[iVodev] = atoi(cInnerCmd[2]);
	}
	else if (strcmp(cInnerCmd[0], "NTP") == 0)
	{
		TRACE("NTP to ADD\n");
	}
	else if (strcmp(cInnerCmd[0], "FTPUPDATE") == 0)
	{
		TRACE("FTPUPDATE TODO ADD\n");
	}
	else if (strcmp(cInnerCmd[0], "PREVIEWPARA") == 0)
	{
		int iIndex = 0;
		int iVodev = atoi(cInnerCmd[1]);
		if (iVodev < 2)
		{
			iIndex = iVodev;
		}
		else
		{
			iIndex = 2;
		}
		m_tVodev[iIndex].iVodev = iVodev;
		m_tVodev[iIndex].iPicCnt = atoi(cInnerCmd[2]);
		int iChnlist[4] = { 0 };
		sscanf(cInnerCmd[3], "%d:%d:%d:%d", iChnlist, iChnlist + 1, iChnlist + 2, iChnlist + 3);
		for (i = 0; i < m_tVodev[iIndex].iPicCnt; i++)
		{
			m_tVodev[iIndex].iChnlist[i] = iChnlist[i];
		}
	
		TRACE("PREVIEWREC TODO ADD\n");
	}
	else if (strcmp(cInnerCmd[0], "LOCALSTORE") == 0)
	{
		TRACE("LOCALSTORE TODO ADD\n");
	}
	else if (strcmp(cInnerCmd[0], "LOCALSTORE") == 0)
	{
		TRACE("LOCALSTORE TODO ADD\n");
	}
	else if (strcmp(cInnerCmd[0], "DISK") == 0)
	{
		TRACE("DISK TODO ADD\n");
	}
	else if (strcmp(cInnerCmd[0], "VIDEOSIZE_LIST") == 0)
	{
		TRACE("VIDEOSIZE_LIST TODO ADD\n");
	}
	else if (strcmp(cInnerCmd[0], "VIDEOSIZE_LIST") == 0)
	{
		TRACE("VIDEOSIZE_LIST TODO ADD\n");
	}
	else if (strcmp(cInnerCmd[0], "HEART") == 0)
	{
		m_iHeartIntval = atoi(cInnerCmd[1]);
		TRACE("m_iHeartIntval = %d\n", m_iHeartIntval);

	}
	else if (strcmp(cInnerCmd[0], "LOGFILE") == 0)
	{
		m_iLoglevel = atoi(cInnerCmd[1]);
		m_iLogSize = atoi(cInnerCmd[2]);
		TRACE("m_iLoglevel = %d\n", m_iLoglevel);
	}
	else if (strcmp(cInnerCmd[0], "DHCP") == 0)
	{
		TRACE("DHCP to add\n");
	}
	else
	{
		TRACE("cInnerCmd[0] = %s \n", cInnerCmd[0]);
		TRACE("PARAGET unrecgnised \n");
	}
	return true;
}

bool Client::ClientGetsetPararsp(char * _data, int _iBuflength)
{
	ClientGetpara(_data, _iBuflength);
	return true;
}

bool Client::ClientGetUserpara(char * _data, int _iBuflength)
{
	int i;
	char cMsg[2048] = { 0 };
	char* cInnerCmd[48] = { NULL };
	char  cTmp[48 * 48] = { 0 };
	if (cInnerCmd[0] == NULL)
	{
		for (i = 0; i < 48; i++)
		{
			cInnerCmd[i] = &cTmp[i * 48];
		}
	}
	//	int iRet;
	for (i = 0; i < 32; i++)
	{
		memset(cInnerCmd[i], 0, 32);
	}
	if (SplitString((unsigned char*)_data, (unsigned char*)"\t", (unsigned char**)cInnerCmd, 32, 32) < 0)
		return false;
	if (strcmp(cInnerCmd[0], "GETUSERNUM") == 0)
	{
		m_iUserNum = atoi(cInnerCmd[1]);

	}
	else if (strcmp(cInnerCmd[0], "GETUSERINFO") == 0)
	{

	}
	else if (strcmp(cInnerCmd[0], "MAXCONUSER") == 0)
	{
		m_iMaxUserNum = atoi(cInnerCmd[1]);
	}
	else
	{
		TRACE("cInnerCmd[0] = %s \n", cInnerCmd[0]);
		TRACE("PARAGET unrecgnised \n");
	}
	return false;
}

bool Client::ClientLoginRsp(char * _data, int _iBuflengt)
{
	int i;
	char cMsg[2048] = { 0 };
	char* cInnerCmd[48] = { NULL };
	char  cTmp[48 * 48] = { 0 };
	if (cInnerCmd[0] == NULL)
	{
		for (i = 0; i < 48; i++)
		{
			cInnerCmd[i] = &cTmp[i * 48];
		}
	}
	//	int iRet;
	for (i = 0; i < 32; i++)
	{
		memset(cInnerCmd[i], 0, 32);
	}
	if (SplitString((unsigned char*)_data, (unsigned char*)"\t", (unsigned char**)cInnerCmd, 32, 32) < 0)
		return false;
	if (strcmp(cInnerCmd[0], "AUTHERFAILED") == 0)
	{

		unsigned int iKeyLen = 0;
		base64_decode(m_cLoginKey, &iKeyLen, (unsigned char *)cInnerCmd[3], strlen(cInnerCmd[3]));
		TRACE("%x, %x, %x, %x, %x, %x, %x, %x", m_cLoginKey[0], m_cLoginKey[1], m_cLoginKey[2], m_cLoginKey[3], m_cLoginKey[4], m_cLoginKey[5], m_cLoginKey[6], m_cLoginKey[7]);
		char cMd5ToBase64[128];
		int iEncryptLen = 127;
		cMsg[0] = '\0';
		char cTmp[16];
		sprintf_s(cMsg, "%s%s%s%s%s%s", m_cStrServerIp, "\tIP\t", "USER", TAB, "LOGON", TAB);
		EncryptStrToMd5ToBase64(m_cUserName, strlen(m_cUserName), (const char *)m_cLoginKey, 8, cMd5ToBase64, iEncryptLen);

		strcat_s(cMsg, cMd5ToBase64);
		strcat_s(cMsg, "\t");
		EncryptStrToMd5ToBase64(m_cPassWord, strlen(m_cPassWord), (const char *)m_cLoginKey, 8, cMd5ToBase64, iEncryptLen);

		strcat_s(cMsg, cMd5ToBase64);
		strcat_s(cMsg, "\t");
		_itoa(0, cTmp, 10);
		strcat_s(cMsg, cTmp);
		strcat_s(cMsg, "\n\n\n");

		SocketSendbuf(m_icmdSocket, cMsg, strlen(cMsg), 0);
		return 0;
	}
	else if (strcmp(cInnerCmd[0], "SUCCEED") == 0)
	{
		m_iLoginstat = 0x2;
		m_blUpdate = true;
	}
	else if (strcmp(cInnerCmd[0], "FAILED") == 0)
	{
		m_iLoginstat = 0;
		m_iFailReason = atoi(cInnerCmd[3]);
		m_blUpdate = true;
	}
	else if (strcmp(cInnerCmd[0], "FINISHED") == 0)
	{
		m_iLoginstat = 8;
		m_blUpdate = true;
		::PostMessage(m_cHwnd, WM_MY_REFRESHCHNPARA, 0, 0);
	}
	else
	{
		TRACE("cInnerCmd[0] = %s \n", cInnerCmd[0]);
		TRACE("LOGON unrecgnised \n");
	}
	return true;
}



int Client::ClientSetPreviewpara(int _iVodev, int iCnt, int *iChn)
{
	int i = 0;
	char cTmp[64];
	char cTemp[16];
	char cMsg[2048];
	sprintf(cMsg, "%s%s%s%s%s%s%s%s%d%s", m_cStrServerIp, TAB, "IP", TAB, "PARASET", TAB, "PREVIEWPARA", TAB,0,TAB);
	_itoa(iCnt, cTmp, 10);
	strcat(cMsg, cTmp);
	strcat(cMsg, "\t");
	cTmp[0] = '\0';
	sprintf(cTmp, "%d", iChn[0]);
	for (i = 1; i < iCnt; i++)
	{
		strcat(cTmp, ":");
		_itoa(iChn[i], cTemp, 10);
		strcat(cTmp, cTemp);
	}
	strcat(cMsg, cTmp);
	strcat(cMsg, END);
	
	SocketSendbuf(m_icmdSocket, cMsg, strlen(cMsg), 0);
	return 0;
}

int Client::ClientConnectChn(int _iChn, int _iSub, int _iNetMode, HWND _hWnd)
{
	int iSocket = CreateSocket(m_cStrServerIp, m_iServerPort + 1);
	if (iSocket < 0)
	{
		TRACE("con failed\n");
	}
	int iChn = _iChn + _iSub * m_iChnnum;
	m_tVChn[iChn].m_iDataSocket = iSocket;
	m_tVChn[iChn].ChnSendConnect(_iSub, _iNetMode, m_iCmdId, _hWnd);
	return 0;
}

int Client::ClientLogoffFromServer()
{
	int i;
	for (i = 0; i < MAX_CHN; i++)
	{
		m_tVChn[i].ChnDisConFromServer();		
	}
	
	if (m_icmdSocket > 0)
	{
		int i = 0;
	
		char cMsg[2048];
		sprintf(cMsg, "%s%s%s%s%s%s%s%s%s%s", m_cStrServerIp, TAB, "IP", TAB, "USER", TAB, "LOGOFF", TAB, m_cUserName, TAB);

		strcat(cMsg, END);
		SocketSendbuf(m_icmdSocket, cMsg, strlen(cMsg), 0);
		closesocket(m_icmdSocket);
		m_icmdSocket = 0;
	}
	return 1;
}


int SendSocketMsg(char *_cServerIp,SOCKET _iSocket, char *_cMsg, int _iLen)
{
	char cMsg[2048];
	sprintf(cMsg, "%s%s%s%s%s", _cServerIp, TAB, "IP", TAB, _cMsg);
	SocketSendbuf(_iSocket, cMsg, strlen(cMsg), 0);
	return 0;
}



int Client::ClientSetVodevSize(int _iVodev, int _iVodevSize)
{
	if (m_icmdSocket > 0)
	{
		int i = 0;

		char cMsg[2048];
		sprintf(cMsg, "%s%s%s%s%s%s%s%s%d%s%d%s", m_cStrServerIp, TAB, "IP", TAB, "PARASET", TAB, "VODEVSIZE", TAB, _iVodev, TAB, _iVodevSize, END);

	
		SocketSendbuf(m_icmdSocket, cMsg, strlen(cMsg), 0);
	}
	return true;
}