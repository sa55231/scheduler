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

#pragma once

#include "ViewDockingPane.h"

class CFileViewToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CStockEventsListCtrl : public CMFCListCtrl
{
	DECLARE_DYNAMIC(CStockEventsListCtrl)
public:
	virtual COLORREF OnGetCellBkColor(int nRow, int nColumn) override;
	virtual int OnCompareItems(LPARAM lParam1,LPARAM lParam2,int iColumn) override;
};

class CStockEventView : public CViewDockingPane
{
	DECLARE_DYNAMIC(CStockEventView)
	// Construction
public:
	CStockEventView() noexcept;

	void AdjustLayout();
	void OnChangeVisualStyle();

	// Attributes
protected:

	CStockEventsListCtrl m_wndEventListView;
	CImageList m_EventListImages;
	CFileViewToolBar m_wndToolBar;
	int addingItemIndex = -1;
	bool updatingEventsSelection = false;
	bool addingEvent = false;
protected:
	virtual void OnInitialUpdate();
	virtual void OnUpdate(const LPARAM lHint);

private:
	void ReloadEventsList(CSchedulerDoc* pDoc);
	CString FormatDuration(std::chrono::seconds seconds);
	
// Implementation
public:
	virtual ~CStockEventView();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnAddEvent();
	afx_msg void OnRemoveEvent();
	afx_msg void OnEditEvent();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginDragEvent(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateCommandToolbarButtons(CCmdUI* pCmdUI);
	afx_msg void OnEventListItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnEventObjectSelected(WPARAM wParam, LPARAM lParam);
	//afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()
};

