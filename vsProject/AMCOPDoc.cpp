// AMCOPDoc.cpp : implementation of the CAMCOPDoc class
//

#include "stdafx.h"
#include "AMCOP.h"

#include "AMCOPDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAMCOPDoc

IMPLEMENT_DYNCREATE(CAMCOPDoc, CDocument)

BEGIN_MESSAGE_MAP(CAMCOPDoc, CDocument)
	//{{AFX_MSG_MAP(CAMCOPDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAMCOPDoc construction/destruction

CAMCOPDoc::CAMCOPDoc()
{
	// TODO: add one-time construction code here

}

CAMCOPDoc::~CAMCOPDoc()
{
}

BOOL CAMCOPDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CAMCOPDoc serialization

void CAMCOPDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAMCOPDoc diagnostics

#ifdef _DEBUG
void CAMCOPDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAMCOPDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAMCOPDoc commands
