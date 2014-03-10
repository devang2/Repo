//--------------------------------------------------------------------
//
//	QRmakerPro.ocx�g�p���� C++ �̃T���v���v���O����
//
//  Copyright (C) DENSO CORPORATION 2000 all rights reserved
//--------------------------------------------------------------------
// testOCX2View.h : CTestOCX2View �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂��B
//
/////////////////////////////////////////////////////////////////////////////
#include "qrmakerpro.h"

#if !defined(AFX_TESTOCX2VIEW_H__2B9C998E_DCF0_11D1_8D3B_A05802C10000__INCLUDED_)
#define AFX_TESTOCX2VIEW_H__2B9C998E_DCF0_11D1_8D3B_A05802C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CTestOCX2CntrItem;

class CTestOCX2View : public CFormView
{
protected: // �V���A���C�Y�@�\�݂̂���쐬���܂��B
	CTestOCX2View();
	DECLARE_DYNCREATE(CTestOCX2View)

public:
	//{{AFX_DATA(CTestOCX2View)
	enum { IDD = IDD_TESTOCX2_FORM };
	CComboBox	m_Combo_Unit;
	CComboBox	m_Combo_Ecc;
	CSpinButtonCtrl	m_SpinAdjust;
	CEdit	m_EditAdjust;
	CSpinButtonCtrl	m_SpinQZone;
	CEdit	m_EditQZone;
	CEdit	m_EditModelNo;
	CSpinButtonCtrl	m_SpinModelNo;
	CSpinButtonCtrl	m_SpinPitch;
	CEdit	m_EditPitch;
	CString	m_EditInputData;
	CQRmakerPro	m_QRmaker;
	//}}AFX_DATA

// �A�g���r���[�g
public:
	CTestOCX2Doc* GetDocument();
	//   m_pSelection �͌��݂� CTestOCX2CntrItem �ɑ΂���I����ێ����Ă��܂��B
	//  �����̃A�v���P�[�V�����ł��̂悤�ȃ����o�ϐ��́A�����̑I���� CTestOCX2CntrItem
	//  �I�u�W�F�N�g�ł͂Ȃ��I�u�W�F�N�g�̑I�����\���ɕ\���ł��܂���B
	//  ���̋@�\�͑I���@�\�����p���悤�Ƃ���v���O���}�̗����������邽
	//  �߂ɑg�ݍ��܂�Ă��܂��B

	// TODO: ���̑I����@���A�v���P�[�V�����ɓK�������@�ɒu�������Ă��������B
	CTestOCX2CntrItem* m_pSelection;

// �I�y���[�V����
public:

// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CTestOCX2View)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �̃T�|�[�g
	virtual void OnInitialUpdate(); // �\�z��̍ŏ��̂P�x�����Ăяo����܂��B
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo*);
	virtual BOOL IsSelected(const CObject* pDocItem) const;// OLE �R���e�i �T�|�[�g
	virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
public:
	int m_Height;
	HMETAFILE m_hmf;
	virtual ~CTestOCX2View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// �������ꂽ���b�Z�[�W �}�b�v�֐�
protected:
	//{{AFX_MSG(CTestOCX2View)
	afx_msg void OnDestroy();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnInsertObject();
	afx_msg void OnCancelEditCntr();
	afx_msg void OnCancelEditSrvr();
	afx_msg void OnButtonCopyPicture();
	afx_msg void OnChangeEditInputdata();
	afx_msg void OnAppExit();
	afx_msg void OnButtonGethandle();
	afx_msg void OnChangeEditPitch();
	afx_msg void OnChangeEditModelno();
	afx_msg void OnChangeEditQzone();
	afx_msg void OnChangeEditAdjust();
	afx_msg void OnButtonBinary();
	afx_msg void OnCloseupComboEcc();
	afx_msg void OnCloseupComboUnit();
	afx_msg void OnButtonDrawqrimage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // testOCX2View.cpp �t�@�C�����f�o�b�O���̎��g�p����܂��B
inline CTestOCX2Doc* CTestOCX2View::GetDocument()
   { return (CTestOCX2Doc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_TESTOCX2VIEW_H__2B9C998E_DCF0_11D1_8D3B_A05802C10000__INCLUDED_)
