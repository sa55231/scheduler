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
void CSchedulerView::UpdateRendererLayout(CSchedulerDoc* pDoc)
{
	_AFX_D2D_STATE* pD2DState = AfxGetD2DState();
	ASSERT(NULL != pD2DState);
	IDWriteFactory* pDirectWriteFactory = pD2DState->GetWriteFactory();
	ASSERT(NULL != pDirectWriteFactory);
	ID2D1Factory* factory = pD2DState->GetDirect2dFactory();
	ASSERT(NULL != factory);

	auto sizeTotal = docRenderer.UpdateLayout(pDoc, GetRenderTarget(), pDirectWriteFactory,factory);
	// The total scroll size has to be multiplied by the DPI scale
	// as the size we calculated earlier is in DIPs
	sizeTotal.width = sizeTotal.width * dpiScaleX;
	sizeTotal.height = sizeTotal.height * dpiScaleY;
	TRACE("ONUPDATE: %f %f\n", sizeTotal.width, sizeTotal.height);
	SetScrollSizes(MM_TEXT, CSize((int)sizeTotal.width, (int)sizeTotal.height));
}
void CSchedulerView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	CScrollView::OnUpdate(pSender, lHint, pHint);
	CSchedulerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc)
	{
		UpdateRendererLayout(pDoc);
		((CMainFrame*)AfxGetMainWnd())->UpdateSecondaryViews(lHint);
	}
}

void CSchedulerView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CSchedulerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc) return;

	lastLButtonDownPoint = point;

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
			AfxGetMainWnd()->PostMessage(WM_EVENT_OBJECT_SELECTED, (WPARAM)event->GetEventId(), (LPARAM)this);
		}
		else
		{
			AfxGetMainWnd()->PostMessage(WM_EVENT_OBJECT_SELECTED, (WPARAM)(-1), (LPARAM)this);
		}
		AfxGetMainWnd()->PostMessage(WM_TRACK_OBJECT_SELECTED, (WPARAM)(track->GetTrackId()), (LPARAM)this);
	}
	else
	{
		AfxGetMainWnd()->PostMessage(WM_TRACK_OBJECT_SELECTED, (WPARAM)(-1), (LPARAM)this);
		AfxGetMainWnd()->PostMessage(WM_EVENT_OBJECT_SELECTED, (WPARAM)(-1), (LPARAM)this);
	}

	RedrawWindow();
	CScrollView::OnLButtonDown(nFlags, point);
}
void CSchedulerView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (dragging)
	{
		ReleaseCapture();

		if(eventDraggingImageList)
		{
			eventDraggingImageList->DragLeave(GetDesktopWindow());
			eventDraggingImageList->EndDrag();
			delete eventDraggingImageList;
			eventDraggingImageList = nullptr;
		}

		CSchedulerDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		if (!pDoc) return;

		CPoint pt(point); //Get current mouse coordinates
		ClientToScreen(&pt); //Convert to screen coordinates
		// Get the CWnd pointer of the window that is under the mouse cursor
		CWnd* pDropWnd = WindowFromPoint(pt);
		ASSERT(pDropWnd); //make sure we have a window

		if (selectedEvent != nullptr && pDropWnd == this && selectedEvent != dropTargetEvent)
		{
			int stockEventIndex = pDoc->GetStockEventIndex(selectedEvent->GetEventId());
			// This is kinda a bug and kinda not
			// When dropping on an event in a different track, it inserts in front of the dropped event
			// When dropping on an event on the same track it inserts it after the dropped event
			// but only sometimes. sometimes it's before (deterministic, it's not magic).
			// But, it has the weird effect of actually being quite intuitive on its behaviour
			// So, at least for now, i'm leaving it in.
			selectedTrack->GetTrack()->RemoveEvent(selectedEvent->GetEvent());
			AddEventAtPoint(stockEventIndex, pt);
		}		
		if (dropTargetTrack != nullptr)
		{
			dropTargetTrack->SetDropTarget(false);
			dropTargetTrack = nullptr;
		}

		if (dropTargetEvent != nullptr)
		{
			dropTargetEvent->SetDropTarget(false);
			dropTargetEvent = nullptr;
		}
		dragging = false;
		RedrawWindow();
	}

	CScrollView::OnLButtonUp(nFlags, point);
}

void CSchedulerView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!dragging)
	{
		auto distance = std::sqrt((lastLButtonDownPoint.x - point.x) * (lastLButtonDownPoint.x - point.x) +
			(lastLButtonDownPoint.y - point.y) * (lastLButtonDownPoint.y - point.y));		
		dragging = (nFlags & MK_LBUTTON) && (selectedEvent != nullptr) && (distance > 20.0);
		if (dragging)
		{
			int nOffset = -10; //offset in pixels for drag image (positive is up and to the left; neg is down and to the right)
			eventDraggingImageList = new CImageList();
			eventDraggingImageList->Create(100, 100, ILC_COLOR4, 1, 0);
			eventDraggingImageList->BeginDrag(0, CPoint(nOffset, nOffset - 4));
			eventDraggingImageList->DragEnter(GetDesktopWindow(), point);
			SetCapture();
		}
	}
	
	if (dragging)
	{
		CPoint pt(point);	//get our current mouse coordinates
		ClientToScreen(&pt); //convert to screen coordinates
		if (eventDraggingImageList)
		{
			eventDraggingImageList->DragMove(pt); //move the drag image to those coordinates
			// Unlock window updates (this allows the dragging image to be shown smoothly)
			eventDraggingImageList->DragShowNolock(false);
		}

		CWnd* pDropWnd = WindowFromPoint(pt);
		ASSERT(pDropWnd); //make sure we have a window

		if (selectedEvent != nullptr && pDropWnd == this)
		{
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			DraggingEventAtPoint(-1, pt);
		}
	}

	CView::OnMouseMove(nFlags, point);
}
void CSchedulerView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point); //Convert to screen coordinates
	OnContextMenu(this, point);
}

void CSchedulerView::DraggingEventAtPoint(int stockEventIndex, CPoint point)
{
	CSchedulerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc) return;
	CRect windowRect;
	GetWindowRect(windowRect);
	ScreenToClient(windowRect);
	ScreenToClient(&point);
	auto pos = GetScrollPosition();
	auto horizontalThreshold = std::max(100, (int)(windowRect.right / 10));
	auto verticalThreshold = std::max(100, (int)(windowRect.bottom / 10));
	if(point.x > windowRect.right - horizontalThreshold)
	{
		SetScrollPos(SB_HORZ, pos.x + 50);
	}
	if (pos.x > 0 && point.x > 0 && point.x < horizontalThreshold)
	{
		SetScrollPos(SB_HORZ, pos.x - 50);
	}
	if (point.y > windowRect.bottom - verticalThreshold)
	{
		SetScrollPos(SB_VERT, pos.y + 50);
	}
	if (pos.y > 0 && point.y > 0 && point.y < verticalThreshold)
	{
		SetScrollPos(SB_VERT, pos.y - 50);
	}

	pos = GetScrollPosition();
	// The scroll position has to be converted to DIPs (device indipendent pixels)
	pos.x += point.x;
	pos.y += point.y;

	auto dipPosition = D2D1::Point2F((float)pos.x / dpiScaleX, (float)pos.y / dpiScaleY);
	if (dropTargetEvent != nullptr)
	{
		dropTargetEvent->SetDropTarget(false);
		dropTargetEvent = nullptr;
	}

	if (dropTargetTrack != nullptr)
	{
		dropTargetTrack->SetDropTarget(false);
		dropTargetTrack = nullptr;
	}

	CTrackRenderer* track = docRenderer.GetTrackAtPoint(dipPosition);
	if (track != nullptr)
	{
		dropTargetTrack = track;
		dropTargetTrack->SetDropTarget(true);
		CEventRenderer* event = track->GetEventAtPoint(dipPosition);
		if (event != nullptr)
		{
			dropTargetTrack->SetDropTarget(false);
			dropTargetEvent = event;
			dropTargetEvent->SetDropTarget(true);
		}		
	}
	RedrawWindow();
}

void CSchedulerView::AddEventAtPoint(int stockEventIndex, CPoint point)
{
	CSchedulerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc) return;
	ScreenToClient(&point);

	auto pos = GetScrollPosition();
	// The scroll position has to be converted to DIPs (device indipendent pixels)
	pos.x += point.x;
	pos.y += point.y;

	auto dipPosition = D2D1::Point2F((float)pos.x / dpiScaleX, (float)pos.y / dpiScaleY);
	CTrackRenderer* track = docRenderer.GetTrackAtPoint(dipPosition);
	if (track != nullptr)
	{
		CEventRenderer* event = track->GetEventAtPoint(dipPosition);
		if (event != nullptr)
		{
			int index = track->GetEventRenderIndex(event);
			pDoc->AddTrackEventAtIndex(stockEventIndex,track->GetName(),index, reinterpret_cast<LPARAM>(this));
		}
		else
		{
			auto trackLabelBounds = track->GetTrackLabelBounds();
			if (point.x >= trackLabelBounds.left && point.x <= trackLabelBounds.right)
			{
				pDoc->AddTrackEventAtIndex(stockEventIndex, track->GetName(), 0, reinterpret_cast<LPARAM>(this));
			}
			else
			{
				pDoc->AddTrackEvent(stockEventIndex, track->GetName(), reinterpret_cast<LPARAM>(this));
			}
		}		
	}
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
