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

#include "pch.h"
#include "framework.h"
#include "MainFrm.h"
#include "TracksView.h"
#include "Resource.h"
#include "scheduler.h"

class CTrackViewMenuButton : public CMFCToolBarMenuButton
{
	friend class CTracksToolBar;

	DECLARE_SERIAL(CTrackViewMenuButton)

public:
	CTrackViewMenuButton(HMENU hMenu = nullptr) noexcept : CMFCToolBarMenuButton((UINT)-1, hMenu, -1)
	{
	}

	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE,
		BOOL bCustomizeMode = FALSE, BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE)
	{
		pImages = CMFCToolBar::GetImages();

		CAfxDrawState ds;
		pImages->PrepareDrawImage(ds);

		CMFCToolBarMenuButton::OnDraw(pDC, rect, pImages, bHorz, bCustomizeMode, bHighlight, bDrawBorder, bGrayDisabledButtons);

		pImages->EndDrawImage(ds);
	}
};

IMPLEMENT_SERIAL(CTrackViewMenuButton, CMFCToolBarMenuButton, 1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTracksView::CTracksView() noexcept
{
	m_nCurrSort = ID_SORTING_GROUPBYTYPE;
}

CTracksView::~CTracksView()
{
}
IMPLEMENT_DYNAMIC(CTracksView, CViewDockingPane)

BEGIN_MESSAGE_MAP(CTracksView, CViewDockingPane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()	
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_ADD_TRACK, OnAddTrack)
	ON_COMMAND(ID_EDIT_TRACK, OnEditTrack)
	ON_COMMAND(ID_REMOVE_TRACK, OnRemoveTrack)
	ON_UPDATE_COMMAND_UI(ID_EDIT_TRACK, OnUpdateOtherCommandToolbarButtons)
	ON_UPDATE_COMMAND_UI(ID_REMOVE_TRACK, OnUpdateOtherCommandToolbarButtons)
	ON_UPDATE_COMMAND_UI(ID_ADD_TRACK, OnUpdateAddTrackCommand)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_TRACKS_LIST, OnEndLabelEdit)
	ON_MESSAGE(WM_TRACK_OBJECT_SELECTED, OnTrackObjectSelected)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_TRACKS_LIST, OnTrackListItemChanged)	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClassView message handlers

int CTracksView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create views:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_EDITLABELS | LVS_SHOWSELALWAYS | LVS_SINGLESEL;

	if (!m_wndClassView.Create(dwViewStyle, rectDummy, this, IDC_TRACKS_LIST))
	{
		TRACE0("Failed to create Class View\n");
		return -1;      // fail to create
	}

	CString strTemp;
	BOOL bNameValid = strTemp.LoadString(IDS_TRACK_NAME);
	ASSERT(bNameValid);
	m_wndClassView.InsertColumn(0, strTemp, LVCFMT_LEFT);
	bNameValid = strTemp.LoadString(IDS_TRACK_START_DATE);
	ASSERT(bNameValid);
	m_wndClassView.InsertColumn(1, strTemp, LVCFMT_LEFT);

	// Load images:
	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_TRACKS_TOOLBAR);
	m_wndToolBar.LoadToolBar(IDR_TRACKS_TOOLBAR, 0, 0, TRUE /* Is locked */);
	CMFCToolBar::AddToolBarForImageCollection(IDR_TRACKS_TOOLBAR, IDB_TRACKS_TOOLBAR_24);
	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	AdjustLayout();

	// Fill in some static tree view data (dummy code, nothing magic here)
//	FillClassView();

	return 0;
}

void CTracksView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CTracksView::OnInitialUpdate()
{
	CSchedulerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	ReloadTracksList(pDoc);
}
void CTracksView::OnUpdate(const LPARAM lHint)
{
	CTracksView* potentialSource = reinterpret_cast<CTracksView*>(lHint);
	if (potentialSource && potentialSource == this) return;

	CSchedulerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	ReloadTracksList(pDoc);
}

void CTracksView::ReloadTracksList(CSchedulerDoc* pDoc)
{
	m_wndClassView.SetRedraw(FALSE);
	m_wndClassView.DeleteAllItems();
	
	std::chrono::minutes utcOffset(pDoc->GetUTCOffsetMinutes());

	int index = 0;
	for (const auto& track : pDoc->GetTracks())
	{
		auto insertedIndex = m_wndClassView.InsertItem(LVIF_TEXT | LVIF_STATE | LVIF_PARAM ,index++, 
			track->GetName(),0,0,0, (LPARAM)track.get());
		auto startTime = track->GetStartTime();
		date::local_seconds start{ std::chrono::duration_cast<std::chrono::seconds>(startTime.time_since_epoch()) - utcOffset };
		CString startText(date::format("%Y %b %d %R", start).c_str());

		m_wndClassView.SetItem(insertedIndex, 1, LVIF_TEXT, startText, 0, 0, 0, 0);
	}

	m_wndClassView.SetRedraw(TRUE);
}

void CTracksView::FillClassView()
{
}

void CTracksView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CListCtrl* pWndTree = (CListCtrl*)&m_wndClassView;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// Select clicked item:
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		int hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != -1)
		{
			pWndTree->SetSelectionMark(hTreeItem);
		}
	}

	pWndTree->SetFocus();
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_TRACKS, point.x, point.y, this, TRUE);
}

void CTracksView::AdjustLayout()
{
	if (GetSafeHwnd() == nullptr)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndClassView.SetWindowPos(nullptr, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);

	auto c = m_wndClassView.GetColumnWidth(0);
	if (m_wndClassView.GetColumnWidth(0) < 20)
	{
		m_wndClassView.SetColumnWidth(0, rectClient.Width() / 2);
	}
	if (m_wndClassView.GetColumnWidth(1) < 20)
	{
		m_wndClassView.SetColumnWidth(1, rectClient.Width() / 2);
	}

}

BOOL CTracksView::PreTranslateMessage(MSG* pMsg)
{
	return CDockablePane::PreTranslateMessage(pMsg);
}

void CTracksView::OnAddTrack()
{
	CSchedulerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	std::chrono::minutes utcOffset(pDoc->GetUTCOffsetMinutes());
	int index = m_wndClassView.GetItemCount();
	CString newEventName(_T("New Track"));
	auto insertedIndex = m_wndClassView.InsertItem(LVIF_TEXT | LVIF_STATE | LVIF_PARAM, index, newEventName, 0, 0, 0, 0);
	date::local_seconds start{ std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()) - utcOffset };
	CString startText(date::format("%Y %b %d %R", start).c_str());
	m_wndClassView.SetItem(insertedIndex, 1, LVIF_TEXT, startText, 0, 0, 0, 0);

	addingItemIndex = index;
	auto editControl = m_wndClassView.EditLabel(index);
	if (editControl)
	{
		addingTrack = true;
		editControl->SetWindowText(newEventName);
		editControl->SetSel(0, -1);
	}

}
void CTracksView::OnRemoveTrack()
{
	POSITION pos = m_wndClassView.GetFirstSelectedItemPosition();
	if (pos)
	{
		int index = m_wndClassView.GetNextSelectedItem(pos);		
		CScheduleTrack * track = (CScheduleTrack*)m_wndClassView.GetItemData(index);
		CString message;
		message.Format(_T("Are you sure you want to delete track %s?"), track->GetName().GetString());
		const int result = MessageBox(message, _T("Are you sure?"), MB_YESNO | MB_ICONWARNING);
		if (result == IDYES)
		{
			GetDocument()->DeleteTrack(track, reinterpret_cast<LPARAM>(this));
			m_wndClassView.DeleteItem(index);
			AfxGetMainWnd()->PostMessage(WM_TRACK_OBJECT_SELECTED, -1, (LPARAM)this);
		}
	}

}
void CTracksView::OnEditTrack()
{
	POSITION pos = m_wndClassView.GetFirstSelectedItemPosition();
	if (pos)
	{
		int index = m_wndClassView.GetNextSelectedItem(pos);
		auto editControl = m_wndClassView.EditLabel(index);
		auto text = m_wndClassView.GetItemText(index, 0);
		if (editControl)
		{
			editControl->SetWindowText(text);
			editControl->SetSel(0, -1);
		}
	}
}
void CTracksView::OnUpdateOtherCommandToolbarButtons(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_wndClassView.GetFirstSelectedItemPosition() != nullptr);
}
void CTracksView::OnUpdateAddTrackCommand(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
}

void CTracksView::OnTrackListItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (updatingTracksSelection)
	{
		updatingTracksSelection = false;
		return;
	}

	if (addingTrack) return;

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (pNMListView->uChanged & LVIF_STATE)
	{
		if (pNMListView->uNewState & LVIS_SELECTED)
		{
			WPARAM trackIdParam = -1;

			if (pNMListView->lParam != 0)
			{
				CScheduleTrack* track = (CScheduleTrack*)pNMListView->lParam;
				trackIdParam = (WPARAM)track->GetId();
			}
			
			AfxGetMainWnd()->PostMessage(WM_TRACK_OBJECT_SELECTED, trackIdParam, (LPARAM)this);
		}
		else
		{
			// do stuff...
		}
	}

	*pResult = 0;
}
void CTracksView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rectTree;
	m_wndClassView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CTracksView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndClassView.SetFocus();
}

void CTracksView::OnChangeVisualStyle()
{
	m_ClassViewImages.DeleteImageList();

	UINT uiBmpId = IDB_EXPLORER_24;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("Can't load bitmap: %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= ILC_COLOR24;

	m_ClassViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_ClassViewImages.Add(&bmp, RGB(255, 0, 0));

	m_wndClassView.SetImageList(&m_ClassViewImages, TVSIL_NORMAL);

	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(IDB_EXPLORER_24, 0, 0, TRUE /* Locked */);
}

void CTracksView::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	addingTrack = false;
	NMLVDISPINFO* listInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	if (NULL != listInfo->item.pszText)
	{
		m_wndClassView.SetItemText(listInfo->item.iItem, 0, listInfo->item.pszText);
		WPARAM trackIdParam = -1;
		if (listInfo->item.lParam == 0)
		{
			CScheduleTrack* track = GetDocument()->AddTrack(listInfo->item.pszText, reinterpret_cast<LPARAM>(this));
			trackIdParam = (WPARAM)track->GetId();
			m_wndClassView.SetItemData(listInfo->item.iItem, (LPARAM)track);
		}
		else
		{
			CScheduleTrack* track = (CScheduleTrack*)listInfo->item.lParam;
			trackIdParam = (WPARAM)track->GetId();
			GetDocument()->UpdateTrackName(track, listInfo->item.pszText, reinterpret_cast<LPARAM>(this));
		}
		
		AfxGetMainWnd()->PostMessage(WM_TRACK_OBJECT_SELECTED, trackIdParam, (LPARAM)this);
	}
	else
	{
		if (addingItemIndex > -1)
		{
			m_wndClassView.DeleteItem(addingItemIndex);			
		}
	}
	addingItemIndex = -1;
	*pResult = 0;

}
LRESULT CTracksView::OnTrackObjectSelected(WPARAM wParam, LPARAM lParam)
{

	return (LRESULT)TRUE;
}
