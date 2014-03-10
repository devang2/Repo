// CntrItem.h : CTestOCX2CntrItem �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂��B
//

#if !defined(AFX_CNTRITEM_H__2B9C9995_DCF0_11D1_8D3B_A05802C10000__INCLUDED_)
#define AFX_CNTRITEM_H__2B9C9995_DCF0_11D1_8D3B_A05802C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CTestOCX2Doc;
class CTestOCX2View;

class CTestOCX2CntrItem : public COleClientItem
{
	DECLARE_SERIAL(CTestOCX2CntrItem)

// �R���X�g���N�^
public:
	CTestOCX2CntrItem(CTestOCX2Doc* pContainer = NULL);
		// ����: pContainer �� NULL �ɂ���� IMPLEMENT_SERIALIZE ���s�����Ƃ��ł��܂��B
		//  IMPLEMENT_SERIALIZE ���s�����߂ɂ̓N���X���Ɉ����̂Ȃ��R���X�g���N�^���K�v�ł��B
		//  �ʏ�AOLE �A�C�e���� NULL �łȂ��h�L�������g �|�C���^�őg�ݍ��܂�Ă��܂��B
		//

// �A�g���r���[�g
public:
	CTestOCX2Doc* GetDocument()
		{ return (CTestOCX2Doc*)COleClientItem::GetDocument(); }
	CTestOCX2View* GetActiveView()
		{ return (CTestOCX2View*)COleClientItem::GetActiveView(); }

	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CTestOCX2CntrItem)
	public:
	virtual void OnChange(OLE_NOTIFICATION wNotification, DWORD dwParam);
	virtual void OnActivate();
	protected:
	virtual void OnGetItemPosition(CRect& rPosition);
	virtual void OnDeactivateUI(BOOL bUndoable);
	virtual BOOL OnChangeItemPosition(const CRect& rectPos);
	virtual BOOL CanActivate();
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
public:
	~CTestOCX2CntrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual void Serialize(CArchive& ar);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_CNTRITEM_H__2B9C9995_DCF0_11D1_8D3B_A05802C10000__INCLUDED_)
