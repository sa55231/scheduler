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
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	afx_msg void OnSize(UINT nType,int cx,int cy);
// Implementation
public:
	virtual ~CSchedulerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

private:
	CSchedulerDocumentRenderer docRenderer;
	FLOAT dpiX = 0.f;
	FLOAT dpiY = 0.f;
	FLOAT dpiScaleX = 0.f;
	FLOAT dpiScaleY = 0.f;
	CEventRenderer* selectedEvent = nullptr;
	CTrackRenderer* selectedTrack = nullptr;
// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnAfxDraw2D(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAfxRecreated2DResources(WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

};

#ifndef _DEBUG  // debug version in schedulerView.cpp
inline CSchedulerDoc* CSchedulerView::GetDocument() const
   { return reinterpret_cast<CSchedulerDoc*>(m_pDocument); }
#endif

