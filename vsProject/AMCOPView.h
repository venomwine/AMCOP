// AMCOPView.h : interface of the CAMCOPView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_AMCOPVIEW_H__E09C2E83_0684_4F3D_99C2_86D8129446B6__INCLUDED_)
#define AFX_AMCOPVIEW_H__E09C2E83_0684_4F3D_99C2_86D8129446B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../engine/ogl/oglFunctions.h"
#include "../engine/include/types.h"

class CAMCOPView : public CView
{
protected: // create from serialization only
	CAMCOPView();
	DECLARE_DYNCREATE(CAMCOPView)

// Attributes
public:
	CAMCOPDoc* GetDocument();

	enum {
		TOUCH_INIT = 0,
		TOUCH_FLAG = 3,					// 터치 튐 방지
		TOUCH_ACTION_COUNT = 5,	// 다른 액션 방지
	};

	TouchCheck	m_touchCheck[2];
	TouchPoint m_firstTouch;
	TouchPoint m_oldPoint;
	TouchGesture m_TG;
	oglFunctions m_OGL;

	DWORD m_drawTick;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAMCOPView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	HDC m_hDC;
	HGLRC m_hRC;
	virtual ~CAMCOPView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CAMCOPView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#ifndef _DEBUG  // debug version in AMCOPView.cpp
inline CAMCOPDoc* CAMCOPView::GetDocument()
   { return (CAMCOPDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AMCOPVIEW_H__E09C2E83_0684_4F3D_99C2_86D8129446B6__INCLUDED_)
