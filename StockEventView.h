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
#include "utils.h"
#include <map>

class CFileViewToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }

	virtual void AdjustLocations();
};

class CStockEventsListCtrl : public CMFCListCtrl
{
	DECLARE_DYNAMIC(CStockEventsListCtrl)
public:
	virtual COLORREF OnGetCellBkColor(int nRow, int nColumn) override;
	virtual int OnCompareItems(LPARAM lParam1,LPARAM lParam2,int iColumn) override;
	BOOL SetCellText(int index, EventsViewColumns column,LPCTSTR text);
	int GetColWidth(EventsViewColumns col) const;
	void SetColWidth(EventsViewColumns col, int width);
	bool IsColumnVisible(EventsViewColumns col) const;
	void SetColumnVisible(EventsViewColumns col, bool flag);
	void AddColumn(EventsViewColumns col, LPCTSTR caption, int format);
private:
	struct Column
	{
		Column() {}
		Column(EventsViewColumns column, int colIndex, bool visible, 
			int colWidth,const CString& caption, int format):
			column(column),colIndex(colIndex),visible(visible),
			colWidth(colWidth),caption(caption),format(format)
		{}
		EventsViewColumns column = EventsViewColumns::Name;
		int colIndex = 0;
		bool visible = true;
		int colWidth = 90;
		CString caption;
		int format = 0;
	};
	std::map<EventsViewColumns, Column> columnMappings;
	int addingColumnIndex = 0;
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
	void LoadListSettings();
	void SaveListSettings();
// Implementation
public:
	virtual ~CStockEventView();

protected:
	afx_msg void OnDestroy();
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
	afx_msg void OnUpdateShowColumnsButton(CCmdUI* pCmdUI);	
	afx_msg void OnEventListItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnEventObjectSelected(WPARAM wParam, LPARAM lParam);
	afx_msg void OnShowHideColumn(UINT id);
	afx_msg void OnUpdateShowHideColumnsPopup(CCmdUI* pCmdUI);

	//afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()
};

