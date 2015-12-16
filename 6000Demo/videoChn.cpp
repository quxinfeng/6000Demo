#include "stdafx.h"
#include "videoChn.h"
#include "socket.h"
#include "com_fun.h"
static char videodataframe[1024 * 1024];
UINT ThreadDecode(LPVOID pParam);
videoChn::videoChn()
{
	m_iConnectStat = 0;
	m_iDataSize = 0;			// 包实际数据长度
	m_iType = 0;				// 数据类型
	m_bNewFrame = 1; 			//是否为新的一帧数据
	m_iDataRecvLen = 0;                          //本帧收到的数据长度
	m_bStartLost = 0;                       //是否已开始丢帧
	m_iFrameType = 0; //当前帧类型
	m_ulFrameNo = 0;			//当前帧序号
	m_iRcvFramNum = 0;
	m_iFramLostNum = 0;
	//memset(m_cData, 0, sizeof(m_cData));
	m_RcvVideoData = NULL;

	m_iDataSocket = 0;
	m_iDataLiveTime = 0;
	m_iRecvLen = 0;
	m_cVideoData = NULL;
	m_iVideoDatalen = 0;


	InitializeCriticalSection(m_iLock);

	m_iVideoRecFramNo = 0;
	m_iVideoDecFramNo = 0;
	memset(&m_TOverLay, 0, 3 * sizeof(TChnOverlay));
	memset(&m_tVideopara, 0, sizeof(VideoParam));
}


videoChn::~videoChn()
{
}
int videoChn::cSendMsg( SOCKET _iSocket, char *_cMsg, int _iLen)
{
	char cMsg[2048];
	sprintf(cMsg, "%s%s%s%s%s", m_cServerIp, TAB, "IP", TAB, _cMsg);
	SocketSendbuf(_iSocket, cMsg, strlen(cMsg), 0);
	return 0;
}

int videoChn::ChnSendConnect(int _iSub, int _iNetMode, int m_iCmdId, HWND hMainwnd)
{
	int i = 0;
	int iChn = m_iChnNo;
	char cTmp[64];
	m_iConnectStat = 1;
	m_cData = (char *)malloc(sizeof(char) * 128 * 1024);
	yuv = (unsigned char *)malloc(sizeof(char) *6* 1024 * 1024);
	m_cVideoData = (char *)malloc(sizeof(char) * 20 * 1024 * 1024);
	//m_tDataCenter.initDatacenter();
	char cMsg[2048];
	CWinThread*pThread = AfxBeginThread(ThreadReceiveVideoData, this, 0, 0, 0);
	Sleep(100);
	if (pThread == NULL)
	{
		DWORD dw = GetLastError();
		TRACE("ThreadReceiveVideoData failed dw =%d \n", dw);
	}
	//m_tDecoder.initHIH264();
	m_tDraw.m_hDisplayWnd = hMainwnd;            
	m_tDraw.ReleaseDirectDraw();
	m_tDraw.InitDraw(1920, 1088, 24);                        
	m_tDraw.CreateDirectDraw(hMainwnd, 0);
	m_tDecoder.DecoderCreate(1920, 1090);

	pThread = AfxBeginThread(ThreadDecode, this, 0, 0, 0);
	Sleep(100);
	if (pThread == NULL)
	{
		DWORD dw = GetLastError();
		TRACE("ThreadDecode failed dw =%d \n", dw);
	}
	TRACE("iChn = %d \n", iChn);
	TRACE("_iNetMode = %d \n", _iNetMode);
	sprintf(cMsg, "%s%s%s%s",  "INNER", TAB, "CONNECT", TAB);
	_itoa(iChn, cTmp, 10);
	strcat(cMsg, cTmp);
	strcat(cMsg, "\t");
	_itoa(_iNetMode + 1, cTmp, 10);
	strcat(cMsg, cTmp);
	strcat(cMsg, "\t");
	_itoa(m_iCmdId, cTmp, 10);
	strcat(cMsg, cTmp);
	strcat(cMsg, END);
	cSendMsg(m_iDataSocket, cMsg, strlen(cMsg));
	
	
	ChnForceIframe();
	//char *FileName = "1.h264";

	return 0;
}
void videoChn::ChnForceIframe()
{
	if (m_iDataSocket <= 0)
	{
		return;
	}
	int i = 0;

	char cMsg[2048];
	sprintf(cMsg, "%s%s%s%s%d",  "CMD", TAB, "FORCE_IFRAME", TAB, m_iChnNo);
	strcat(cMsg, END);
	cSendMsg(m_iDataSocket, cMsg, strlen(cMsg));
}
void videoChn::SetChnVideosize(int _iVideosize)
{
	if (*m_pCmdSocket <= 0)
	{
		return;
	}
	
	int iWidth = 1920;
	int iHeight = 1080;
	switch (_iVideosize)
	{
	case CIF:
		iWidth = 352;
		iHeight = 288;
		break;
	case D1:
		iWidth = 704;
		iHeight = 576;
		break;
	case HD720:
		iWidth = 1280;
		iHeight = 720;
		break;
	case HD960:
		iWidth = 1280;
		iHeight = 960;
		break;
	case HD1080:
		iWidth = 1920;
		iHeight = 1080;
		break;
	default:
		break;
	}
	char cMsg[2048];
	sprintf(cMsg, "%s%s%s%s%d%s%d%s", "PARASET", TAB, "VIDEOSIZE", TAB, m_iChnNo,TAB, _iVideosize,END);

	cSendMsg(*m_pCmdSocket, cMsg, strlen(cMsg));
}
void videoChn::SetChnFramerate(int _iFramerate)
{
	if (*m_pCmdSocket <= 0)
	{
		return;
	}

	
	char cMsg[2048];
	sprintf(cMsg, "%s%s%s%s%d%s%d%s", "PARASET", TAB, "FRAMERATE", TAB, m_iChnNo, TAB, _iFramerate,  END);

	cSendMsg(*m_pCmdSocket, cMsg, strlen(cMsg));
}
void videoChn::SetChnBitrate(int _iBitrate)
{
	if (*m_pCmdSocket <= 0)
	{
		return;
	}


	char cMsg[2048];
	sprintf(cMsg, "%s%s%s%s%d%s%d%s", "PARASET", TAB, "BITRATE", TAB, m_iChnNo, TAB, _iBitrate, END);

	cSendMsg(*m_pCmdSocket, cMsg, strlen(cMsg));
}

void videoChn::SetChnIframerate(INT _iIFramerate)
{
	if (*m_pCmdSocket <= 0)
	{
		return;
	}


	char cMsg[2048];
	sprintf(cMsg, "%s%s%s%s%d%s%d%s", "PARASET", TAB, "IFRAMERATE", TAB, m_iChnNo, TAB, _iIFramerate, END);

	cSendMsg(*m_pCmdSocket, cMsg, strlen(cMsg));
}
void videoChn::SetChnVideoquality(int _iVideoQuality)
{
	if (*m_pCmdSocket <= 0)
	{
		return;
	}


	char cMsg[2048];
	sprintf(cMsg, "%s%s%s%s%d%s%d%s", "PARASET", TAB, "VIDEOQUALITY", TAB, m_iChnNo, TAB, _iVideoQuality, END);

	cSendMsg(*m_pCmdSocket, cMsg, strlen(cMsg));
}
void videoChn::SetChnEncodetype(int _iEncodeType)
{
	if (*m_pCmdSocket <= 0)
	{
		return;
	}


	char cMsg[2048];
	sprintf(cMsg, "%s%s%s%s%d%s%d%s", "PARASET", TAB, "ENCODETYPE", TAB, m_iChnNo, TAB, _iEncodeType, END);

	cSendMsg(*m_pCmdSocket, cMsg, strlen(cMsg));
}
void videoChn::SetChnVideopara(int _iBrightness, int _iConstant, int _isaturation,int _iHue)
{
	if (*m_pCmdSocket <= 0)
	{
		return;
	}


	char cMsg[2048];
	sprintf(cMsg, "%s%s%s%s%d%s%d%s%d%s%d%s%d%s", "PARASET", TAB, "VIDEOPARA", TAB, m_iChnNo, TAB, _iBrightness,TAB, _iConstant,TAB, _isaturation,TAB, _iHue,END);

	cSendMsg(*m_pCmdSocket, cMsg, strlen(cMsg));
}
int videoChn::ChnDisConFromServer()
{
	if (m_iDataSocket <= 0)
	{
		return 0;
	}
	int i = 0;

	char cMsg[2048];
	sprintf(cMsg, "%s%s%s%s%d%s%d%s", "INNER", TAB, "DISCONNECT", TAB, 0, TAB, 1, TAB);
	m_iConnectStat = 0;
	strcat(cMsg, END);
	cSendMsg(m_iDataSocket, cMsg, strlen(cMsg));
	closesocket(m_iDataSocket);
	m_iDataSocket = 0;
	if (m_cData)
	{
		free(m_cData);
		m_cData = NULL;

	}
	if (yuv)
	{
		free(yuv);
		yuv = NULL;
	}
	if (m_cVideoData)
	{
		free(m_cVideoData);
		m_cVideoData = NULL;
	}
	return 1;
}
static int IsValidFrame(unsigned char* _packBuf, int _packLen,  int *_frameNo)
{
	return 1;
	
}

static int GetFrameType(unsigned char* _ucData, int _iLength, int *_iFramSize)
{
	int i, iRet;
	unsigned int dwTmp;
	iRet = -1;
	*_iFramSize = 0;
	if (!_ucData) return iRet;

	if (_iLength < sizeof(S_FrameHeader)) return iRet;
	//TRACE("sizeof(S_FrameHeader) = %d \n", sizeof(S_FrameHeader));
	for (i = 0; i<(_iLength - (int)sizeof(S_FrameHeader)); i++)
	{
		memcpy(&dwTmp, _ucData + i, 4);
//		TRACE("dwTmp = %x \n", dwTmp);
		if (dwTmp == 0x02000000)
		{
			pS_FrameHeader FrameHeader = (pS_FrameHeader)_ucData;
			iRet = FrameHeader->u8FrameType;
			*_iFramSize = FrameHeader->u32FrameSize;


	//		TRACE("_iFramSize = %d \n", *_iFramSize);
	//		TRACE("uvstreamlen = %d \n", FrameHeader->u32StreamLen);
	//		TRACE("u16AFrameSize = %d \n", FrameHeader->u16AFrameSize);

	//		TRACE("iRet = %d \n", iRet);
			return iRet;
		}
	}
	return iRet;
}



int  videoChn::AVData_WriteBuffer(char *_ucData, int _iLen)
{


	//	td_printf(0, "%s  ichan :%d\n",  __FUNCTION__,pThis->m_iChan);

	int iFrameSize = 0, iSize = 0;
	int iRecvLen = _iLen;
	int bFull = 0;
	int test = 0;

	if (iRecvLen <= 0)
	{
		return false;
	}

	//TRACE("1234567 %s :%d m_bNewFrame =%d\n", __FUNCTION__, __LINE__, m_bNewFrame);
	if (m_bNewFrame)
	{
		m_iDataRecvLen = 0;
		m_iFrameType = GetFrameType((unsigned char*)_ucData, iRecvLen, &iSize);
	//	TRACE("1234567 %s :%d m_iFrameType= %d m_bStartLost = %d\n", __FUNCTION__, __LINE__, m_iFrameType, m_bStartLost);
		if (TD_IFRAME == m_iFrameType)
		{
			m_iRcvFramNum++;
			m_bStartLost = 0;
		}
		else if (m_iFrameType == TD_PFRAME)
		{
			m_iRcvFramNum++;
			if (m_bStartLost == 1)
			{
				m_iFramLostNum++;
				return true;
			}
		}
		else
		{
			return true;
		}
		m_bNewFrame = 0;
		if (m_RcvVideoData != NULL)
		{
			
			m_RcvVideoData = NULL;
		}
	//	TRACE("1234567 %s :%diSize =%d  \n", __FUNCTION__, __LINE__, iSize);

		m_RcvVideoData = videodataframe;
		m_iType = iSize;
		m_iDataSize = iSize;
	//	TRACE("1234567 %s :%d \n", __FUNCTION__, __LINE__);
		if (m_RcvVideoData == NULL)
		{
			m_bStartLost = 1;
			m_iFramLostNum++;
	//		TRACE("AVData_WriteBuffer   DataPackageCreate   fail  iSize:%d \n", iSize);
			return true;
		}
	}


	if (m_bStartLost)
	{
		if (m_RcvVideoData != NULL)
		{
		

			m_RcvVideoData = NULL;
		}
		m_bNewFrame = 1;
		return true;
	}

	if (iRecvLen > 0)
	{


		//TRACE("1234567 %s :%d m_iDataRecvLen = %d iRecvLen =%d\n", __FUNCTION__, __LINE__, m_iDataRecvLen, iRecvLen);
		if ((m_iDataRecvLen + iRecvLen) > m_iType)

		{
			m_bNewFrame = 1;
			m_bStartLost = 1;
			m_iFramLostNum++;
			
			m_RcvVideoData = NULL;
			return true;
		}

		//TRACE("1234567 %s :%d m_iType =%d\n", __FUNCTION__, __LINE__, m_iType);
		memcpy(m_RcvVideoData + m_iDataRecvLen, _ucData, iRecvLen);

		m_iDataRecvLen += iRecvLen;
	}
	//TRACE("1234567 %s :%d iFrameSize =%d\n", __FUNCTION__, __LINE__, iFrameSize);
	//如果是一帧的结束
	if (m_iDataSize == m_iDataRecvLen)
	{

		m_bNewFrame = 1;
		{
			//接收完整一帧数据后才写入缓冲区


			test = IsValidFrame((unsigned char *)m_RcvVideoData, m_iDataSize, &m_ulFrameNo);
		//	TRACE("AVData_WriteBuffer  m_iDataSize=%d test=%d \n", m_iDataSize, test);

			if (test)
			{
				bFull = 1;
			}
			else
			{
				m_bStartLost = 1;
				m_iFramLostNum++;
				
				m_RcvVideoData = NULL;
			//	TRACE(" ###########AVData_WriteBuffer  IsValidFrame  error \n");
				//nvs_ForceIFrame(pThis);
				return true;
			}
		}
	}



	if (bFull)
	{
		//TRACE("1234567 %s :%d bFull =%d \n", __FUNCTION__, __LINE__, bFull);
	
		
		//TRACE("1234567 %s :%d bFull =%d \n", __FUNCTION__, __LINE__, bFull);
		
		m_iLock.Lock();
		m_iVideoRecFramNo++;
	
		TRACE("m_iVideoRecFramNo = %d tm = %umsd\n", m_iVideoRecFramNo, GetTickCount());
		if (m_iVideoDatalen + m_iDataSize > (20 * 1024 * 1024))
		{
			m_iLock.Unlock();
			return true;
		}
		memcpy(m_cVideoData + m_iVideoDatalen, m_RcvVideoData, m_iDataSize);
		m_iVideoDatalen += m_iDataSize;
		TRACE("m_iVideoDatalen = %d \n", m_iVideoDatalen);
		m_iLock.Unlock();
		m_RcvVideoData = NULL;
		m_iDataSize = 0;
	}
	return TRUE;
}
 char g_cSplit[] = "\t";
 char g_cEnd[] = "\n\n\n";

 int videoChn::Recv_Command(char* _cRecv, int _iCount)
{

	int i;
	int iRet;
	int iChan;
	char*cCmd[48];
	char cCommand[2056];
	char  cTmp[48 * 48] = { 0 };

	if (strlen(_cRecv) >= 2056)
	{
		TRACE("Command Error!\n");
		return true;
	}
	for (i = 0; i<48; i++)
	{
		cCmd[i] = &cTmp[i * 48];
	}

	TRACE("Recv_Command %s\n",_cRecv);
	while (strlen(_cRecv) > 10)
	{
		memset(cCommand, 0, sizeof(cCommand));
		iRet = OnCharLeft(_cRecv, "\n\n\n", cCommand);     //取第一条命令
		if (iRet != 0)
		{
			return true;
		}
		OnCharRight(_cRecv, g_cEnd, _cRecv);     			//剩下的命令
		iRet = OnCharRight(cCommand, "INNER", cCommand);     //剩下的命令
		if (iRet != 0)
		{
			return true;
		}

		if (strlen(cCommand) >= 3)
		{
			strcat(cCommand, "\0");
			
			//td_printf(0,"cCommand = %s", cCommand);
			if (SplitString((unsigned char *)cCommand, (unsigned char *)g_cSplit, (unsigned char **)cCmd, 32, 32) < 0)
			{
				TRACE( "$$$$$$$$$$$$$$$$$$$$$$$$ 1\n");
				return true;
			}

			if (cCmd[1] == NULL)
			{
				TRACE("$$$$$$$$$$$$$$$$$$$$$$$$ 2l\n");
				return true;
			}

			if (0 == strcmp(cCmd[1], "HEADER"))
			{
				TRACE("$$$$$$$$$$$$$$$$$$$$$$$$ 2l\n");
				TRACE("%s  HEADER  \n", __FUNCTION__);
				for (i = 2; i<7; i++)
				{
					if (cCmd[i] == NULL)
					{
						TRACE("$$$$$$$$$$$$$$$$$$$$$$$$ 2l\n");
						(0, "$$$$$$$$$$$$$$$$$$$$$$$$ 3l\n");
						return true;
					}
				}

				iChan = atoi(cCmd[2]);
				/*
				if (((iChan >= 0 && iChan < pNvs->m_iChannelNum * 2) && (iChan % pNvs->m_iChannelNum < MAX_CHANNEL_NUM)) ||
					((iChan >= pNvs->m_iChannelNum * STREAM_AUDIO_START_CHN && iChan < pNvs->m_iChannelNum * (STREAM_AUDIO_START_CHN + 1))))
				{
					if (iChan >= 0 && iChan < pNvs->m_iChannelNum * 2)
					{
						pVideoChan = &pNvs->m_VideoChannel[MAINORSUB(iChan, pNvs->m_iChannelNum)][iChan%pNvs->m_iChannelNum];
					}
					else if (iChan >= pNvs->m_iChannelNum * STREAM_AUDIO_START_CHN && iChan < pNvs->m_iChannelNum * (STREAM_AUDIO_START_CHN + 1))
					{
						pVideoChan = &pNvs->m_VideoChannel[AUDSTREAM][iChan%pNvs->m_iChannelNum];
					}

					pVideoChan->m_VideoParam.m_VideoDim.m_iHeight = Nvsatoi(cCmd[5]);
					pVideoChan->m_VideoParam.m_VideoDim.m_iWidth = Nvsatoi(cCmd[4]);
					pVideoChan->m_VideoParam.m_VideoDim.m_iType = Nvsatoi(cCmd[6]);
					pVideoChan->m_VideoParam.m_VideoDim.m_iFrame = Nvsatoi(cCmd[3]);
					pVideoChan->m_VideoParam.m_iFrameRate = Nvsatoi(cCmd[3]);
					pVideoChan->m_VideoParam.m_iHeight = pVideoChan->m_VideoParam.m_VideoDim.m_iHeight;
					pVideoChan->m_VideoParam.m_iWidth = pVideoChan->m_VideoParam.m_VideoDim.m_iWidth;
					TRACE("$$$$$$$$$$$$$$$$$$$$$$$$ 2l\n");
					(0, "%s  HEADER   m_iWidth:%d   m_iHeight:%d\n", __FUNCTION__, pVideoChan->m_VideoParam.m_iWidth, pVideoChan->m_VideoParam.m_iHeight);
					TRACE("$$$$$$$$$$$$$$$$$$$$$$$$ 2l\n");
					(0, "%s  HEADER   m_IpChan:%p   m_iIpChanNum:%d\n", __FUNCTION__, pVideoChan->m_IpChan, pVideoChan->m_iIpChanNum);
					gTdNvsModule.MsgNotifyCallback(pVideoChan->m_IpChan, pVideoChan->m_iIpChanNum, EVENT_CONNECT, MSG_VIDEO_HEADER, (void *)&pVideoChan->m_VideoParam.m_VideoDim);		//inchannel_enevt
																																														//	SendConnectMsg(pVideoChan,MSG_CONNECT_SUCCESS);	
					if (PARAM_NOSEND == pVideoChan->m_iSendVCParamStat)
					{
						pVideoChan->m_iSendVCParamStat = PARAM_RCVHEAD;
					}
				}*/
			}
			
			else if ((0 == strcmp(cCmd[1], "PARAGET")) || (0 == strcmp(cCmd[1], "PARASET")))
			{
				/*
				if ((0 == strcmp(cCmd[1], "PARASET")) && (pNvs->m_iLogonState != LOGON_SUCCESS))
				{
					TRACE("$$$$$$$$$$$$$$$$$$$$$$$$ 2l\n");
					(0, "%s, pNvs->m_iLogonState:%d", __func__, pNvs->m_iLogonState);
					return true;
				}

				//调用字符串分析函数
				iRet = Inner_AnalyseChnMsg(pNvs, cCmd);
				if (iRet<0)
				{
					td_printf(0, "$$$$$$$$$$$$$$$$$$$$$$$$ 5l\n");
					return iRet;
				}
				*/
			}
			else if ((0 == strcmp(cCmd[1], "USER")) || (0 == strcmp(cCmd[1], "SUPER")))
			{
				/*
				//调用字符串分析函数
				iRet = Inner_AnalyseUserMsg(pNvs, cCmd);
				if (iRet<0)
				{
					td_printf(0, "$$$$$$$$$$$$$$$$$$$$$$$$ 6l\n");
					return iRet;
				}
				*/
			}
			else if (0 == strcmp(cCmd[1], "LOGON"))
			{
				/*
				iRet = Inner_AnalyseChnMsg(pNvs, cCmd);
				if (iRet<0)
				{
					td_printf(0, "$$$$$$$$$$$$$$$$$$$$$$$$ 7l\n");
					return iRet;
				}
				*/
			}
			
			else if (0 == strcmp(cCmd[1], "CMDID"))
			{
				/*
				if (cCmd[2] == NULL)
				{
					td_printf(0, "$$$$$$$$$$$$$$$$$$$$$$$$ 9l\n");
					return true;
				}
				pNvs->m_iCmdID = atoi(cCmd[2]);
				*/
			}
			else if (0 == strcmp(cCmd[1], "CMD"))
			{
				/*
				if (cCmd[2] == NULL)
				{
					td_printf(0, "$$$$$$$$$$$$$$$$$$$$$$$$ 10l\n");
					return true;
				}
				if (0 == strcmp(cCmd[2], "TALK"))
				{
					iRet = Inner_AnalyseTalkMsg(pNvs, cCmd);
					if (iRet<0)
					{
						td_printf(0, "$$$$$$$$$$$$$$$$$$$$$$$$ 11l\n");
						return iRet;
					}
				}

				if (0 == strcmp(cCmd[2], "CONNECTFAILED"))
				{
					iChan = atoi(cCmd[3]);
					td_printf(0, " Recv_Command   CONNECTFAILED  iChan:%d\n", iChan);

					if (((iChan >= 0 && iChan < pNvs->m_iChannelNum * 2) && (iChan%pNvs->m_iChannelNum < MAX_CHANNEL_NUM)) ||
						(iChan >= pNvs->m_iChannelNum * STREAM_AUDIO_START_CHN && iChan < pNvs->m_iChannelNum * (STREAM_AUDIO_START_CHN + 1)))
					{
						if (iChan >= 0 && iChan < pNvs->m_iChannelNum * 2)
						{
							pVideoChan = &pNvs->m_VideoChannel[MAINORSUB(iChan, pNvs->m_iChannelNum)][iChan%pNvs->m_iChannelNum];
						}
						else if (iChan >= pNvs->m_iChannelNum * STREAM_AUDIO_START_CHN && iChan < pNvs->m_iChannelNum * (STREAM_AUDIO_START_CHN + 1))
						{
							pVideoChan = &pNvs->m_VideoChannel[AUDSTREAM][iChan%pNvs->m_iChannelNum];
						}

						if (pVideoChan->m_Connect != NULL)
						{
							pVideoChan->m_Connect->connection_close(pVideoChan->m_Connect);
						}
					}
				}

				if (0 == strcmp(cCmd[2], "PROUPGRADE"))
				{
					iRet = Inner_AnalyseUpgrateMsg(pNvs, cCmd, 1);
					if (iRet<0)
					{
						td_printf(0, "Inner_AnalyseUpgrateMsg PROUPGRADE\n");
						return iRet;
					}
				}

				if (0 == strcmp(cCmd[2], "WEBUPGRADE"))
				{
					iRet = Inner_AnalyseUpgrateMsg(pNvs, cCmd, 2);
					if (iRet<0)
					{
						td_printf(0, "PROUPGRADE WEBUPGRADE\n");
						return iRet;
					}
				}
				*/
			}
			else if (0 == strcmp(cCmd[1], "VERSION"))
			{
				/*
				if (cCmd[2] == NULL)
				{
					td_printf(0, "$$$$$$$$$$$$$$$$$$$$$$$$ 12l\n");
					return true;
				}
				td_printf(0, "%s, %d, cCmd = %s", __func__, __LINE__, cCmd[2]);
				strncpy(pNvs->m_cVersion, cCmd[2], 31);
				pNvs->m_cVersion[31] = 0;
				*/
			}
			
		}
	}

	return true;
}

 int  videoChn::AnalyzeCmd(char * _cBuf, int _iLen)
{

	int iRet;
	char cEncoder[64], cHead[64];
	char ucBuf[2048];
	
	

	//	td_printf(0, "%s  pClient->m_iType:%d\n", __FUNCTION__,pClient->m_iType);
	if ((_iLen<=0) || (_iLen >= 2048))
	{
		return true;
	}

	

	memset(ucBuf, 0, 2048);
	memcpy(ucBuf, _cBuf, _iLen);

	

	iRet = OnCharLeft(ucBuf, "\t", cEncoder);
	if (iRet != 0)
	{
		return true;
	}
	OnCharRight(ucBuf, "\t", ucBuf);
	iRet = OnCharLeft(ucBuf, "\t", cHead);
	if (iRet != 0)
	{
		return true;
	}
	OnCharRight(ucBuf, "\t", ucBuf);

	return Recv_Command(ucBuf, strlen(ucBuf));
}



void videoChn::AnalysisData(char *_ucData, int _iLen)
{
	unsigned int 	ui32Head;
	STREAMHEAD  dataHeader;
	COMMANDHEAD cmdHeader;
	int iLen = 0;

	//首先将数据拷贝到接收缓冲中
	if ((m_iRecvLen + _iLen)>(128 * 1024))
	{

		memcpy(m_cData, _ucData, _iLen);
		m_iRecvLen = _iLen;
	}
	else
	{

		memcpy(m_cData + m_iRecvLen, _ucData, _iLen);
		m_iRecvLen += _iLen;
	}
//	TRACE("m_iRecvLen = %d \n", m_iRecvLen);
	while ((m_iRecvLen - iLen)> sizeof(unsigned int))
	{

		memcpy(&ui32Head, m_cData + iLen, 4);
		if (ui32Head == COMMPACKHEAD)
		{
			if ((m_iRecvLen - iLen) < sizeof(COMMANDHEAD))
			{
				break;
			}
			memset(&cmdHeader, 0, sizeof(COMMANDHEAD));
			memcpy(&cmdHeader, m_cData + iLen, sizeof(COMMANDHEAD));
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
				AnalyzeCmd(m_cData + iLen + sizeof(COMMANDHEAD),
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
			memcpy(&dataHeader, m_cData + iLen, sizeof(STREAMHEAD));
//			TRACE("dataHeader.m_ui16Size = %d \n", dataHeader.m_ui16Size);
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
				ChnRecevedata(m_cData + iLen + sizeof(STREAMHEAD),
					dataHeader.m_ui16Size - sizeof(STREAMHEAD));
			}

			iLen += dataHeader.m_ui16Size;
//			TRACE("iLen = %d \n", iLen);
		}
		else
		{
			iLen += 1;
		}
	}
	//将剩余数据前移,并更新数据指针
	if (m_iRecvLen > iLen)
		memmove(m_cData, m_cData + iLen, m_iRecvLen - iLen);
	m_iRecvLen -= iLen;
	if (m_iRecvLen < 0) m_iRecvLen = 0;
}
bool videoChn::ChnRecevedata(char *_data, int _iBuflength)
{
	
//	TRACE("_iBuflength = %d \n", _iBuflength);
	char *ucPtr = _data;
	int iLen = _iBuflength;
	if (strcmp(ucPtr, "IEC") == 0)
	{
	//	TRACE("IEC12345678899\n");
		ucPtr += 8;
		iLen -= 8;
	}

	if (iLen > 0)
	{
		AVData_WriteBuffer(ucPtr, iLen);
	}
	return 0;
}

UINT ThreadDecode(LPVOID pParam)
{
	videoChn *Chn = (videoChn *)pParam;
	int iRet = -1;
	pS_FrameHeader pv = NULL;
	int iLen = 0;
	char *cBuf;
	H264_DEC_FRAME_S *dec_frame = &(Chn->m_tDecoder.m_tdec_frame);
	TRACE("Chn->m_iConnectStat= %d \n", Chn->m_iConnectStat);
	while (Chn->m_iConnectStat)
	{
		TRACE("Chn->m_iConnectStat= %d Chn->m_iVideoDatalen %d\n", Chn->m_iConnectStat, Chn->m_iVideoDatalen);
		if (Chn->m_iVideoDatalen <= 0)
		{
			Sleep(40);
			continue;
		}
		
		Chn->m_iLock.Lock();
		Chn->m_iVideoDecFramNo++;
		TRACE("m_iVideoDecFramNo = %d tm = %umsd\n", Chn->m_iVideoDecFramNo, GetTickCount());
		pv = (pS_FrameHeader)(Chn->m_cVideoData);

		iLen = pv->u32StreamLen;
		cBuf = Chn->m_cVideoData + sizeof(S_FrameHeader);
		iRet = Chn->m_tDecoder.m_pHi264DecAu(Chn->m_tDecoder.m_hHandle, (HI_U8 *)(cBuf), (HI_U32)(iLen),0, dec_frame, 0);
		Chn->m_iVideoDatalen -= pv->u32FrameSize;
		TRACE("Chn->m_iVideoDatalen =%d \n", Chn->m_iVideoDatalen);
		memmove(Chn->m_cVideoData, Chn->m_cVideoData + pv->u32FrameSize, Chn->m_iVideoDatalen);
		Chn->m_iLock.Unlock();
		TRACE("iRet =%d \n", iRet);
		if (HI_H264DEC_OK == iRet)                                                /* 输出一帧图像 */
		{
			//TRACE("RealDisplay\n ");
			//critical_section[pInfo->iJ].Lock();
			const HI_U8 *pY = dec_frame->pY;
			const HI_U8 *pU = dec_frame->pU;
			const HI_U8 *pV = dec_frame->pV;
			HI_U32 width = dec_frame->uWidth;
			HI_U32 height = dec_frame->uHeight;
			HI_U32 yStride = dec_frame->uYStride;
			HI_U32 uvStride = dec_frame->uUVStride;
		//	TRACE("width = %d height =%d yStride =%d uvStride =%d \n", width, height, yStride, uvStride);
			memcpy(Chn->yuv, pY, height* yStride);                        //真正实现的含义是：把各分量存到相应的数组中
			memcpy(&Chn->yuv[height* yStride], pU, height*uvStride / 2);
			memcpy(&Chn->yuv[height* yStride + height* uvStride / 2], pV, height* uvStride / 2);  //依次存入,防止出现绿色
			Chn->m_tDraw.DirectDrawYUV(Chn->yuv);
			//Sleep(1);
		}
				
		Sleep(1);
		
	}
	return 0;
}