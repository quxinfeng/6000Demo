#pragma once
#include <winsock.h>
#include "afxsock.h"

#define TAB  "\t"
#define END "\n\n\n"



#define  COMMPACKHEAD			0xF5EAF5F1    //命令包包头
#define  DATAPACKHEAD			0xF9EAF5F1    //数据包包头 

//命令包包头
typedef struct
{
	unsigned int     m_ui32Head;			    //命令包包头
	unsigned short	   m_ui16Serial;	        //序列号,用来判断是否丢包
	unsigned short     m_ui16CommandSize;       //命令总长度
	unsigned int 	   m_ui32Target;		    //命令包目的地，如果为全1则是广播包
	unsigned int 	   m_ui32Source;	        //命令包源地址
	unsigned short	   m_ui16Order;		        //命令码
	unsigned short	   m_ui16ParaCount;	        //参数的长度
}COMMANDHEAD, *PCOMMANDHEAD;

typedef struct
{
	uint32	m_ui32Head;			//数据包包头
	uint32    m_ui32Serial;         //序列号
	uint32	  m_ui32Source;	        //数据包源地址
	uint8     m_ui8Channel;         //通道号
	uint8     m_ui8FrameType;       //帧类型
	uint16    m_ui16Size;      		//数据长度
	uint32    m_ui32StreamType;//码流类型
}STREAMHEAD, *PSTREAMHEAD;
SOCKET  CreateSocket(char* _pcIp, int _iPort);

int SocketClose(IN SOCKET _iSocket);

int SocketReceveBuf(IN SOCKET _iSocket, __in int iLenRecv, __in int iFlagsRecv, __out char* pcBufRecv);

int SocketSendbuf(IN SOCKET _iSocket, IN const char FAR * pcBufSend, IN int iLenSend, IN int iFlagsSend);

UINT ThreadReceiveData(LPVOID pParam);
UINT ThreadReceiveVideoData(LPVOID pParam);