//--------------------------------------------------------------------
//
//	QRmakerPro.ocx�g�p���� C++ �̃T���v���v���O����
//
//  Copyright (C) DENSO CORPORATION 2000 all rights reserved
//--------------------------------------------------------------------
// testOCX2.h : TESTOCX2 �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C��
//

#if !defined(AFX_TESTOCX2_H__2B9C9986_DCF0_11D1_8D3B_A05802C10000__INCLUDED_)
#define AFX_TESTOCX2_H__2B9C9986_DCF0_11D1_8D3B_A05802C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // ���C�� �V���{��

/////////////////////////////////////////////////////////////////////////////
// CTestOCX2App:
// ���̃N���X�̓���̒�`�Ɋւ��Ă� testOCX2.cpp �t�@�C�����Q�Ƃ��Ă��������B
//

class CTestOCX2App : public CWinApp
{
public:
	CTestOCX2App();

// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CTestOCX2App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
	COleTemplateServer m_server;
		// �h�L�������g���쐬���邽�߂̃T�[�o�[ �I�u�W�F�N�g

	//{{AFX_MSG(CTestOCX2App)
	afx_msg void OnAppAbout();
		// ���� - ClassWizard �͂��̈ʒu�Ƀ����o�֐���ǉ��܂��͍폜���܂��B
		//        ���̈ʒu�ɐ��������R�[�h��ҏW���Ȃ��ł��������B
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_TESTOCX2_H__2B9C9986_DCF0_11D1_8D3B_A05802C10000__INCLUDED_)
