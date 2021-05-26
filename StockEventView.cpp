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
#include "utils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CStockEventsListCtrl, CMFCListCtrl)

COLORREF CStockEventsListCtrl::OnGetCellBkColor(int nRow, int nColumn)
{
	CScheduleStockEvent* event = (CScheduleStockEvent*)GetItemData(nRow);
	if (event != nullptr && nColumn == (int)EventsViewColumns::Color)
	{		
		D2D1::ColorF color(event->GetColor());
		return RGB(color.r * 255.f, color.g * 255.f, color.b * 255.f);
	}
	return CMFCListCtrl::OnGetCellBkColor(nRow,nColumn);
}
int CStockEventsListCtrl::OnCompareItems(LPARAM lParam1, LPARAM lParam2, int iColumn)
{
	CScheduleStockEvent* s1 = (CScheduleStockEvent*)lParam1;
	CScheduleStockEvent* s2 = (CScheduleStockEvent*)lParam2;
	EventsViewColumns col = (EventsViewColumns)iColumn;
	switch (col)
	{
	case EventsViewColumns::Name:
		return s1->GetName().CompareNoCase(s2->GetName());
	case EventsViewColumns::Duration:
		return (int)(s1->GetDuration() - s2->GetDuration()).count();
	case EventsViewColumns::Color:
		return (int)(s1->GetColor() - s2->GetColor());
	case EventsViewColumns::Usage:
		return s1->GetUsage() - s2->GetUsage();
	default:
		return 0;
	}

}

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
	//ON_NOTIFY(LVN_COLUMNCLICK, IDC_EVENT_LIST, OnEventListColumnClick)
	ON_MESSAGE(WM_EVENT_OBJECT_SELECTED, OnEventObjectSelected)
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
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_EDITLABELS | LVS_SHOWSELALWAYS | LVS_SINGLESEL ;

	if (!m_wndEventListView.Create(dwViewStyle, rectDummy, this, IDC_EVENT_LIST))
	{
		TRACE0("Failed to create file view\n");
		return -1;      // fail to create
	}
	m_wndEventListView.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_wndEventListView.EnableMarkSortedColumn();
	m_wndEventListView.EnableMultipleSort(FALSE);

	CString strTemp;
	BOOL bNameValid = strTemp.LoadString(IDS_STOCK_EVENT_NAME);
	ASSERT(bNameValid);
	m_wndEventListView.InsertColumn((int)EventsViewColumns::Name, strTemp, LVCFMT_LEFT);
	bNameValid = strTemp.LoadString(IDS_STOCK_EVENT_DURATION);
	ASSERT(bNameValid);
	m_wndEventListView.InsertColumn((int)EventsViewColumns::Duration, strTemp, LVCFMT_LEFT);
	bNameValid = strTemp.LoadString(IDS_STOCK_EVENT_USAGE);
	ASSERT(bNameValid);
	m_wndEventListView.InsertColumn((int)EventsViewColumns::Usage, strTemp, LVCFMT_LEFT);
	bNameValid = strTemp.LoadString(IDS_STOCK_EVENT_COLOR);
	ASSERT(bNameValid);
	m_wndEventListView.InsertColumn((int)EventsViewColumns::Color, strTemp, LVCFMT_LEFT);

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

	
	AdjustLayout();

	m_wndEventListView.SetSortColumn((int)EventsViewColumns::Name, true);

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


void CStockEventView::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	addingEvent = false;
	NMLVDISPINFO* listInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	if (NULL != listInfo->item.pszText)
	{
		int sortColumn = m_wndEventListView.GetHeaderCtrl().GetSortColumn();
		bool ascending = m_wndEventListView.GetHeaderCtrl().IsAscending();

		m_wndEventListView.SetItemText(listInfo->item.iItem, 0, listInfo->item.pszText);
		WPARAM eventIdParam = -1;
		if (listInfo->item.lParam == 0)
		{
			CScheduleStockEvent* event = GetDocument()->AddEvent(listInfo->item.pszText, reinterpret_cast<LPARAM>(this));
			eventIdParam = (WPARAM)event->GetId();
			m_wndEventListView.SetItemData(listInfo->item.iItem, (LPARAM)event);
			CString color;
			//color.Format(_T("%x"), event->GetColor());
			auto duration = FormatDuration(event->GetDuration());
			CString usage;
			usage.Format(_T("%d"), event->GetUsage());
			m_wndEventListView.SetItem(listInfo->item.iItem, (int)EventsViewColumns::Duration, LVIF_TEXT, duration, 0, 0, 0, 0);
			m_wndEventListView.SetItem(listInfo->item.iItem, (int)EventsViewColumns::Color, LVIF_TEXT, color, 0, 0, 0, 0);
			m_wndEventListView.SetItem(listInfo->item.iItem, (int)EventsViewColumns::Usage, LVIF_TEXT, usage, 0, 0, 0, 0);
		}
		else
		{
			CScheduleStockEvent* event = (CScheduleStockEvent*)listInfo->item.lParam;
			eventIdParam = (WPARAM)event->GetId();
			GetDocument()->UpdateStockEventName(event, listInfo->item.pszText, reinterpret_cast<LPARAM>(this));
		}
		if (sortColumn != -1)
		{
			m_wndEventListView.Sort(sortColumn, ascending);
		}

		AfxGetMainWnd()->PostMessage(WM_STOCK_EVENT_OBJECT_SELECTED, eventIdParam, (LPARAM)this);
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
CString CStockEventView::FormatDuration(std::chrono::seconds seconds)
{
	CString str;
	typedef std::chrono::duration<int, std::ratio<86400>> days;
	const auto d = std::chrono::duration_cast<days>(seconds);
	if (d.count() > 0)
	{
		str.AppendFormat(_T("%d days "),d.count());
	}
	seconds = seconds - d;
	const auto h = std::chrono::duration_cast<std::chrono::hours>(seconds);
	str.AppendFormat(_T("%02dh"), h.count());
	seconds = seconds -  h;
	const auto m = std::chrono::duration_cast<std::chrono::minutes>(seconds);
	str.AppendFormat(_T(":%02dm"), m.count());
	seconds = seconds - m;
	str.AppendFormat(_T(":%02ds"), (int)seconds.count());
	return str;
}
void CStockEventView::ReloadEventsList(CSchedulerDoc* pDoc)
{
	m_wndEventListView.SetRedraw(FALSE);	
	int sortColumn = m_wndEventListView.GetHeaderCtrl().GetSortColumn();
	bool ascending = m_wndEventListView.GetHeaderCtrl().IsAscending();

	if (TRUE)//m_wndEventListView.GetItemCount() != pDoc->GetStockEvents().size()
	{
		m_wndEventListView.DeleteAllItems();
		int index = 0;
		for (const auto& ev : pDoc->GetStockEvents())
		{
			auto insertedIndex = m_wndEventListView.InsertItem(LVIF_TEXT | LVIF_STATE | LVIF_PARAM, index++, ev->GetName(), 0, 0, 0, (LPARAM)ev.get());
			CString color;
			CString usage;
			usage.Format(_T("%d"), ev->GetUsage());
			auto duration = FormatDuration(ev->GetDuration());
			m_wndEventListView.SetItem(insertedIndex, (int)EventsViewColumns::Duration, LVIF_TEXT, duration, 0, 0, 0, 0);
			m_wndEventListView.SetItem(insertedIndex, (int)EventsViewColumns::Color, LVIF_TEXT, color, 0, 0, 0, 0);
			m_wndEventListView.SetItem(insertedIndex, (int)EventsViewColumns::Usage, LVIF_TEXT, usage, 0, 0, 0, 0);
		}
	}
	else
	{
		for (int i = 0; i < m_wndEventListView.GetItemCount(); ++i)
		{
			CScheduleStockEvent* event = (CScheduleStockEvent*)m_wndEventListView.GetItemData(i);
			CString color;
			CString usage;
			usage.Format(_T("%d"), event->GetUsage());
			auto duration = FormatDuration(event->GetDuration());
			m_wndEventListView.SetItem(i, (int)EventsViewColumns::Name, LVIF_TEXT, event->GetName(), 0, 0, 0, 0);
			m_wndEventListView.SetItem(i, (int)EventsViewColumns::Duration, LVIF_TEXT, duration, 0, 0, 0, 0);
			m_wndEventListView.SetItem(i, (int)EventsViewColumns::Color, LVIF_TEXT, color, 0, 0, 0, 0);
			m_wndEventListView.SetItem(i, (int)EventsViewColumns::Usage, LVIF_TEXT, usage, 0, 0, 0, 0);
		}
	}

	if (sortColumn != -1)
	{
		m_wndEventListView.Sort(sortColumn,ascending);
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
	int colCount = m_wndEventListView.GetHeaderCtrl().GetItemCount();
	for (int i = 0; i < colCount;++i)
	{
		if (m_wndEventListView.GetColumnWidth(i) < 20)
		{
			m_wndEventListView.SetColumnWidth(i, rectClient.Width() / colCount);
		}
	}
}

void CStockEventView::OnBeginDragEvent(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int dragItemIndex = pNMListView->iItem;
	//// Create a drag image
	POINT pt = {0,0};
	CImageList*  dragImageList = m_wndEventListView.CreateDragImage(dragItemIndex, &pt);
	ASSERT(dragImageList); //make sure it was created
	CScheduleStockEvent* event = (CScheduleStockEvent*)m_wndEventListView.GetItemData(dragItemIndex);
	GetMainFrame()->StartDraggingStockEvent(event->GetId(), dragImageList, pNMListView->ptAction);		

	*pResult = 0;
}


void CStockEventView::OnAddEvent()
{
	int index = m_wndEventListView.GetItemCount();
	CString newEventName(_T("New Event"));
	auto insertedIndex = m_wndEventListView.InsertItem(LVIF_TEXT | LVIF_STATE | LVIF_PARAM, index, newEventName, 0, 0, 0, 0);
	CString color;
	color.Format(_T("%x"), 0);
	auto duration = FormatDuration(std::chrono::seconds(3600));
	m_wndEventListView.SetItem(insertedIndex, 1, LVIF_TEXT, duration, 0, 0, 0, 0);
	m_wndEventListView.SetItem(insertedIndex, 2, LVIF_TEXT, color, 0, 0, 0, 0);

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
		CScheduleStockEvent* event = (CScheduleStockEvent*)m_wndEventListView.GetItemData(index);
		CString message;
		message.Format(_T("Are you sure you want to delete event %s? All scheduled events from it will be removed."), event->GetName().GetString());
		const int result = MessageBox(message, _T("Are you sure?"), MB_YESNO|MB_ICONWARNING);
		if (result == IDYES)
		{
			GetDocument()->DeleteStockEvent(event, reinterpret_cast<LPARAM>(this));
			m_wndEventListView.DeleteItem(index);
			AfxGetMainWnd()->PostMessage(WM_EVENT_OBJECT_SELECTED, (WPARAM)-1, (LPARAM)this);
			AfxGetMainWnd()->PostMessage(WM_STOCK_EVENT_OBJECT_SELECTED, (WPARAM)-1, (LPARAM)this);
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
			WPARAM eventIdParam = -1;
			if (pNMListView->lParam != 0)
			{
				CScheduleStockEvent* event = (CScheduleStockEvent*)pNMListView->lParam;
				eventIdParam = (WPARAM)event->GetId();
			}

			AfxGetMainWnd()->PostMessage(WM_STOCK_EVENT_OBJECT_SELECTED, eventIdParam, (LPARAM)this);
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
	auto scheduledEvent = GetDocument()->GetEvent(eventId);
	if (!scheduledEvent) return FALSE;
	auto stockEvent = GetDocument()->GetStockEvent(scheduledEvent->GetStockId());
	if (!stockEvent) return (LRESULT)FALSE;
	for (int i = 0; i < m_wndEventListView.GetItemCount(); i++)
	{
		CScheduleStockEvent* event = (CScheduleStockEvent*)m_wndEventListView.GetItemData(i);
		if (event->GetId() == stockEvent->GetId())
		{
			updatingEventsSelection = true;
			m_wndEventListView.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
			m_wndEventListView.SetSelectionMark(i);
			break;
		}
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
