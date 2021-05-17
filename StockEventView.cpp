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
#include "mainfrm.h"
#include "StockEventView.h"
#include "Resource.h"
#include "scheduler.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CStockEventView

CStockEventView::CStockEventView() noexcept
{	
}

CStockEventView::~CStockEventView()
{
}

IMPLEMENT_DYNAMIC(CStockEventView, CViewDockingPane)

BEGIN_MESSAGE_MAP(CStockEventView, CViewDockingPane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_ADD_EVENT, OnAddEvent)
	ON_COMMAND(ID_EDIT_EVENT, OnEditEvent)
	ON_COMMAND(ID_REMOVE_EVENT, OnRemoveEvent)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_EVENT_LIST, OnEndLabelEdit)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_NOTIFY(LVN_BEGINDRAG, IDC_EVENT_LIST, OnBeginDragEvent)
	ON_UPDATE_COMMAND_UI(ID_EDIT_EVENT,OnUpdateCommandToolbarButtons)
	ON_UPDATE_COMMAND_UI(ID_REMOVE_EVENT, OnUpdateCommandToolbarButtons)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_EVENT_LIST, OnEventListItemChanged)
	ON_MESSAGE(WM_EVENT_OBJECT_SELECTED, OnEventObjectSelected)
	//ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_REMOVE_SCHEDULED_EVENT, &CStockEventView::OnRemoveScheduledEventKey)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar message handlers

int CStockEventView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create view:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | LVS_LIST | LVS_EDITLABELS | LVS_SHOWSELALWAYS | LVS_SINGLESEL;

	if (!m_wndEventListView.Create(dwViewStyle, rectDummy, this, IDC_EVENT_LIST))
	{
		TRACE0("Failed to create file view\n");
		return -1;      // fail to create
	}

	// Load view images:
	m_EventListImages.Create(IDB_FILE_VIEW, 16, 0, RGB(255, 0, 255));
	m_wndEventListView.SetImageList(&m_EventListImages, LVSIL_SMALL);

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_EXPLORER);
	m_wndToolBar.LoadToolBar(IDR_EXPLORER, 0, 0, TRUE /* Is locked */);	
	CMFCToolBar::AddToolBarForImageCollection(IDR_EXPLORER, IDB_EXPLORER_24);
	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	// Fill in some static tree view data (dummy code, nothing magic here)	
	AdjustLayout();

	
	return 0;
}
void CStockEventView::OnUpdateCommandToolbarButtons(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_wndEventListView.GetFirstSelectedItemPosition() != nullptr);
}
void CStockEventView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CStockEventView::FillFileView()
{
	m_wndEventListView.InsertItem(0,_T("FakeApp files"), 0);
	//m_wndEventListView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);

	m_wndEventListView.InsertItem(0,_T("FakeApp.cpp"), 1);
	m_wndEventListView.InsertItem(0, _T("FakeApp.rc"), 1);
	m_wndEventListView.InsertItem(0, _T("FakeAppDoc.cpp"), 1);
	m_wndEventListView.InsertItem(0, _T("FakeAppView.cpp"), 1);
	m_wndEventListView.InsertItem(0, _T("MainFrm.cpp"), 1);
	m_wndEventListView.InsertItem(0, _T("pch.cpp"), 1);

	m_wndEventListView.InsertItem(0, _T("FakeApp.h"), 2);
	m_wndEventListView.InsertItem(0, _T("FakeAppDoc.h"), 2);
	m_wndEventListView.InsertItem(0, _T("FakeAppView.h"), 2);
	m_wndEventListView.InsertItem(0, _T("Resource.h"), 2);
	m_wndEventListView.InsertItem(0, _T("MainFrm.h"), 2);
	m_wndEventListView.InsertItem(0, _T("pch.h"), 2);

	m_wndEventListView.InsertItem(0,_T("FakeApp.ico"), 2);
	m_wndEventListView.InsertItem(0, _T("FakeApp.rc2"), 2);
	m_wndEventListView.InsertItem(0, _T("FakeAppDoc.ico"), 2);
	m_wndEventListView.InsertItem(0, _T("FakeToolbar.bmp"), 2);

}

void CStockEventView::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	addingEvent = false;
	NMLVDISPINFO* listInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	if (NULL != listInfo->item.pszText)
	{
		m_wndEventListView.SetItemText(listInfo->item.iItem, 0, listInfo->item.pszText);
		GetDocument()->UpdateStockEventName(listInfo->item.iItem, listInfo->item.pszText, reinterpret_cast<LPARAM>(this));
	}
	else
	{
		if (addingItemIndex > -1)
		{
			m_wndEventListView.DeleteItem(addingItemIndex);
		}
	}
	addingItemIndex = -1;
	*pResult = 0;
}

void CStockEventView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CListCtrl* pWndTree = (CListCtrl*) &m_wndEventListView;
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
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EXPLORER, point.x, point.y, this, TRUE);
}

void CStockEventView::OnInitialUpdate()
{
	CSchedulerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	ReloadEventsList(pDoc);
}
void CStockEventView::OnUpdate(const LPARAM lHint)
{
	CStockEventView* potentialSource = reinterpret_cast<CStockEventView*>(lHint);
	if (potentialSource && potentialSource == this) return;

	CSchedulerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	ReloadEventsList(pDoc);
}

void CStockEventView::ReloadEventsList(CSchedulerDoc* pDoc)
{
	m_wndEventListView.SetRedraw(FALSE);
	m_wndEventListView.DeleteAllItems();
	int index = 0;
	for (const auto& ev : pDoc->GetStockEvents())
	{
		//LVITEM item;
		
		m_wndEventListView.InsertItem(index++, ev->GetName(), 0);
	}

	m_wndEventListView.SetRedraw(TRUE);
}

void CStockEventView::AdjustLayout()
{
	if (GetSafeHwnd() == nullptr)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndEventListView.SetWindowPos(nullptr, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CStockEventView::OnBeginDragEvent(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int dragItemIndex = pNMListView->iItem;
	//// Create a drag image
	POINT pt = {0,0};
	CImageList*  dragImageList = m_wndEventListView.CreateDragImage(dragItemIndex, &pt);
	ASSERT(dragImageList); //make sure it was created

	//CBitmap bitmap;
	//bitmap.LoadBitmap(IDB_FILE_VIEW_24);
	//dragImageList->Replace(0,&bitmap, &bitmap);

	GetMainFrame()->StartDraggingStockEvent(dragItemIndex, dragImageList, pNMListView->ptAction);
		

	*pResult = 0;
}


void CStockEventView::OnAddEvent()
{
	int index = m_wndEventListView.GetItemCount();
	CString newEventName(_T("New Event"));
	m_wndEventListView.InsertItem(index, newEventName, 0);
	addingItemIndex = index;
	auto editControl = m_wndEventListView.EditLabel(index);
	if (editControl)
	{
		addingEvent = true;
		editControl->SetWindowText(newEventName);
		editControl->SetSel(0, -1);
	}
}

void CStockEventView::OnRemoveEvent()
{
	POSITION pos = m_wndEventListView.GetFirstSelectedItemPosition();
	if (pos)
	{
		int index = m_wndEventListView.GetNextSelectedItem(pos);
		auto text = m_wndEventListView.GetItemText(index, 0);
		CString message;
		message.Format(_T("Are you sure you want to delete event %s? All scheduled events from it will be removed."),text.GetString());
		const int result = MessageBox(message, _T("Are you sure?"), MB_YESNO|MB_ICONWARNING);
		if (result == IDYES)
		{
			GetDocument()->DeleteStockEvent(index, reinterpret_cast<LPARAM>(this));
			m_wndEventListView.DeleteItem(index);
		}		
	}
}

void CStockEventView::OnEditEvent()
{
	POSITION pos = m_wndEventListView.GetFirstSelectedItemPosition();
	if (pos)
	{
		int index = m_wndEventListView.GetNextSelectedItem(pos);
		auto editControl = m_wndEventListView.EditLabel(index);
		auto text = m_wndEventListView.GetItemText(index,0);
		if (editControl)
		{
			editControl->SetWindowText(text);
			editControl->SetSel(0, -1);
		}
	}
}

void CStockEventView::OnEventListItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (updatingEventsSelection)
	{
		updatingEventsSelection = false;
		return;
	}

	if (addingEvent) return;

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (pNMListView->uChanged & LVIF_STATE)
	{
		if (pNMListView->uNewState & LVIS_SELECTED)
		{
			auto event = GetDocument()->GetStockEventAtIndex(pNMListView->iItem);
			if (event)
			{
				AfxGetMainWnd()->PostMessage(WM_EVENT_OBJECT_SELECTED, (WPARAM)event->GetId(), (LPARAM)this);
			}
		}
		else
		{
			// do stuff...
		}		
	}

	*pResult = 0;
}
LRESULT CStockEventView::OnEventObjectSelected(WPARAM wParam, LPARAM lParam)
{	
	CStockEventView* sender = reinterpret_cast<CStockEventView*>(lParam);
	if (sender && sender == this) return 0;

	POSITION pos = m_wndEventListView.GetFirstSelectedItemPosition();
	if (pos)
	{
		int oldSelectedIndex = m_wndEventListView.GetNextSelectedItem(pos);
		updatingEventsSelection = true;
		m_wndEventListView.SetItemState(oldSelectedIndex, ~LVIS_SELECTED, LVIS_SELECTED);
	}

	int eventId = (int)wParam;
	int index = GetDocument()->GetStockEventIndex(eventId);
	if (index >= 0 && index < m_wndEventListView.GetItemCount())
	{
		updatingEventsSelection = true;
		m_wndEventListView.SetItemState(index, LVIS_SELECTED, LVIS_SELECTED);
		m_wndEventListView.SetSelectionMark(index);
	}
	return (LRESULT)TRUE;
}
void CStockEventView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rectTree;
	m_wndEventListView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CStockEventView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndEventListView.SetFocus();
}

void CStockEventView::OnChangeVisualStyle()
{
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(IDB_EXPLORER_24, 0, 0, TRUE /* Locked */);

	m_EventListImages.DeleteImageList();

	UINT uiBmpId = IDB_FILE_VIEW_24;

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

	m_EventListImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_EventListImages.Add(&bmp, RGB(255, 0, 255));

	m_wndEventListView.SetImageList(&m_EventListImages, LVSIL_SMALL);
}




void CStockEventView::OnRemoveScheduledEventKey()
{
	// TODO: Add your command handler code here
}
