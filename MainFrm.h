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

// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "StockEventView.h"
#include "TracksView.h"
#include "OutputWnd.h"
#include "PropertiesWnd.h"
#include "CalendarBar.h"
#include "TrackEventsListView.h"
#include "Resource.h"
#include "RibbonDateTimeControl.h"

class CMainFrame : public CFrameWndEx
{
	
protected: // create from serialization only
	CMainFrame() noexcept;
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	void InitializeSecondaryViews(CSchedulerDoc* doc);
	void UpdateSecondaryViews(LPARAM lHint);
	void StartDraggingStockEvent(int dragItemIndex, CImageList* dragImageList, CPoint point);

private:
	

protected:  // control bar embedded members
	CMFCRibbonBar     m_wndRibbonBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CMFCRibbonStatusBar  m_wndStatusBar;
	CStockEventView         m_wndStockEventView;
	CTracksView        m_wndTracksView;
	CPropertiesWnd    m_wndProperties;
	CTrackEventsListView m_wndTrackEventsListView;
	int dragItemIndex = -1;
	CImageList* dragImageList = nullptr;
	bool dragging = false;
// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnViewEventView();
	afx_msg void OnUpdateViewEventView(CCmdUI* pCmdUI);
	afx_msg void OnViewTrackView();
	afx_msg void OnUpdateViewTrackView(CCmdUI* pCmdUI);
	
	
	afx_msg void OnViewPropertiesWindow();
	afx_msg void OnUpdateViewPropertiesWindow(CCmdUI* pCmdUI);
	afx_msg void OnOptions();
	afx_msg void OnFilePrint();
	afx_msg void OnFilePrintPreview();
	afx_msg void OnUpdateFilePrintPreview(CCmdUI* pCmdUI);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg LRESULT OnEventObjectSelected(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnTrackObjectSelected(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnDocumentLoaded(WPARAM wparam, LPARAM lparam);
	afx_msg void OnUpdateSetStartTime(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSetStartDate(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSetUTCOffset(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRemoveAllEvents(CCmdUI* pCmdUI);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnStartTimeChange(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnSetUTCOffset();
	afx_msg void OnRemoveAllEvents();
	
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);

};


