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

class CTracksToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CTracksView : public CViewDockingPane
{
	DECLARE_DYNAMIC(CTracksView)
public:
	CTracksView() noexcept;
	virtual ~CTracksView();

	void AdjustLayout();
	void OnChangeVisualStyle();

protected:
	CTracksToolBar m_wndToolBar;
	CListCtrl m_wndClassView;
	CImageList m_ClassViewImages;
	UINT m_nCurrSort;
	int addingItemIndex = -1;
	bool updatingTracksSelection = false;
	bool addingTrack = false;

	void FillClassView();
	virtual void OnInitialUpdate();
	virtual void OnUpdate(const LPARAM lHint);
	void ReloadTracksList(CSchedulerDoc* pDoc);

// Overrides
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnChangeActiveTab(WPARAM, LPARAM);
	afx_msg void OnAddTrack();
	afx_msg void OnRemoveTrack();
	afx_msg void OnEditTrack();
	afx_msg void OnUpdateOtherCommandToolbarButtons(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAddTrackCommand(CCmdUI* pCmdUI);
	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnTrackObjectSelected(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTrackListItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
};

