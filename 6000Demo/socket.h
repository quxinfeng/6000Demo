#pragma once
#include <winsock.h>
#include "afxsock.h"

#define TAB  "\t"
#define END "\n\n\n"



#define  COMMPACKHEAD			0xF5EAF5F1    //�������ͷ
#define  DATAPACKHEAD			0xF9EAF5F1    //���ݰ���ͷ 

//�������ͷ
typedef struct
{
	unsigned int     m_ui32Head;			    //�������ͷ
	unsigned short	   m_ui16Serial;	        //���к�,�����ж��Ƿ񶪰�
	unsigned short     m_ui16CommandSize;       //�����ܳ���
	unsigned int 	   m_ui32Target;		    //�����Ŀ�ĵأ����Ϊȫ1���ǹ㲥��
	unsigned int 	   m_ui32Source;	        //�����Դ��ַ
	unsigned short	   m_ui16Order;		        //������
	unsigned short	   m_ui16ParaCount;	        //�����ĳ���
}COMMANDHEAD, *PCOMMANDHEAD;

typedef struct
{
	uint32	m_ui32Head;			//���ݰ���ͷ
	uint32    m_ui32Serial;         //���к�
	uint32	  m_ui32Source;	        //���ݰ�Դ��ַ
	uint8     m_ui8Channel;         //ͨ����
	uint8     m_ui8FrameType;       //֡����
	uint16    m_ui16Size;      		//���ݳ���
	uint32    m_ui32StreamType;//��������
}STREAMHEAD, *PSTREAMHEAD;
SOCKET  CreateSocket(char* _pcIp, int _iPort);

int SocketClose(IN SOCKET _iSocket);

int SocketReceveBuf(IN SOCKET _iSocket, __in int iLenRecv, __in int iFlagsRecv, __out char* pcBufRecv);

int SocketSendbuf(IN SOCKET _iSocket, IN const char FAR * pcBufSend, IN int iLenSend, IN int iFlagsSend);

UINT ThreadReceiveData(LPVOID pParam);
UINT ThreadReceiveVideoData(LPVOID pParam);