// AMCOPView.cpp : implementation of the CAMCOPView class
//

#include "stdafx.h"
#include "AMCOP.h"

#include "AMCOPDoc.h"
#include "AMCOPView.h"

#include <math.h>
#include <stdio.h>
#include <time.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define FRAME		60
#define FRAMELIMIT (1000 / FRAME)
/////////////////////////////////////////////////////////////////////////////
// CAMCOPView

IMPLEMENT_DYNCREATE(CAMCOPView, CView)

BEGIN_MESSAGE_MAP(CAMCOPView, CView)
	//{{AFX_MSG_MAP(CAMCOPView)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAMCOPView construction/destruction

CAMCOPView::CAMCOPView()
{
	// TODO: add construction code here

}

CAMCOPView::~CAMCOPView()
{
}

BOOL CAMCOPView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CAMCOPView drawing

void CAMCOPView::OnDraw(CDC* pDC)
{
	CAMCOPDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
	wglMakeCurrent(m_hDC, m_hRC);

	// check old touch
	m_TG.flag += m_TG.flag < TOUCH_FLAG ? 1 : 0;

	DWORD getCurTick = GetTickCount();
	if(getCurTick - m_drawTick < FRAMELIMIT)	// Limit Frame.
		return;

	m_OGL.m_VC->calcSmoothView();
	m_OGL.draw3DWorld();
	m_OGL.draw2DInfo();

	SwapBuffers(m_hDC);

	m_drawTick = getCurTick;

	wglMakeCurrent(m_hDC, NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CAMCOPView printing

BOOL CAMCOPView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CAMCOPView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CAMCOPView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CAMCOPView diagnostics

#ifdef _DEBUG
void CAMCOPView::AssertValid() const
{
	CView::AssertValid();
}

void CAMCOPView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CAMCOPDoc* CAMCOPView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAMCOPDoc)));
	return (CAMCOPDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAMCOPView message handlers

void CAMCOPView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	VERIFY(wglMakeCurrent(m_hDC, m_hRC));

	int retVal = m_OGL.initWorld(cx, cy);

	VERIFY(wglMakeCurrent(NULL, NULL));	
}

int CAMCOPView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	/////
	int nPixelFormat;
	m_hDC = ::GetDC(m_hWnd);

	static PIXELFORMATDESCRIPTOR pfd = 
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		24,
		0,0,0,0,0,0,
		0,0,
		0,0,0,0,0,
		32,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0,0,0
	};

	nPixelFormat = ChoosePixelFormat(m_hDC, &pfd);
	VERIFY(SetPixelFormat(m_hDC, nPixelFormat, &pfd));
	m_hRC = wglCreateContext(m_hDC);
	VERIFY(wglMakeCurrent(m_hDC, m_hRC));
	wglMakeCurrent(NULL, NULL);

	SetTimer(1, 10, NULL);

	return 0;
}

void CAMCOPView::OnDestroy()
{
	wglDeleteContext(m_hRC);
	::ReleaseDC(m_hWnd, m_hDC);

	KillTimer(1);

	CView::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

void CAMCOPView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CView::OnLButtonDown(nFlags, point);

	m_TG.action = 1;
	m_TG.click = 1;

	m_touchCheck[0].time = GetTickCount();
	m_touchCheck[0].point.x = point.x;
	m_touchCheck[0].point.y = point.y;
}

void CAMCOPView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CView::OnLButtonUp(nFlags, point);

	if(m_TG.click == 1) {
		wglMakeCurrent(m_hDC, m_hRC);
		m_OGL.drawPicking3DWorld();
		m_OGL.drawPicking2DInfo();

		GLfloat mx = 0, my = 0;
		unsigned int objID = 0;
		int retval = m_OGL.getObjInfo(point.x, point.y, objID, mx, my);
		if(retval > 0) {
//			SwapBuffers(m_hDC);	// for click debug.
			Invalidate(FALSE);
// 			char temp[100] = {0, };
// 			sprintf(temp, "[%d][%d], %f, %f", retval, objID, mx/MAP_EXPANSION, my/MAP_EXPANSION);
// 			AfxMessageBox(temp, MB_OK|MB_ICONEXCLAMATION);
		}
		wglMakeCurrent(m_hDC, NULL);
	} else {
		m_touchCheck[1].time = GetTickCount();
		m_touchCheck[1].point.x = point.x;
		m_touchCheck[1].point.y = point.y;

		int touchMoveXVal = m_touchCheck[1].point.x - m_touchCheck[0].point.x;
		int touchMoveYVal = m_touchCheck[1].point.y - m_touchCheck[0].point.y;
		float touchMoveDist = sqrt((float)(pow((double)touchMoveXVal, 2) + pow((double)touchMoveYVal, 2)));

		if( ((float)(m_touchCheck[1].time - m_touchCheck[0].time)) / 1000 < touchMoveDist / MAP_INERTIA_UNIT) {
			if(0.0f < touchMoveDist) {
				float moveDegree = m_OGL.m_VC->getTanDegree((float)touchMoveXVal, (float)touchMoveYVal);
				m_OGL.m_VC->changeViewCenter(moveDegree, touchMoveDist * MAP_INERTIA_VALUE * (m_OGL.m_VC->getEyeDistance3D(CURRENT_VIEW) / INIT_ZOOM_VAL));	
			}
		}
	}

	m_TG.action = 0;
}

void CAMCOPView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CView::OnRButtonDown(nFlags, point);

	m_TG.action = 3;
}

void CAMCOPView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CView::OnRButtonUp(nFlags, point);

	m_TG.action = 0;
}

void CAMCOPView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CView::OnMouseMove(nFlags, point);

	m_TG.click = 0;

	if(m_TG.flag < TOUCH_FLAG) {
		
		switch(m_TG.action) {
			case 1:	// CENTER MOVE
				{
					float moveXVal = (float)(point.x - m_oldPoint.x);
					float moveYVal = (float)(point.y - m_oldPoint.y);

					float moveDist = sqrt((float)(pow((double)moveXVal, 2) + pow((double)moveYVal, 2)));

					if(0.0f < moveDist) {
						float moveDegree = m_OGL.m_VC->getTanDegree((float)moveXVal, (float)moveYVal);
						m_OGL.m_VC->changeViewCenter(moveDegree, moveDist * (m_OGL.m_VC->getEyeDistance3D(CURRENT_VIEW) / (float)INIT_ZOOM_VAL));	// map 올리면 조절 필요
						Invalidate(FALSE);
					}
				}
				break;
			case 2:	// ZOOM
				{
					// mouse wheel..
				}
				break;
			case 3:	// EYE MOVE
				{
					float moveXVal = point.x - m_oldPoint.x;
					float moveYVal = -(point.y - m_oldPoint.y);

					m_OGL.m_VC->setEyeTurnAngle(CHANGE_VIEW, moveXVal/2.0f);
					m_OGL.m_VC->setEyeTiltAngle(CHANGE_VIEW, moveYVal/2.0f);
					Invalidate(FALSE);
				}
				break;
			default:
				break;
		}
	}

	m_oldPoint.x = point.x;
	m_oldPoint.y = point.y;
	m_TG.flag = TOUCH_INIT;
	
}

BOOL CAMCOPView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	float zoomVal = zDelta / 10;
	m_OGL.m_VC->setEyeDistance(CHANGE_VIEW, zoomVal * m_OGL.m_VC->getEyeDistance3D(CURRENT_VIEW)/INIT_ZOOM_VAL*10.0f);
	Invalidate(FALSE);

	//return CView::OnMouseWheel(nFlags, zDelta, pt);
	return FALSE;
}

void CAMCOPView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	Invalidate(FALSE);

	CView::OnTimer(nIDEvent);
}

void CAMCOPView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	switch(nChar)
	{
	case VK_LEFT:
		m_OGL.m_VC->changeViewCenter(-90, 30 * (m_OGL.m_VC->getEyeDistance3D(CURRENT_VIEW) / INIT_ZOOM_VAL));
		Invalidate(FALSE);
		break;
	case VK_RIGHT:
		m_OGL.m_VC->changeViewCenter(90, 30 * (m_OGL.m_VC->getEyeDistance3D(CURRENT_VIEW) / INIT_ZOOM_VAL));
		Invalidate(FALSE);
		break;
	case VK_UP:
		m_OGL.m_VC->changeViewCenter(0, 30 * (m_OGL.m_VC->getEyeDistance3D(CURRENT_VIEW) / INIT_ZOOM_VAL));
		Invalidate(FALSE);
		break;
	case VK_DOWN:
		m_OGL.m_VC->changeViewCenter(180, 30 * (m_OGL.m_VC->getEyeDistance3D(CURRENT_VIEW) / INIT_ZOOM_VAL));
		Invalidate(FALSE);
		break;

	case VK_OEM_PLUS:
		m_OGL.m_VC->setEyeDistance(CHANGE_VIEW, -10 * m_OGL.m_VC->getEyeDistance3D(CURRENT_VIEW)/INIT_ZOOM_VAL*50.0f);
		Invalidate(FALSE);
		break;
	case VK_OEM_MINUS:
		m_OGL.m_VC->setEyeDistance(CHANGE_VIEW, +10 * m_OGL.m_VC->getEyeDistance3D(CURRENT_VIEW)/INIT_ZOOM_VAL*50.0f);
		Invalidate(FALSE);
		break;

	case 'A':
		m_OGL.m_VC->setEyeTurnAngle(CHANGE_VIEW, -1);
		Invalidate(FALSE);
		break;
	case 'D':
		m_OGL.m_VC->setEyeTurnAngle(CHANGE_VIEW, 1);
		Invalidate(FALSE);
		break;
	case 'W':
		m_OGL.m_VC->setEyeTiltAngle(CHANGE_VIEW, 1);
		Invalidate(FALSE);
		break;
	case 'S':
		m_OGL.m_VC->setEyeTiltAngle(CHANGE_VIEW, -1);
		Invalidate(FALSE);
		break;

	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
