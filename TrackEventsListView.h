#pragma once
#include "ViewDockingPane.h"


class CTrackEventsListView :
    public CViewDockingPane
{
    DECLARE_DYNAMIC(CTrackEventsListView)
public:
    CTrackEventsListView() noexcept;
    void AdjustLayout();
	void OnChangeVisualStyle();

	// Attributes
public:
protected:
	virtual void OnInitialUpdate();
	virtual void OnUpdate(const LPARAM lHint);
protected:
	CListCtrl m_wndEventsList;

public:
	virtual ~CTrackEventsListView();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnPaint();
	afx_msg LRESULT OnTrackObjectSelected(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

