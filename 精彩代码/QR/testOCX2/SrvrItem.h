//--------------------------------------------------------------------
//
//	QRmakerPro.ocx�g�p���� C++ �̃T���v���v���O����
//
//  Copyright (C) DENSO CORPORATION 2000 all rights reserved
//--------------------------------------------------------------------
// SrvrItem.h : CTestOCX2SrvrItem �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂��B
//

#if !defined(AFX_SRVRITEM_H__2B9C9991_DCF0_11D1_8D3B_A05802C10000__INCLUDED_)
#define AFX_SRVRITEM_H__2B9C9991_DCF0_11D1_8D3B_A05802C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CTestOCX2SrvrItem : public CDocObjectServerItem
{
	DECLARE_DYNAMIC(CTestOCX2SrvrItem)

// �R���X�g���N�^
public:
	CTestOCX2SrvrItem(CTestOCX2Doc* pContainerDoc);

// �A�g���r���[�g
	CTestOCX2Doc* GetDocument() const
		{ return (CTestOCX2Doc*)CDocObjectServerItem::GetDocument(); }

// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CTestOCX2SrvrItem)
	public:
	virtual BOOL OnDraw(CDC* pDC, CSize& rSize);
	virtual BOOL OnGetExtent(DVASPECT dwDrawAspect, CSize& rSize);
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
public:
	~CTestOCX2SrvrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void Serialize(CArchive& ar);   // �h�L�������g I/O �ɑ΂��ăI�[�o�[���C�h����܂��B
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_SRVRITEM_H__2B9C9991_DCF0_11D1_8D3B_A05802C10000__INCLUDED_)
