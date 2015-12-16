#include <stdio.h>
#include "stdafx.h"
#include "Client.h"
#include "socket.h"
#include "videoChn.h"
SOCKET  CreateSocket(char* _pcIp, int _iPort) //socket不需要导出
{										 //加载网络库

	SOCKET iRetsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	if (INVALID_SOCKET == iRetsocket)
	{
		TRACE("%s %d create error \n", __FUNCTION__, __LINE__);
		return -1;

	}
	else
	{
		
		sockaddr_in RemoteAddr;
		RemoteAddr.sin_family = AF_INET;
		RemoteAddr.sin_port = htons(_iPort);

		RemoteAddr.sin_addr.s_addr = inet_addr(_pcIp);

		if (connect(iRetsocket, (SOCKADDR*)&RemoteAddr, sizeof(SOCKADDR)) == SOCKET_ERROR)
		{
			//OutputDebugString(msg);
			TRACE("%s %d create error\n", __FUNCTION__, __LINE__);
			return -1;
		}
		
		CString msg = "chenggggg\r\n";
		OutputDebugString(msg);
		return iRetsocket;
	}
}

int SocketSendbuf(IN SOCKET _iSocket,IN const char FAR * pcBufSend, IN int iLenSend, IN int iFlagsSend)
{
//	TRACE("pcBufSend = %s \n", pcBufSend);
	COMMANDHEAD CHead;
	memset(&CHead, 0, sizeof(COMMANDHEAD));
	CHead.m_ui32Head = COMMPACKHEAD;
	CHead.m_ui16ParaCount = iLenSend;
	CHead.m_ui16CommandSize = sizeof(COMMANDHEAD) + iLenSend;
//	TRACE("sizeof(COMMANDHEAD) = %d \n", sizeof(COMMANDHEAD));
//	TRACE("iLenSend = %d \n", iLenSend);
	char cMsg[1024] = { 0 };
	memcpy(cMsg, &CHead, sizeof(COMMANDHEAD));
	memcpy(cMsg + sizeof(COMMANDHEAD), pcBufSend, iLenSend);
	const char FAR* buf = cMsg;
	int              len = iLenSend + sizeof(COMMANDHEAD);
	int              flags = iFlagsSend;
	int iResult = send(_iSocket, buf, len, flags);

	return iResult;
}
int SocketReceveBuf(IN SOCKET _iSocket, __in int iLenRecv, __in int iFlagsRecv, __out char* pcBufRecv)
{
	char* buf = pcBufRecv;
	int   len = iLenRecv;
	int   flags = iFlagsRecv;
	int iRet = recv(_iSocket, buf, len, flags);
	int uError = GetLastError();
	return iRet;
}

int SocketClose(IN SOCKET _iSocket)
{

	return  closesocket(_iSocket);

}


 UINT ThreadReceiveData(LPVOID pParam)
{
	//char cPriRes;
	//char cRes;
	int iRet = -1;
	int iLength = 0;
	char cMsg[4096];
	Client* ClientSock = (Client*)pParam;
	TRACE("ClientSock->GetClientInitFlg() = %d \n", ClientSock->GetClientInitFlg());
	while (ClientSock->GetClientInitFlg())                                                                           //控制线程的开和关
	{
		
		memset(cMsg, 0, sizeof(cMsg));
		iRet = SocketReceveBuf(ClientSock->m_icmdSocket, 4096, 0, cMsg);
		if (iRet > 0)
		{
//			TRACE("ClientSock->m_iRet = %d \n", iRet);
			ClientSock->m_iCmdLiveTime  = 0;
//			TRACE("ClientSock->m_iCmdLiveTime = %d \n", ClientSock->m_iCmdLiveTime);
			iLength = iRet;
			ClientSock->ClientParseData(cMsg, iLength);
		}

		Sleep(5);
	}
	return 0;
}


 UINT ThreadReceiveVideoData(LPVOID pParam)
 {
	 //char cPriRes;
	 //char cRes;
	 int iRet = -1;
	 int iLength = 0;
	 char cMsg[128*1024];
	 videoChn* chn = (videoChn*)pParam;
	 TRACE("chn->m_iConnectStat = %d \n", chn->m_iConnectStat);
	 while (chn->m_iConnectStat)                                                                   //控制线程的开和关
	 {

		 iRet = SocketReceveBuf(chn->m_iDataSocket, 128 * 1024, 0, cMsg);
//		 TRACE("dataSock->m_iRet = %d \n", iRet);
		 if (iRet > 0)
		 {
			 chn->m_iDataLiveTime = 0;
		//	 TRACE(" chn->->m_iRet = %d \n", iRet);
		//	 TRACE(" chn->->m_iDataLiveTime = %d \n", chn->m_iDataLiveTime);
			 //	TRACE("ClientSock->m_cDatabuf = %s \n", ClientSock->m_cDataBuf);
			 iLength = iRet;
			 chn->AnalysisData(cMsg, iLength);
			 //	ClientSock->ClientRecevedata(ClientSock->m_cDataBuf, iLength);
		 }

		 //	TRACE("ClientSock->GetClientInitFlg() = %d \n", ClientSock->GetClientInitFlg());

	 }
	 return 0;
 }


 