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
	ON_MESSAGE(WM_DPICHANGED, &CSchedulerView::DpiChanged)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CSchedulerView construction/destruction

CSchedulerView::CSchedulerView() noexcept
{
	//factory->GetDpiForWindow(&dpiX, &dpiY);	
}

CSchedulerView::~CSchedulerView()
{
}

BOOL CSchedulerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	return CScrollView::PreCreateWindow(cs);
}
LRESULT CSchedulerView::DpiChanged(WPARAM wParam, LPARAM lParam)
{
	dpi = static_cast<FLOAT>(LOWORD(wParam));
	dpiScale = dpi / 96.f;
	return 0;
}
void CSchedulerView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType,cx,cy);
	docRenderer.Resize(cx, cy);
}
// CSchedulerView drawing

void CSchedulerView::OnDraw(CDC* /*pDC*/)
{
	RECT rc;
	GetClientRect(&rc);
	auto pos = GetScrollPosition();
	// The scroll position has to be converted to DIPs (device indipendent pixels)
	pos.x = (long)((float)pos.x / dpiScale);
	pos.y = (long)((float)pos.y / dpiScale);;
	docRenderer.Render(rc, this->m_hWnd, pos);
}

void CSchedulerView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSchedulerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
}

void CSchedulerView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	CScrollView::OnUpdate(pSender, lHint, pHint);
	CSchedulerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc)
	{
		dpi = static_cast<FLOAT>(GetDpiForWindow(this->m_hWnd));
		dpiScale = dpi / 96.f;
		auto sizeTotal = docRenderer.UpdateLayout(pDoc);
		// The total scroll size has to be multiplied by the DPI scale
		// as the size we calculated earlier is in DIPs
		sizeTotal.width =sizeTotal.width * dpiScale;
		sizeTotal.height = sizeTotal.height * dpiScale;
		TRACE("ONUPDATE: %f %f\n", sizeTotal.width, sizeTotal.height);
		SetScrollSizes(MM_TEXT, CSize(sizeTotal.width,sizeTotal.height));
		//ResizeParentToFit();
	}
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
