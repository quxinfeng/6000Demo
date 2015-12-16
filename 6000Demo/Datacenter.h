#pragma once
#define LEVEL_1		1//128K
#define LEVEL_2		2//512K
#define LEVEL_3		3//1M
#define   LEVEL_1_CNT   5
#define   LEVEL_2_CNT   5
#define   LEVEL_3_CNT   2
#define   LEVEL_1_SIZE		128 * 1024
#define   LEVEL_2_SIZE		512 * 1024
#define   LEVEL_3_SIZE		1024 * 1024
static char cLevel1buf[LEVEL_1_CNT][LEVEL_1_SIZE];
static char cLevel2buf[LEVEL_2_CNT][LEVEL_2_SIZE];
static char cLevel3buf[LEVEL_3_CNT][LEVEL_3_SIZE];
typedef struct
{
	int m_iId;
	int m_iUsed;
	int m_iLevel;
	int m_datasize;
	char *m_cData;

}DataBuf;
class Datacenter
{	
public:
	int m_iInitFlg;
	int m_iReadNo;
	int m_iWriteNo;
	CCriticalSection m_iLock;
	DataBuf m_tDatabuf[12];
public:
	Datacenter();
	~Datacenter();
	void initDatacenter();
	int DataAddtoDatacenter(char *_cData, int _iLength);
	DataBuf* ReadFromDatacenter();
	void ReleaseDatafromDatacenter(DataBuf* _cDatabuf);
	void clearDatacenter();
};



