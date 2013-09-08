// AMCOPDoc.h : interface of the CAMCOPDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_AMCOPDOC_H__5BA64B7F_B3FE_4E36_A80C_BE1842E36D4C__INCLUDED_)
#define AFX_AMCOPDOC_H__5BA64B7F_B3FE_4E36_A80C_BE1842E36D4C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CAMCOPDoc : public CDocument
{
protected: // create from serialization only
	CAMCOPDoc();
	DECLARE_DYNCREATE(CAMCOPDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAMCOPDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAMCOPDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CAMCOPDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AMCOPDOC_H__5BA64B7F_B3FE_4E36_A80C_BE1842E36D4C__INCLUDED_)
