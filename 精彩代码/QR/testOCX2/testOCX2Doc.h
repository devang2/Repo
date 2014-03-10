//--------------------------------------------------------------------
//
//	QRmakerPro.ocx�g�p���� C++ �̃T���v���v���O����
//
//  Copyright (C) DENSO CORPORATION 2000 all rights reserved
//--------------------------------------------------------------------
// testOCX2Doc.h : CTestOCX2Doc �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂��B
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTOCX2DOC_H__2B9C998C_DCF0_11D1_8D3B_A05802C10000__INCLUDED_)
#define AFX_TESTOCX2DOC_H__2B9C998C_DCF0_11D1_8D3B_A05802C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CTestOCX2SrvrItem;

class CTestOCX2Doc : public COleServerDoc
{
protected: // �V���A���C�Y�@�\�݂̂���쐬���܂��B
	CTestOCX2Doc();
	DECLARE_DYNCREATE(CTestOCX2Doc)

// �A�g���r���[�g
public:
	CTestOCX2SrvrItem* GetEmbeddedItem()
		{ return (CTestOCX2SrvrItem*)COleServerDoc::GetEmbeddedItem(); }

// �I�y���[�V����
public:

//�I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CTestOCX2Doc)
	protected:
	virtual COleServerItem* OnGetEmbeddedItem();
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
public:
	virtual ~CTestOCX2Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual CDocObjectServer* GetDocObjectServer(LPOLEDOCUMENTSITE pDocSite);

// �������ꂽ���b�Z�[�W �}�b�v�֐�
protected:
	//{{AFX_MSG(CTestOCX2Doc)
		// ���� - ClassWizard �͂��̈ʒu�Ƀ����o�֐���ǉ��܂��͍폜���܂��B
		//        ���̈ʒu�ɐ��������R�[�h��ҏW���Ȃ��ł��������B
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_TESTOCX2DOC_H__2B9C998C_DCF0_11D1_8D3B_A05802C10000__INCLUDED_)
