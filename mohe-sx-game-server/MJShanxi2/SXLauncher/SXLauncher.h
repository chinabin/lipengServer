
// SXLauncher.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CSXLauncherApp: 
// �йش����ʵ�֣������ SXLauncher.cpp
//

class CSXLauncherApp : public CWinApp
{
public:
	CSXLauncherApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CSXLauncherApp theApp;