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

// schedulerView.h : interface of the CSchedulerView class
//

#pragma once
#include "SchedulerDocumentRenderer.h"

class CSchedulerView : public CScrollView
{
protected: // create from serialization only
	CSchedulerView() noexcept;
	DECLARE_DYNCREATE(CSchedulerView)

// Attributes
public:
	CSchedulerDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
//	LRESULT DpiChanged(WPARAM wParam, LPARAM lParam);
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	afx_msg void OnSize(UINT nType,int cx,int cy);
// Implementation
public:
	virtual ~CSchedulerView();
	void AddEventAtPoint(int stockEventId, CPoint point);
	void DraggingEventAtPoint(int stockEventId, CPoint point);
	void OnRemoveScheduledEvent();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	CBitmap* GetPrintingBitmap() const;
protected:
	void HandleEventSelection(CPoint point);
	void UpdateRendererLayout(CSchedulerDoc* pDoc);
	void CreateEventDraggingImageList();
	struct PrintingMetrics
	{
		int pageSizeX = 0;
		int pageSizeY = 0;
		int ppiX = 0;
		int ppiY = 0;
		float ratioX = 0.f;
		float ratioY = 0.f;
		float totalPrintingWidth = 0.f;
		float totalPrintingHeight = 0.f;
		int pagesCountPerWidth = 0;
		int pagesCountPerHeight = 0;
		int maxPages = 0;
		float dipsPerPageWidth = 0.f;
		float dipsPerPageHeight = 0.f;
		D2D1_SIZE_F bitmapsDpi;
		int textLineHeight = 0;
		int textLinesPerPage = 0;
	};

	PrintingMetrics ComputeDCMetrics(CDC* pDC, D2D1_SIZE_F dpi);
	D2D1_SIZE_F CreatePrintingBitmap();
	D2D1_SIZE_F CalculatePrintingBitmapSize(ID2D1Factory* factory, IWICImagingFactory* wicFactory, IDWriteFactory* pDirectWriteFactory);
private:
	CSchedulerDocumentRenderer docRenderer;
	FLOAT dpiX = 0.f;
	FLOAT dpiY = 0.f;
	FLOAT dpiScaleX = 0.f;
	FLOAT dpiScaleY = 0.f;
	CEventRenderer* selectedEvent = nullptr;
	CTrackRenderer* selectedTrack = nullptr;
	CEventRenderer* dropTargetEvent = nullptr;
	CTrackRenderer* dropTargetTrack = nullptr;
	bool dragging = false;
	CPoint lastLButtonDownPoint = {0,0};
	CImageList* eventDraggingImageList = nullptr;
	CImageList contextMenuImageList;
	CBitmap removeSelectedEventBitmap;
	CBitmap* printingBitmap = nullptr;
	D2D1_SIZE_F renderingDipSize = {0,0};
	PrintingMetrics printingMetrics;
	std::vector<CString> textPrintingLines;
// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnAfxDraw2D(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAfxRecreated2DResources(WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnUpdateCommandRemoveScheduledEvent(CCmdUI* pCmdUI);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg LRESULT RepaintView(WPARAM wParam, LPARAM lParam);
};

#ifndef _DEBUG  // debug version in schedulerView.cpp
inline CSchedulerDoc* CSchedulerView::GetDocument() const
   { return reinterpret_cast<CSchedulerDoc*>(m_pDocument); }
#endif

