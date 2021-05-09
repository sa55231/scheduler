// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface
// (the "Fluent UI") and is provided only as referential material to supplement the
// Microsoft Foundation Classes Reference and related electronic documentation
// included with the MFC C++ library software.
// License terms to copy, use or distribute the Fluent UI are available separately.
// To learn more about our Fluent UI licensing program, please visit
// https://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// schedulerView.cpp : implementation of the CSchedulerView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "scheduler.h"
#endif

#include "schedulerDoc.h"
#include "schedulerView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSchedulerView

IMPLEMENT_DYNCREATE(CSchedulerView, CScrollView)

BEGIN_MESSAGE_MAP(CSchedulerView, CScrollView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CSchedulerView::OnFilePrintPreview)
//	ON_MESSAGE(WM_DPICHANGED, &CSchedulerView::DpiChanged)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_REGISTERED_MESSAGE(AFX_WM_DRAW2D, &CSchedulerView::OnAfxDraw2D)
	ON_REGISTERED_MESSAGE(AFX_WM_RECREATED2DRESOURCES, &CSchedulerView::OnAfxRecreated2DResources)
END_MESSAGE_MAP()

// CSchedulerView construction/destruction

CSchedulerView::CSchedulerView() noexcept
{	
	SetScrollSizes(MM_TEXT, CSize(0, 0));
}

CSchedulerView::~CSchedulerView()
{
}

int CSchedulerView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;

	EnableD2DSupport();
	dpiX = m_pRenderTarget->GetDpi().width;
	dpiY = m_pRenderTarget->GetDpi().height;
	dpiScaleX = dpiX / 96.f;
	dpiScaleY = dpiY / 96.f;
	SetScrollSizes(MM_TEXT, CSize(0, 0));
	return 0;
}

BOOL CSchedulerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	return CScrollView::PreCreateWindow(cs);
}

void CSchedulerView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType,cx,cy);
	docRenderer.Resize(cx, cy);
}
// CSchedulerView drawing

LRESULT CSchedulerView::OnAfxDraw2D(WPARAM wParam, LPARAM lParam)
{
	CHwndRenderTarget* pRenderTarget = (CHwndRenderTarget*)lParam;
	ASSERT_KINDOF(CHwndRenderTarget, pRenderTarget);
	auto pos = GetScrollPosition();
	// The scroll position has to be converted to DIPs (device indipendent pixels)
	auto dipScrollPosition = D2D1::Point2F((float)pos.x / dpiScaleX, (float)pos.y / dpiScaleY);
	docRenderer.Render(pRenderTarget, dipScrollPosition);
	return (LRESULT)TRUE;
}
LRESULT CSchedulerView::OnAfxRecreated2DResources(WPARAM wParam, LPARAM lParam)
{
	CHwndRenderTarget* pRenderTarget = (CHwndRenderTarget*)lParam;
	ASSERT_KINDOF(CHwndRenderTarget, pRenderTarget);
	return (LRESULT)TRUE;
}

void CSchedulerView::OnDraw(CDC* /*pDC*/)
{
}

void CSchedulerView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSchedulerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	((CMainFrame*)AfxGetMainWnd())->InitializeSecondaryViews(pDoc);
}

void CSchedulerView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	CScrollView::OnUpdate(pSender, lHint, pHint);
	CSchedulerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc)
	{
		_AFX_D2D_STATE* pD2DState = AfxGetD2DState();
		ASSERT(NULL != pD2DState);
		IDWriteFactory* pDirectWriteFactory = pD2DState->GetWriteFactory();
		ASSERT(NULL != pDirectWriteFactory);
		
		auto sizeTotal = docRenderer.UpdateLayout(pDoc, GetRenderTarget(),pDirectWriteFactory, 
			std::chrono::system_clock::now(), std::chrono::hours(-4));
		// The total scroll size has to be multiplied by the DPI scale
		// as the size we calculated earlier is in DIPs
		sizeTotal.width = sizeTotal.width * dpiScaleX;
		sizeTotal.height = sizeTotal.height * dpiScaleY;
		TRACE("ONUPDATE: %f %f\n", sizeTotal.width, sizeTotal.height);
		SetScrollSizes(MM_TEXT, CSize((int)sizeTotal.width, (int)sizeTotal.height));
		((CMainFrame*)AfxGetMainWnd())->UpdateSecondaryViews(lHint);
	}
}

void CSchedulerView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CSchedulerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc) return;

	if (selectedEvent != nullptr)
	{
		selectedEvent->SetSelected(false);
		selectedEvent = nullptr;
	}

	if (selectedTrack != nullptr)
	{
		selectedTrack->SetSelected(false);
		selectedTrack = nullptr;
	}

	auto pos = GetScrollPosition();
	// The scroll position has to be converted to DIPs (device indipendent pixels)
	pos.x += point.x;
	pos.y += point.y;

	auto dipPosition = D2D1::Point2F((float)pos.x / dpiScaleX, (float)pos.y / dpiScaleY);
	CTrackRenderer* track = docRenderer.GetTrackAtPoint(dipPosition);
	if (track != nullptr)
	{
		track->SetSelected(true);
		selectedTrack = track;

		CEventRenderer* event = track->GetEventAtPoint(dipPosition);
		if (event != nullptr)
		{
			event->SetSelected(true);
			selectedEvent = event;
			
			track->SetSelected(false);
			selectedTrack = nullptr;

			AfxGetMainWnd()->PostMessage(WM_EVENT_OBJECT_SELECTED, 0, (LPARAM)event);
		}

	}

	RedrawWindow();
	CScrollView::OnLButtonDown(nFlags, point);
}
void CSchedulerView::OnLButtonUp(UINT nFlags, CPoint point)
{
	CScrollView::OnLButtonUp(nFlags, point);
}

void CSchedulerView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CSchedulerView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}

// CSchedulerView printing
void CSchedulerView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CSchedulerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CSchedulerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CSchedulerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

// CSchedulerView diagnostics

#ifdef _DEBUG
void CSchedulerView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CSchedulerView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CSchedulerDoc* CSchedulerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSchedulerDoc)));
	return (CSchedulerDoc*)m_pDocument;
}
#endif //_DEBUG


// CSchedulerView message handlers
