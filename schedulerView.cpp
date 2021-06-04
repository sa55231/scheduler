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

#include <chrono>

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
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_REGISTERED_MESSAGE(AFX_WM_DRAW2D, &CSchedulerView::OnAfxDraw2D)
	ON_REGISTERED_MESSAGE(AFX_WM_RECREATED2DRESOURCES, &CSchedulerView::OnAfxRecreated2DResources)
	ON_UPDATE_COMMAND_UI(ID_REMOVE_SCHEDULED_EVENT, OnUpdateCommandRemoveScheduledEvent)
	ON_COMMAND(ID_REMOVE_SCHEDULED_EVENT, OnRemoveScheduledEvent)
	ON_WM_KEYDOWN()
	ON_MESSAGE(WM_REPAINT_VIEW, &CSchedulerView::RepaintView)
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
	CMFCToolBar::AddToolBarForImageCollection(IDR_REMOVE_SCHEDULED_EVENT, IDB_REMOVE_SCHEDULED_EVENT);
	
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
	auto zoomLevel = docRenderer.GetZoomLevel();
	dipScrollPosition = D2D1::Point2F(dipScrollPosition.x / zoomLevel.width, dipScrollPosition.y / zoomLevel.height);
	docRenderer.Render(pRenderTarget, dipScrollPosition);
	return (LRESULT)TRUE;
}
LRESULT CSchedulerView::OnAfxRecreated2DResources(WPARAM wParam, LPARAM lParam)
{
	CHwndRenderTarget* pRenderTarget = (CHwndRenderTarget*)lParam;
	ASSERT_KINDOF(CHwndRenderTarget, pRenderTarget);
	PostMessage(WM_REPAINT_VIEW, 0, 0);
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
	renderingDipSize = sizeTotal;
	// The total scroll size has to be multiplied by the DPI scale
	// as the size we calculated earlier is in DIPs
	sizeTotal.width = sizeTotal.width * dpiScaleX;
	sizeTotal.height = sizeTotal.height * dpiScaleY;
	TRACE("ONUPDATE: %f %f\n", sizeTotal.width, sizeTotal.height);
	SetScrollSizes(MM_TEXT, CSize((int)sizeTotal.width, (int)sizeTotal.height));
}
void CSchedulerView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	selectedEvent = nullptr;
	selectedTrack = nullptr;
	dropTargetEvent = nullptr;
	dropTargetTrack = nullptr;

	CScrollView::OnUpdate(pSender, lHint, pHint);
	CSchedulerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (pDoc)
	{
		UpdateRendererLayout(pDoc);
		((CMainFrame*)AfxGetMainWnd())->UpdateSecondaryViews(lHint);
	}
}

void CSchedulerView::HandleEventSelection(CPoint point)
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
	auto zoomLevel = docRenderer.GetZoomLevel();	
	auto dipPosition = D2D1::Point2F((float)pos.x / dpiScaleX, (float)pos.y / dpiScaleY);
	dipPosition = D2D1::Point2F(dipPosition.x / zoomLevel.width, dipPosition.y / zoomLevel.height);
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
			AfxGetMainWnd()->PostMessage(WM_TRACK_OBJECT_SELECTED, (WPARAM)(track->GetTrackId()), (LPARAM)this);
		}		
	}
	else
	{
		AfxGetMainWnd()->PostMessage(WM_TRACK_OBJECT_SELECTED, (WPARAM)(-1), (LPARAM)this);
		AfxGetMainWnd()->PostMessage(WM_EVENT_OBJECT_SELECTED, (WPARAM)(-1), (LPARAM)this);
	}

	RedrawWindow();
}
void CSchedulerView::OnLButtonDown(UINT nFlags, CPoint point)
{
	HandleEventSelection(point);
	if (DragDetect(point) && !dragging)
	{
		if (selectedEvent != nullptr)
		{
			dragging = true;
			CreateEventDraggingImageList();
			eventDraggingImageList->BeginDrag(0, CPoint(0, 0));
			eventDraggingImageList->DragEnter(GetDesktopWindow(), point);
			SetCapture();
		}
	}
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
			int stockEventId = selectedEvent->GetEvent()->GetStockId();
			// This is kinda a bug and kinda not
			// When dropping on an event in a different track, it inserts in front of the dropped event
			// When dropping on an event on the same track it inserts it after the dropped event
			// but only sometimes. sometimes it's before (deterministic, it's not magic).
			// But, it has the weird effect of actually being quite intuitive on its behaviour
			// So, at least for now, i'm leaving it in.
			selectedTrack->GetTrack()->RemoveEvent(selectedEvent->GetEvent());
			AddEventAtPoint(stockEventId, pt);
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
			DraggingEventAtPoint(-1, pt);
			if (dropTargetEvent == nullptr && dropTargetTrack == nullptr)
			{
				SetCursor(LoadCursor(NULL, IDC_NO));
			}
			else
			{
				SetCursor(LoadCursor(NULL, IDC_ARROW));
			}
		}
		else
		{
			SetCursor(LoadCursor(NULL, IDC_NO));
		}
		if (eventDraggingImageList)
		{
			eventDraggingImageList->DragShowNolock(true);
		}
		
	}

	CView::OnMouseMove(nFlags, point);
}
void CSchedulerView::CreateEventDraggingImageList()
{
	auto eventSize = selectedEvent->GetSize();
	_AFX_D2D_STATE* pD2DState = AfxGetD2DState();
	ASSERT(NULL != pD2DState);
	ID2D1Factory* factory = pD2DState->GetDirect2dFactory();
	ASSERT(NULL != factory);
	IWICImagingFactory* wicFactory = pD2DState->GetWICFactory();
	ASSERT(NULL != wicFactory);
	CComPtr<IWICBitmap> wicBitmap;
	HRESULT hr = wicFactory->CreateBitmap((UINT)eventSize.width, (UINT)eventSize.height,
		GUID_WICPixelFormat32bppPBGRA, WICBitmapCacheOnLoad, &wicBitmap);
	ATLENSURE_SUCCEEDED(hr);

	ID2D1RenderTarget* bitmapRenderTarget;
	D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties();
	rtProps.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);
	rtProps.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
	rtProps.usage = D2D1_RENDER_TARGET_USAGE_NONE;
	hr = factory->CreateWicBitmapRenderTarget(wicBitmap, rtProps, &bitmapRenderTarget);
	ATLENSURE_SUCCEEDED(hr);
	CRenderTarget br;
	br.Attach(bitmapRenderTarget);
	{
		br.BeginDraw();
		br.Clear(D2D1::ColorF(D2D1::ColorF::Black, 1.f));
		CD2DSolidColorBrush bgBrush(&br, D2D1::ColorF(selectedEvent->GetEvent()->GetColor(), 1.f));
		CD2DSolidColorBrush frBrush(&br, docRenderer.GetEventForegroundColor());
		selectedEvent->Render(&br, &bgBrush, &frBrush, nullptr);
		br.EndDraw();
	}
	
	CComPtr<IWICComponentInfo> componentInfo;
	CComPtr <IWICPixelFormatInfo> pixelFormatInfo;
	hr = wicFactory->CreateComponentInfo(GUID_WICPixelFormat32bppPBGRA, &componentInfo);
	ATLENSURE_SUCCEEDED(hr);
	hr = componentInfo->QueryInterface<IWICPixelFormatInfo>(&pixelFormatInfo);
	ATLENSURE_SUCCEEDED(hr);
	UINT bitsPerPixel = 0;
	hr = pixelFormatInfo->GetBitsPerPixel(&bitsPerPixel);
	ATLENSURE_SUCCEEDED(hr);
	UINT bytesPerPixel = bitsPerPixel / 8;
	UINT stride = (UINT)eventSize.width * bytesPerPixel;
	UINT bufSize = stride * (UINT)eventSize.height;
	std::unique_ptr<BYTE[]> buf = std::make_unique<BYTE[]>(bufSize);
	hr = wicBitmap->CopyPixels(nullptr, stride, bufSize, buf.get());
	ATLENSURE_SUCCEEDED(hr);
	CBitmap bmp;
	bmp.CreateBitmap((int)eventSize.width, (int)eventSize.height, 1, bitsPerPixel, buf.get());
	eventDraggingImageList = new CImageList();
	eventDraggingImageList->Create((int)eventSize.width, (int)eventSize.height, ILC_COLOR32, 1, 1);
	eventDraggingImageList->Add(&bmp, RGB(0, 0, 0));
}
void CSchedulerView::OnRButtonDown(UINT nFlags, CPoint point)
{
	HandleEventSelection(point);
	CScrollView::OnRButtonDown(nFlags, point);
}
void CSchedulerView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point); //Convert to screen coordinates
	OnContextMenu(this, point);
}

void CSchedulerView::DraggingEventAtPoint(int stockEventId, CPoint point)
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
	auto zoomLevel = docRenderer.GetZoomLevel();
	dipPosition = D2D1::Point2F(dipPosition.x / zoomLevel.width, dipPosition.y / zoomLevel.height);
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

void CSchedulerView::AddEventAtPoint(int stockEventId, CPoint point)
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
	auto zoomLevel = docRenderer.GetZoomLevel();
	dipPosition = D2D1::Point2F(dipPosition.x / zoomLevel.width, dipPosition.y / zoomLevel.height);
	CTrackRenderer* track = docRenderer.GetTrackAtPoint(dipPosition);
	if (track != nullptr)
	{
		CEventRenderer* event = track->GetEventAtPoint(dipPosition);
		if (event != nullptr)
		{
			int index = track->GetEventRenderIndex(event);
			pDoc->AddTrackEventAtIndex(stockEventId,track->GetTrack(),index, reinterpret_cast<LPARAM>(this));
		}
		else
		{
			auto trackLabelBounds = track->GetTrackLabelBounds();
			if (point.x >= trackLabelBounds.left && point.x <= trackLabelBounds.right)
			{
				pDoc->AddTrackEventAtIndex(stockEventId, track->GetTrack(), 0, reinterpret_cast<LPARAM>(this));
			}
			else
			{
				pDoc->AddTrackEvent(stockEventId, track->GetTrack(), reinterpret_cast<LPARAM>(this));
			}
		}
	}

	HandleEventSelection(point);
}


void CSchedulerView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_REMOVE_SCHEDULED_EVENT, point.x, point.y, this, TRUE);
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
	//Lets not make a guess here on the number of pages
	//if the user changes the printer, then we get more or fewer pages
	//and then we will have to implement OnPrepareDC to actually stop at the 
	//actuall number of pages

	// TODO: implement this one day

	/*CPrintDialog pdlg(FALSE);
	if (pdlg.GetDefaults())
	{
		CDC t_pDC;
		t_pDC.Attach(pdlg.GetPrinterDC());
		auto metrics = ComputeDCMetrics(&t_pDC, D2D1::SizeF(dpiX,dpiY));
		pInfo->SetMaxPage(metrics.maxPages);
		pInfo->m_nCurPage = 1;  // start printing at page# 1
	}*/

	return DoPreparePrinting(pInfo);
}

void CSchedulerView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	textPrintingLines.clear();
	auto printScheduleType = theApp.GetProfileInt(_T("Settings"), _T("PrintType"), CSchedulerApp::PrintTypeGraphical);
	auto printTrackPerPage = theApp.GetProfileInt(_T("Settings"), _T("PrintTrackPerPage"), 0);

	if (printScheduleType == CSchedulerApp::PrintTypeGraphical)
	{
		auto rendersTargetDpi = CreatePrintingBitmap();
		printingMetrics = ComputeDCMetrics(pDC, rendersTargetDpi);
		int nMaxPage = printingMetrics.maxPages;// m_nMaxRowCount / m_nRowsPerPage + 1;//Compute this again in case user changed printer
		pInfo->SetMaxPage(nMaxPage);
	}
	else
	{
		//pDC->SetMapMode(MM_TEXT);
		printingMetrics = ComputeDCMetrics(pDC, D2D1::SizeF((float)GetDC()->GetDeviceCaps(LOGPIXELSX), (float)GetDC()->GetDeviceCaps(LOGPIXELSY)));
		TEXTMETRIC textMetrics = { 0 };
		pDC->GetTextMetrics(&textMetrics);
		printingMetrics.textLineHeight = textMetrics.tmHeight;
		printingMetrics.textLinesPerPage = printingMetrics.pageSizeY / printingMetrics.textLineHeight;

		for (const auto& track : GetDocument()->GetTracks())
		{
			textPrintingLines.emplace_back(track->GetName());
			auto startTime = track->GetStartTime();
			std::chrono::minutes utcOffset(GetDocument()->GetUTCOffsetMinutes());
			std::chrono::local_seconds start{ std::chrono::duration_cast<std::chrono::seconds>(startTime.time_since_epoch()) - utcOffset };

			for (const auto& ev : track->GetEvents())
			{
				CString startText(std::format("{:%Y %b %d %R}", start).c_str());
				auto end = start + ev->GetDuration();
				CString endText(std::format("{:%Y %b %d %R}", end).c_str());
				start = end;
				CString text;
				text.Append(ev->GetName());
				text.Append(_T("        "));
				text.Append(startText);
				text.Append(_T("        "));
				text.Append(endText);
				textPrintingLines.emplace_back(text);
			}
			if (printTrackPerPage == 1)
			{	
				int linesLeftPerLastPage = track->GetEvents().size() % printingMetrics.textLinesPerPage;
				for (auto i = linesLeftPerLastPage; i < printingMetrics.textLinesPerPage-1; ++i)
				{
					textPrintingLines.emplace_back(_T("        "));
				}
			}
			else
			{
				textPrintingLines.emplace_back(_T("        "));
			}
		}
		
		printingMetrics.maxPages = (int)std::ceil((double)textPrintingLines.size() / (double)printingMetrics.textLinesPerPage);
		pInfo->SetMaxPage(printingMetrics.maxPages);
	}
	pInfo->m_nCurPage = 1;  // start printing at page# 1
}

D2D1_SIZE_F CSchedulerView::CalculatePrintingBitmapSize(ID2D1Factory* factory, IWICImagingFactory* wicFactory, IDWriteFactory* pDirectWriteFactory)
{
	CComPtr<IWICBitmap> wicBitmap;
	HRESULT hr = wicFactory->CreateBitmap((UINT)renderingDipSize.width, (UINT)renderingDipSize.height,
		GUID_WICPixelFormat32bppPBGRA, WICBitmapCacheOnLoad, &wicBitmap);
	ATLENSURE_SUCCEEDED(hr);

	ID2D1RenderTarget* bitmapRenderTarget;
	D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties();
	rtProps.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);
	rtProps.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
	rtProps.usage = D2D1_RENDER_TARGET_USAGE_NONE;
	hr = factory->CreateWicBitmapRenderTarget(wicBitmap, rtProps, &bitmapRenderTarget);
	ATLENSURE_SUCCEEDED(hr);
	CRenderTarget br;
	br.Attach(bitmapRenderTarget);
	CSchedulerDocumentRenderer renderer;
	return renderer.UpdateLayout(GetDocument(), &br, pDirectWriteFactory, factory);
}
D2D1_SIZE_F CSchedulerView::CreatePrintingBitmap()
{
	_AFX_D2D_STATE* pD2DState = AfxGetD2DState();
	ASSERT(NULL != pD2DState);
	ID2D1Factory* factory = pD2DState->GetDirect2dFactory();
	ASSERT(NULL != factory);
	IWICImagingFactory* wicFactory = pD2DState->GetWICFactory();
	ASSERT(NULL != wicFactory);
	IDWriteFactory* pDirectWriteFactory = pD2DState->GetWriteFactory();
	ASSERT(NULL != pDirectWriteFactory);

	auto bitmapSize = CalculatePrintingBitmapSize(factory,wicFactory,pDirectWriteFactory);

	CComPtr<IWICBitmap> wicBitmap;
	HRESULT hr = wicFactory->CreateBitmap((UINT)bitmapSize.width, (UINT)bitmapSize.height,
		GUID_WICPixelFormat32bppPBGRA, WICBitmapCacheOnLoad, &wicBitmap);
	ATLENSURE_SUCCEEDED(hr);

	ID2D1RenderTarget* bitmapRenderTarget;
	D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties();
	rtProps.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);
	rtProps.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
	rtProps.usage = D2D1_RENDER_TARGET_USAGE_NONE;
	hr = factory->CreateWicBitmapRenderTarget(wicBitmap, rtProps, &bitmapRenderTarget);
	ATLENSURE_SUCCEEDED(hr);
	CRenderTarget br;
	br.Attach(bitmapRenderTarget);
	CSchedulerDocumentRenderer renderer;
	renderer.UpdateLayout(GetDocument(), &br, pDirectWriteFactory, factory);
	{
		br.BeginDraw();
		renderer.Render(&br, D2D1::Point2F());
		br.EndDraw();
	}
	
	CComPtr<IWICComponentInfo> componentInfo;
	CComPtr <IWICPixelFormatInfo> pixelFormatInfo;
	hr = wicFactory->CreateComponentInfo(GUID_WICPixelFormat32bppPBGRA, &componentInfo);
	ATLENSURE_SUCCEEDED(hr);
	hr = componentInfo->QueryInterface<IWICPixelFormatInfo>(&pixelFormatInfo);
	ATLENSURE_SUCCEEDED(hr);
	UINT bitsPerPixel = 0;
	hr = pixelFormatInfo->GetBitsPerPixel(&bitsPerPixel);
	ATLENSURE_SUCCEEDED(hr);
	UINT bytesPerPixel = bitsPerPixel / 8;
	UINT stride = (UINT)bitmapSize.width * bytesPerPixel;
	UINT bufSize = stride * (UINT)bitmapSize.height;
	std::unique_ptr<BYTE[]> buf = std::make_unique<BYTE[]>(bufSize);
	hr = wicBitmap->CopyPixels(nullptr, stride, bufSize, buf.get());
	ATLENSURE_SUCCEEDED(hr);
	printingBitmap = new CBitmap();
	printingBitmap->CreateBitmap((int)bitmapSize.width, (int)bitmapSize.height, 1, bitsPerPixel, buf.get());
	return br.GetDpi();
}

CSchedulerView::PrintingMetrics CSchedulerView::ComputeDCMetrics(CDC* pDC, D2D1_SIZE_F dpi)
{
	PrintingMetrics metrics;
	metrics.bitmapsDpi = dpi;
	metrics.pageSizeX = pDC->GetDeviceCaps(HORZRES);
	metrics.pageSizeY = pDC->GetDeviceCaps(VERTRES);
	metrics.ppiX = pDC->GetDeviceCaps(LOGPIXELSX);
	metrics.ppiY = pDC->GetDeviceCaps(LOGPIXELSY);

	metrics.ratioX = (FLOAT)metrics.ppiX / dpi.width;
	metrics.ratioY = (FLOAT)metrics.ppiY / dpi.height;
	
	
	// total printing size in printing units 
	metrics.totalPrintingWidth = renderingDipSize.width * metrics.ratioX;
	metrics.totalPrintingHeight = renderingDipSize.height * metrics.ratioY;

	metrics.pagesCountPerWidth = (int)std::ceilf(metrics.totalPrintingWidth / (float)metrics.pageSizeX);
	metrics.pagesCountPerHeight = (int)std::ceilf(metrics.totalPrintingHeight / (float)metrics.pageSizeY);
	metrics.maxPages = metrics.pagesCountPerWidth * metrics.pagesCountPerHeight;
	metrics.dipsPerPageWidth = metrics.pageSizeX / metrics.ratioX;
	metrics.dipsPerPageHeight = metrics.pageSizeY / metrics.ratioY;

	return metrics;
}

void CSchedulerView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{	
	auto printScheduleType = theApp.GetProfileInt(_T("Settings"), _T("PrintType"), CSchedulerApp::PrintTypeGraphical);
	auto printTrackPerPage = theApp.GetProfileInt(_T("Settings"), _T("PrintTrackPerPage"), 0);

	if (printScheduleType == CSchedulerApp::PrintTypeGraphical)
	{
		CDC memDC;
		memDC.CreateCompatibleDC(pDC);
		CBitmap* pOldBit = memDC.SelectObject(printingBitmap);

		auto curPage = pInfo->m_nCurPage - 1;

		int bitmapX = (curPage % printingMetrics.pagesCountPerWidth) * (int)printingMetrics.dipsPerPageWidth;
		int bitmapY = (curPage / printingMetrics.pagesCountPerWidth) * (int)printingMetrics.dipsPerPageHeight;

		pDC->StretchBlt(0, 0, std::min(pInfo->m_rectDraw.right, (long)printingMetrics.totalPrintingWidth),
			std::min(pInfo->m_rectDraw.bottom, (long)printingMetrics.totalPrintingHeight), &memDC,
			bitmapX, bitmapY, (int)printingMetrics.dipsPerPageWidth, (int)printingMetrics.dipsPerPageHeight,
			SRCCOPY);

		memDC.SelectObject(pOldBit);
		memDC.DeleteDC();
	}
	else
	{
		auto curPage = pInfo->m_nCurPage - 1;
		int startLine = printingMetrics.textLinesPerPage * curPage;
		int endLine = std::min((int)textPrintingLines.size(), startLine + printingMetrics.textLinesPerPage);
		int top = 0;
		for (int i = startLine; i < endLine; ++i)
		{
			RECT r = {0,top,printingMetrics.pageSizeX,top + printingMetrics.textLineHeight};
			pDC->DrawText(textPrintingLines.at(i), &r, DT_LEFT | DT_NOCLIP | DT_SINGLELINE| DT_NOPREFIX| DT_VCENTER);
			top += printingMetrics.textLineHeight;
		}
	}
}
CBitmap* CSchedulerView::GetPrintingBitmap() const
{
	return printingBitmap;
}
void CSchedulerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	delete printingBitmap;
	printingBitmap = nullptr;
	textPrintingLines.clear();
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
void CSchedulerView::OnUpdateCommandRemoveScheduledEvent(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(selectedEvent != nullptr);
}
void CSchedulerView::OnRemoveScheduledEvent()
{
	if (selectedEvent == nullptr) return;
	CSchedulerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc) return;

	AfxGetMainWnd()->PostMessage(WM_TRACK_OBJECT_SELECTED, (WPARAM)(-1), (LPARAM)this);
	AfxGetMainWnd()->PostMessage(WM_EVENT_OBJECT_SELECTED, (WPARAM)(-1), (LPARAM)this);

	pDoc->RemoveEventFromTrack(selectedTrack->GetTrack(), selectedEvent->GetEvent());	
}

void CSchedulerView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case VK_DELETE:
		OnRemoveScheduledEvent();
		break;
	default:
		CView::OnKeyDown(nChar, nRepCnt, nFlags);
	}
}
LRESULT CSchedulerView::RepaintView(WPARAM wParam, LPARAM lParam)
{
	Invalidate();
	return (LRESULT)TRUE;
}