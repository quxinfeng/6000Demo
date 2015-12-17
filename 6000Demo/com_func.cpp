#include "stdafx.h"
#include <stdio.h>
#include "com_fun.h"


void SetVideoSizeCombobox(CComboBox *cbx, int _iVideoSize)
{
	if (_iVideoSize == CIF)
	{
		cbx->SelectString(0, "CIF");
	}
	else if (_iVideoSize == D1)
	{
		cbx->SelectString(0, "D1");
	}
	else if (_iVideoSize == HD720)
	{
		cbx->SelectString(0, "720P");
	}
	else if (_iVideoSize == HD960)
	{
		cbx->SelectString(0, "960P");
	}
	else if (_iVideoSize == HD1080)
	{
		cbx->SelectString(0, "1080P");
	}

		 

}


void SetVodevsize(CComboBox *cbx, int _iVoSize)
{
	if (_iVoSize == VO_OUT_800x600_60)
	{
		cbx->SelectString(0, "800*600");
	}
	else if (_iVoSize == VO_OUT_1024x768_60)
	{
		cbx->SelectString(0, "1024*768");
	}
	else if (_iVoSize == VO_OUT_1366x768_60)
	{
		cbx->SelectString(0, "1366*768");
	}
	else if (_iVoSize == VO_OUT_1440x900_60)
	{
		cbx->SelectString(0, "1440*900");
	}
	else if (_iVoSize == VO_OUT_720P50)
	{
		cbx->SelectString(0, "720P50");
	}
	else if (_iVoSize == VO_OUT_720P60)
	{
		cbx->SelectString(0, "720P60");
	}
	else if (_iVoSize == VO_OUT_1080P50)
	{
		cbx->SelectString(0, "1080P50");
	}
	else if (_iVoSize == VO_OUT_1080P60)
	{
		cbx->SelectString(0, "1080P60");
	}


}

void GetVodevsize(CComboBox *cbx, int *_iVoSize)
{
	CString Text;
	int iIndex = cbx->GetCurSel();
	cbx->GetLBText(iIndex, Text);

	if (strcmp(Text.GetBuffer(), "800*600") == 0)
	{
		*_iVoSize = VO_OUT_800x600_60;
	}
	else if (strcmp(Text.GetBuffer(), "1024*768") == 0)
	{
		*_iVoSize = VO_OUT_1024x768_60;
	}
	else if (strcmp(Text.GetBuffer(), "1366*768") == 0)
	{
		*_iVoSize = VO_OUT_1366x768_60;
	}
	else if (strcmp(Text.GetBuffer(), "1440*900") == 0)
	{
		*_iVoSize = VO_OUT_1440x900_60;
	}
	else if (strcmp(Text.GetBuffer(), "720P50") == 0)
	{
		*_iVoSize = VO_OUT_720P50;
	}
	else if (strcmp(Text.GetBuffer(), "720P60") == 0)
	{
		*_iVoSize = VO_OUT_720P60;
	}
	else if (strcmp(Text.GetBuffer(), "1080P50") == 0)
	{
		*_iVoSize = VO_OUT_1080P50;
	}
	else if (strcmp(Text.GetBuffer(), "1080P60") == 0)
	{
		*_iVoSize = VO_OUT_1080P60;
	}
	


}
void SetComboboxInt(CComboBox *cbx, int _iint)
{
	char cTmp[16] = { 0 };
	_itoa(_iint, cTmp, 10);
	cbx->SelectString(0, cTmp);

}


void SetOsdcolorCbx(CComboBox *cbx, int _iColor)
{
	int iIndex = 0;
	switch (_iColor)
	{
	case 0x0:
		iIndex = 0;
		break;
	case 0XFF:
		iIndex = 1;
		break;
	case 0XFF00:
		iIndex = 2;
		break;
	case 0XFF0000:
		iIndex = 3;
		break;
	case 0XFFFF:
		iIndex = 4;
		break;
	case 0XFF00FF:
		iIndex = 5;
		break;
	case 0XFFFF00:
		iIndex = 6;
		break;
	case 0XFFFFFF:
		iIndex = 7;
		break;
	default:
		iIndex = 8;
		break;
	}
	
	cbx->SetCurSel(iIndex);
	
}

void GetOsdcolorCbx(CComboBox *cbx, int *_iColor)
{

	int iIndex = cbx->GetCurSel();
	int iColor = 0;
	switch (iIndex)
	{
	case 0:
		iColor = 0X0;
		break;
	case 1:
		iColor = 0XFF;
		break;
	case 2:
		iColor = 0XFF00;
		break;
	case 3:
		iColor = 0XFF0000;
		break;
	case 4:
		iColor = 0XFFFF;
		break;
	case 5:
		iColor = 0XFF00FF;
		break;
	case 6:
		iColor = 0XFFFF00;
		break;
	case 7:
		iColor = 0XFFFFFF;
		break;
	default:
		iColor = 0XFFFFFF;
		break;
	}

	*_iColor = iColor;

}

int OnCharLeft(const char* c1, const char* c2, char* c3)
{
	int i = 0, j = 0;

	char* cPos = NULL;
	cPos = (char *)strstr(c1, c2);
	if (cPos != NULL)
	{
		i = cPos - c1;
		for (j = 0; j < i; j++)
			c3[j] = c1[j];
	}
	c3[i] = 0;
	return 0;
}

int OnCharRight(const char* c1, const char* c2, char* c3)
{
	int i;
	char* cPos = NULL;
	i = strlen(c2);
	cPos = (char *)strstr(c1, c2);
	if (cPos != NULL)
	{
		memcpy(c3, cPos + i, strlen(cPos) - i);
		c3[strlen(cPos) - i] = 0;
	}
	else
	{
		c3[0] = 0;
	}

	return 0;
}
int SplitString(unsigned char* _pcSrc, unsigned char* _pcSplit, unsigned char** _ppcDst, int _iFieldLen, int _iFieldNum)
{
	if (_pcSrc == NULL || _pcSplit == NULL || _ppcDst == NULL)
	{
		return -1;
	}
	int i = 0, j = 0, k = 0, iLen = strlen((char *)_pcSrc);
	for (i = 0; i < iLen; i++)
	{
		if (_pcSrc[i] != _pcSplit[0])
		{
			_ppcDst[j][k++] = _pcSrc[i];
			if (k >= _iFieldLen)
			{
				return -2;
			}
		}
		else
		{
			j++;
			k = 0;
			if (j >= _iFieldNum)
			{
				return -3;
			}
		}
	}
	return ++j;
}



