#pragma once
#include "ViewDockingPane.h"


class CTrackEventsListToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*)GetOwner(), bDisableIfNoHndler);
	}
	//virtual BOOL OnUserToolTip(CMFCToolBarButton* pButton, CString& strTTText) const;


	virtual BOOL AllowShowOnList() const { return FALSE; }
};

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
	CImageList m_TrackEventsListImages;
	CTrackEventsListToolBar m_wndToolBar;
	int trackId = -1;
public:
	virtual ~CTrackEventsListView();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnPaint();
	afx_msg LRESULT OnTrackObjectSelected(WPARAM wParam, LPARAM lParam);
	afx_msg void OnUpdateCommandToolbarButtons(CCmdUI* pCmdUI);
	afx_msg void OnExportToCSV();
	DECLARE_MESSAGE_MAP()
};

