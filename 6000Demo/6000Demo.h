
// 6000Demo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMy6000DemoApp: 
// �йش����ʵ�֣������ 6000Demo.cpp
//

class CMy6000DemoApp : public CWinApp
{
public:
	CMy6000DemoApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMy6000DemoApp theApp;