// waveObject.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CwaveObjectApp:
// �йش����ʵ�֣������ waveObject.cpp
//

class CwaveObjectApp : public CWinApp
{
public:
	CwaveObjectApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CwaveObjectApp theApp;