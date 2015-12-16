#include "stdafx.h"
#include "Datacenter.h"


Datacenter::Datacenter()
{
	m_iReadNo = 0;
	m_iWriteNo = 0;
	m_iInitFlg = 0; 
}


Datacenter::~Datacenter()
{

}

void Datacenter::initDatacenter()
{
	int i;
	for (i = 0; i < 5; i++)
	{
		memset(&m_tDatabuf[i], 0, sizeof(DataBuf));
		m_tDatabuf[i].m_iLevel = 1;
		m_tDatabuf[i].m_cData = cLevel1buf[i];
	}
	for (i = 5; i < 10; i++)
	{
		memset(&m_tDatabuf[i], 0, sizeof(DataBuf));
		m_tDatabuf[i].m_iLevel = 2;
		m_tDatabuf[i].m_cData = cLevel2buf[i - 5];

	}
	for (i = 10; i < 12; i++)
	{
		memset(&m_tDatabuf[i], 0, sizeof(DataBuf));
		m_tDatabuf[i].m_iLevel = 3;
		m_tDatabuf[i].m_cData = cLevel2buf[i - 10];
	}
	m_iInitFlg = 1;
}
int Datacenter::DataAddtoDatacenter(char *_cData, int _iLength)
{
	
	int i;
	int iGet = 0;
	for (i = 0; i < 5; i++)
	{
		if (m_tDatabuf[i].m_iUsed)
		{
			continue;
		}
		if ((_iLength < 128 * 1024) && m_tDatabuf[i].m_iUsed == 0)
		{
			iGet = 1;
			m_iLock.Lock();
			memcpy(m_tDatabuf[i].m_cData, _cData, _iLength);
			m_tDatabuf[i].m_datasize = _iLength;
			m_tDatabuf[i].m_iUsed = 1;
			m_tDatabuf[i].m_iId = m_iWriteNo;
			m_iWriteNo++;
			m_iLock.Unlock();
			goto END;
			
		}
		
	}
	for (i = 5; i < 10; i++)
	{
		if (m_tDatabuf[i].m_iUsed)
		{
			continue;
		}
		if (_iLength < 512 * 1024&& m_tDatabuf[i].m_iUsed == 0)
		{
			iGet = 1;
			m_iLock.Lock();
			memcpy(m_tDatabuf[i].m_cData, _cData, _iLength);
			m_tDatabuf[i].m_datasize = _iLength;
			m_tDatabuf[i].m_iUsed = 1;
			m_tDatabuf[i].m_iId = m_iWriteNo;
			m_iWriteNo++;
			m_iLock.Unlock();
			goto END;

		}
	}

	for (i = 10; i < 12; i++)
	{
		if (m_tDatabuf[i].m_iUsed)
		{
			continue;
		}
		if ((_iLength < 1024 * 1024) && m_tDatabuf[i].m_iUsed == 0)
		{
			iGet = 1;
			m_iLock.Lock();
			memcpy(m_tDatabuf[i].m_cData, _cData, _iLength);
			m_tDatabuf[i].m_datasize = _iLength;
			m_tDatabuf[i].m_iUsed = 1;
			m_tDatabuf[i].m_iId = m_iWriteNo;
			m_iWriteNo++;
			m_iLock.Unlock();
			goto END;

		}
	}
END:
	if (!iGet)
	{
		return -1;
	}
	return 0;
}
DataBuf* Datacenter::ReadFromDatacenter()
{
	int i;
	for (i = 0; i < 12; i++)
	{
		if (m_tDatabuf[i].m_iUsed && m_iReadNo == m_tDatabuf[i].m_iId)
		{
			m_iReadNo++;
			return &m_tDatabuf[i];
		}
	}
	return NULL;
}

void Datacenter::ReleaseDatafromDatacenter(DataBuf* _cDatabuf)
{
	m_iLock.Lock();
	_cDatabuf->m_iId = 0;
	_cDatabuf->m_datasize = 0;
	_cDatabuf->m_iUsed = 0;
	m_iLock.Unlock();
	
}
void Datacenter::clearDatacenter()
{
	if (m_iInitFlg == 0)
	{
		return;
	}
	int m_iReadNo = 0;
	int m_iWriteNo = 0;
	int i;
	for (i = 0; i < 12; i++)
	{
		free(m_tDatabuf[i].m_cData);
		m_tDatabuf[i].m_datasize = 0;
		m_tDatabuf[i].m_iId = 0;
		m_tDatabuf[i].m_iUsed = 0;
	}

}