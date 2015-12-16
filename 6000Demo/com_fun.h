#pragma once
typedef enum
{
	QCIF = 0,
	HCIF,
	CIF,
	HD1,
	D1,
	F960H,   /* 960 * 576 */

	QVGA,    /* 320 * 240 */
	VGA,     /* 640 * 480 */
	SVGA,    /* 800 * 600 */
	XGA,     /* 1024 * 768 */
	WXGA1,   /* 1280 * 768 */
	WXGA2,   /* 1440 * 900 */
	SXGA1,   /* 1280 * 1024 */
	SXGA2,   /* 1400 * 1050 */
	UXGA,    /* 1600 * 1200 */
	QXGA,    /* 2048 * 1536 */

	WVGA,    /* 854 * 480 */
	WSXGA,   /* 1680 * 1050 */
	WUXGA,   /* 1920 * 1200 */
	WQXGA,   /* 2560 * 1600 */

	HD720,   /* 1280 * 720 */
	HD960,   /* 1280 * 960 */
	HD1080,  /* 1920 * 1080 */

	VIEDOSIZE_BUTT
}VIDEO_SIZE_E;



typedef enum //����˼�ı���һ��
{
	VO_OUT_PAL = 0,
	VO_OUT_NTSC,

	VO_OUT_1080P24,
	VO_OUT_1080P25,
	VO_OUT_1080P30,

	VO_OUT_720P50,
	VO_OUT_720P60,
	VO_OUT_1080I50,
	VO_OUT_1080I60,
	VO_OUT_1080P50,
	VO_OUT_1080P60,

	VO_OUT_576P50,
	VO_OUT_480P60,

	VO_OUT_800x600_60,            /* VESA 800 x 600 at 60 Hz (non-interlaced) */
	VO_OUT_1024x768_60,           /* VESA 1024 x 768 at 60 Hz (non-interlaced) */
	VO_OUT_1280x1024_60,          /* VESA 1280 x 1024 at 60 Hz (non-interlaced) */
	VO_OUT_1366x768_60,           /* VESA 1366 x 768 at 60 Hz (non-interlaced) */
	VO_OUT_1440x900_60,           /* VESA 1440 x 900 at 60 Hz (non-interlaced) CVT Compliant */
	VO_OUT_1280x800_60,           /* 1280*800@60Hz VGA@60Hz*/
	VO_OUT_1600x1200_60,          /* VESA 1600 x 1200 at 60 Hz (non-interlaced) */
	VO_OUT_1680x1050_60,          /* VESA 1680 x 1050 at 60 Hz (non-interlaced) */
	VO_OUT_1920x1200_60,          /* VESA 1920 x 1600 at 60 Hz (non-interlaced) CVT (Reduced Blanking)*/

	VO_OUT_USER,
	VO_OUT_BUTT

}VO_DEV_SIZE;


typedef struct
{
	//unsigned int 	u32FrameForm;	//֡���ͣ���Ƶ֡������Ƶ֡
	unsigned int    u32FrameID;    //0x 00 00 00 02
	unsigned char	u8FrameType;	//֡����,8λ,FRAME_I=0,  FRAME_P=1
	unsigned char	u8ACoder;       //���뷽ʽ
	unsigned short	u16AFrames;     //��Ƶ֡��
	unsigned short	u16AFrameSize;   //��Ƶ�����ܴ�С
	unsigned int	u32FrameSize;   //����Ƶ֡��С
	unsigned int	u32FrameNO;	    //֡���,32λ,֡���,ѭ������
	unsigned int	u32TimeStamp;	//��Ƶʱ���,32λ
	unsigned int	u32StreamLen;	//��Ƶ���ݳ���
}S_FrameHeader, *pS_FrameHeader;

typedef struct
{
	unsigned int	u32TimeStamp;	//ʱ���,32λ
	unsigned int	u32StreamLen;	//���ݳ���,32λ,���ݴ�С
}A_FramHeader, *pA_FrameHdeader;
enum FrameType
{
	TD_IFRAME,
	TD_PFRAME,
	TD_EFRAME
};


typedef struct
{
	int m_iType;
	int m_iDispaly;
	int m_iColor;
	int m_iTransport;
	int m_iX;
	int m_iY;
	char m_strWord[256];
}TChnOverlay;
#define MAX_VIDEOSIZE_LIST 32
#define MAX_AUDIOCODER_LIST 16 


typedef struct				//��Ƶ�����������
{
	int m_iHue;				//ɫ���� 0-255
	int m_iLum;				//������ 0-255
	int m_iContrast;			//�Աȶ� 0-255
	int m_iSaturation;		//���Ͷ� 0-255
} VideoP;

typedef struct
{
	int 	m_iWidth;				//��Ƶ���
	int 	m_iHeight;				//��Ƶ�߶�
	int 	m_iType;				//��Ƶѹ����ʽ
	int 	m_iFrame;				//��Ƶ֡��
}VideoDim;	//��Ƶ�ߴ�

typedef struct
{
	int 	m_iWidth;			//ͼ����
	int 	m_iHeight;			//ͼ��߶�
	int 	m_iVencType;			//���뷽ʽ11(H263,T), 21-22(H264,S),  31(MP4,T+), 41(MJPEG��S2) 51(MP4 S2)

	VideoP	m_ViCnf;	//��Ƶ���������ȶԱȶȵ�

	int		m_iStreamType;		//��������3������Ƶ��������2����Ƶ����1����Ƶ��
	int		m_iIFrameRate;		//I֡֡��
	int		m_iBitRate;			//����
	int		m_iEncodeType;
	int		m_iFrameRate;		//֡��
	int		m_iVideoSize;		//��Ƶ��С
	int 	m_iVideoQuality;		//��Ƶ����

	VideoDim		m_VideoDim;

	int		m_iEncodeMode;		//ѹ������0���������ȣ�1����������

	int		m_iAudioSample;		//��Ƶ������8��32��48

	int		m_iAudioSampleCnt;

	int		m_iAudioSampleList[16];//֧�ֵ���Ƶ�������б�

	int		m_iAudioCoderListCnt;

	int		m_iAudioCoderList[MAX_AUDIOCODER_LIST];

	int		m_iVideoList[MAX_VIDEOSIZE_LIST];	// ��Ƶ�ֱ����б�
}VideoParam;

typedef struct
{
	int iIndex;
	int iChn;
	BOOL blUsed;
	BOOL blSelected;
	CStatic *m_tVo;
}TChnvo;


void SetVideoSizeCombobox(CComboBox *cbx, int _iVideoSize);
void SetVodevsize(CComboBox *cbx, int _iVoSize);

void GetVodevsize(CComboBox *cbx, int *_iVoSize);
void SetComboboxInt(CComboBox *cbx, int _iint);
int OnCharLeft(const char* c1, const char* c2, char* c3);


int OnCharRight(const char* c1, const char* c2, char* c3);


int SplitString(unsigned char* _pcSrc, unsigned char* _pcSplit, unsigned char** _ppcDst, int _iFieldLen, int _iFieldNum);
