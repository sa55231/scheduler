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
#include "EventMaxCountConstraint.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace
{
	CString FormatDuration(std::chrono::seconds seconds)
	{
		CString str;
		typedef std::chrono::duration<int, std::ratio<86400>> days;
		const auto d = std::chrono::duration_cast<days>(seconds);
		if (d.count() > 0)
		{
			str.AppendFormat(_T("%d days "), d.count());
		}
		seconds = seconds - d;
		const auto h = std::chrono::duration_cast<std::chrono::hours>(seconds);
		str.AppendFormat(_T("%02dh"), h.count());
		seconds = seconds - h;
		const auto m = std::chrono::duration_cast<std::chrono::minutes>(seconds);
		str.AppendFormat(_T(":%02dm"), m.count());
		seconds = seconds - m;
		str.AppendFormat(_T(":%02ds"), (int)seconds.count());
		return str;
	}

	int GetEventMaxCount(CScheduleStockEvent* event)
	{
		for (const auto& c : event->GetConstraints())
		{
			if (c->GetType() == ConstraintType::MaxCountConstraint)
			{
				auto maxCountConstraint = (CEventMaxCountConstraint*)(c.get());
				return maxCountConstraint->GetCount();
			}
		}
		return 0;
	}
}
IMPLEMENT_DYNAMIC(CStockEventsListCtrl, CMFCListCtrl)

COLORREF CStockEventsListCtrl::OnGetCellBkColor(int nRow, int nColumn)
{
	CScheduleStockEvent* event = (CScheduleStockEvent*)GetItemData(nRow);

	if (event != nullptr && !event->CanScheduleGlobally())
	{
		return RGB(128, 128, 128);
	}

	EventsViewColumns col = EventsViewColumns::ColumnsCount;
	for (const auto& c : columnMappings)
	{
		if (c.second.colIndex == nColumn && c.second.visible)
		{
			col = c.first;
			break;
		}
	}

	if (event != nullptr && col == EventsViewColumns::Color)
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
	EventsViewColumns col = EventsViewColumns::ColumnsCount;
	for (const auto& c : columnMappings)
	{
		if (c.second.colIndex == iColumn && c.second.visible)
		{
			col = c.first;
			break;
		}
	}
	ASSERT(col != EventsViewColumns::ColumnsCount);
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
	case EventsViewColumns::MaxCount:
		return GetEventMaxCount(s1) - GetEventMaxCount(s2);
	default:
		return 0;
	}
}
BOOL CStockEventsListCtrl::SetCellText(int index, EventsViewColumns column, LPCTSTR text)
{
	if (columnMappings.at(column).visible)
	{
		return SetItem(index, columnMappings.at(column).colIndex, LVIF_TEXT, text, 0, 0, 0, 0);
	}
	return FALSE;
}

int CStockEventsListCtrl::GetColWidth(EventsViewColumns col) const
{
	return columnMappings.at(col).colWidth;
}

void CStockEventsListCtrl::SetColWidth(EventsViewColumns col, int width)
{
	columnMappings.at(col).colWidth = width;
	if (columnMappings.at(col).visible)
	{
		SetColumnWidth(columnMappings.at(col).colIndex, width);
	}
}

bool CStockEventsListCtrl::IsColumnVisible(EventsViewColumns col) const
{
	return columnMappings.at(col).visible;
}
void CStockEventsListCtrl::SetColumnVisible(EventsViewColumns col, bool flag)
{
	if (!flag) ASSERT(col != EventsViewColumns::Name);

	if (columnMappings.at(col).visible == flag) return;

	columnMappings.at(col).visible = flag;
	auto& colMapping = columnMappings.at(col);
	if (flag)
	{
		colMapping.colIndex = GetHeaderCtrl().GetItemCount();
		InsertColumn(colMapping.colIndex, colMapping.caption, colMapping.format);
		SetColWidth(col, colMapping.colWidth);
		for (int i = 0; i < GetItemCount(); i++)
		{
			CScheduleStockEvent* event = (CScheduleStockEvent*)GetItemData(i);
			CString color;
			CString usage;
			usage.Format(_T("%d"), event->GetUsage());
			CString maxCount;
			maxCount.Format(_T("%d"), GetEventMaxCount(event));
			auto duration = FormatDuration(event->GetDuration());
			SetCellText(i, EventsViewColumns::Name, event->GetName());
			SetCellText(i, EventsViewColumns::Duration, duration);
			SetCellText(i, EventsViewColumns::Color, color);
			SetCellText(i, EventsViewColumns::Usage, usage);
			SetCellText(i, EventsViewColumns::MaxCount, maxCount);
		}
	}
	else
	{
		DeleteColumn(colMapping.colIndex);
		for (auto&& c : columnMappings)
		{
			if (c.second.column != colMapping.column && c.second.colIndex >= colMapping.colIndex)
			{
				--c.second.colIndex;
			}
		}
	}
}
void CStockEventsListCtrl::AddColumn(EventsViewColumns col, LPCTSTR caption, int format)
{
	Column colMapping(col,addingColumnIndex++,true, 90, caption, format);
	InsertColumn(colMapping.colIndex, colMapping.caption, colMapping.format);
	columnMappings[col] = colMapping;
}

void CFileViewToolBar::AdjustLocations()
{
	CMFCToolBar::AdjustLocations();

	if (GetSafeHwnd())
	{
		int iCount(GetCount());
		if (iCount)
		{
			CRect crClient(0, 0, 0, 0);
			GetClientRect(crClient);

			CMFCToolBarButton* pcButton(GetButton(iCount - 1));
			if (pcButton)
			{
				CRect crPos(pcButton->Rect());
				if (crClient.right > crPos.right)
				{
					crPos.OffsetRect(crClient.right - crPos.right, 0);
					pcButton->SetRect(crPos);
					UpdateTooltips();
				}
			}
		}
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
	ON_WM_DESTROY()
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
	//ON_UPDATE_COMMAND_UI(ID_SHOW_EVENT_COLUMS, OnUpdateShowColumnsButton)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_EVENT_LIST, OnEventListItemChanged)
	//ON_NOTIFY(LVN_COLUMNCLICK, IDC_EVENT_LIST, OnEventListColumnClick)
	ON_MESSAGE(WM_EVENT_OBJECT_SELECTED, OnEventObjectSelected)
	ON_COMMAND_RANGE(ID_SHOWCOLS_DURATION, ID_SHOWCOLS_MAXCOUNT, OnShowHideColumn)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SHOWCOLS_DURATION, ID_SHOWCOLS_MAXCOUNT, OnUpdateShowHideColumnsPopup)
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
	m_wndEventListView.AddColumn(EventsViewColumns::Name, strTemp, LVCFMT_LEFT);

	bNameValid = strTemp.LoadString(IDS_STOCK_EVENT_DURATION);
	ASSERT(bNameValid);
	m_wndEventListView.AddColumn(EventsViewColumns::Duration, strTemp, LVCFMT_LEFT);

	bNameValid = strTemp.LoadString(IDS_STOCK_EVENT_USAGE);
	ASSERT(bNameValid);
	m_wndEventListView.AddColumn(EventsViewColumns::Usage, strTemp, LVCFMT_LEFT);

	bNameValid = strTemp.LoadString(IDS_STOCK_EVENT_MAXCOUNT);
	ASSERT(bNameValid);
	m_wndEventListView.AddColumn(EventsViewColumns::MaxCount, strTemp, LVCFMT_LEFT);

	bNameValid = strTemp.LoadString(IDS_STOCK_EVENT_COLOR);
	ASSERT(bNameValid);
	m_wndEventListView.AddColumn(EventsViewColumns::Color, strTemp, LVCFMT_LEFT);

	LoadListSettings();

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

	CMenu menuSort;
	menuSort.LoadMenu(IDR_SHOW_COLUMNS_MENU);

	m_wndToolBar.ReplaceButton(ID_SHOW_EVENT_COLUMS, CMFCToolBarMenuButton((UINT)-1,menuSort.GetSubMenu(0)->GetSafeHmenu(),3));
	
	CMFCToolBarMenuButton* pButton = DYNAMIC_DOWNCAST(CMFCToolBarMenuButton, m_wndToolBar.GetButton(m_wndToolBar.GetCount()-1));

	if (pButton != NULL)
	{
		pButton->m_bText = FALSE;
		pButton->m_bImage = TRUE;		
		pButton->SetMessageWnd(this);
	}

	
	AdjustLayout();

	m_wndEventListView.SetSortColumn((int)EventsViewColumns::Name, true);

	return 0;
}
void CStockEventView::OnDestroy()
{
	SaveListSettings();
	CViewDockingPane::OnDestroy();
}
void CStockEventView::OnUpdateShowColumnsButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
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
void CStockEventView::LoadListSettings()
{
	m_wndEventListView.SetColWidth(EventsViewColumns::Name, theApp.GetProfileInt(_T("Settings\\StockEventView"), _T("NameColumnWidth"), 90));
	m_wndEventListView.SetColumnVisible(EventsViewColumns::Name, theApp.GetProfileInt(_T("Settings\\StockEventView"), _T("NameColumnVisible"), true));
	m_wndEventListView.SetColWidth(EventsViewColumns::Color, theApp.GetProfileInt(_T("Settings\\StockEventView"), _T("ColorColumnWidth"), 90));
	m_wndEventListView.SetColumnVisible(EventsViewColumns::Color, theApp.GetProfileInt(_T("Settings\\StockEventView"), _T("ColorColumnVisible"), true));
	m_wndEventListView.SetColWidth(EventsViewColumns::Duration, theApp.GetProfileInt(_T("Settings\\StockEventView"), _T("DurationColumnWidth"), 90));
	m_wndEventListView.SetColumnVisible(EventsViewColumns::Duration, theApp.GetProfileInt(_T("Settings\\StockEventView"), _T("DurationColumnVisible"), true));
	m_wndEventListView.SetColWidth(EventsViewColumns::Usage, theApp.GetProfileInt(_T("Settings\\StockEventView"), _T("UsageColumnWidth"), 50));
	m_wndEventListView.SetColumnVisible(EventsViewColumns::Usage, theApp.GetProfileInt(_T("Settings\\StockEventView"), _T("UsageColumnVisible"), true));
	m_wndEventListView.SetColWidth(EventsViewColumns::MaxCount, theApp.GetProfileInt(_T("Settings\\StockEventView"), _T("MaxCountColumnWidth"), 50));
	m_wndEventListView.SetColumnVisible(EventsViewColumns::MaxCount, theApp.GetProfileInt(_T("Settings\\StockEventView"), _T("MaxCountColumnVisible"), true));


}
void CStockEventView::SaveListSettings()
{
	theApp.WriteProfileInt(_T("Settings\\StockEventView"),_T("NameColumnWidth"), m_wndEventListView.GetColWidth(EventsViewColumns::Name));
	theApp.WriteProfileInt(_T("Settings\\StockEventView"), _T("NameColumnVisible"), m_wndEventListView.IsColumnVisible(EventsViewColumns::Name));
	theApp.WriteProfileInt(_T("Settings\\StockEventView"), _T("ColorColumnWidth"), m_wndEventListView.GetColWidth(EventsViewColumns::Color));
	theApp.WriteProfileInt(_T("Settings\\StockEventView"), _T("ColorColumnVisible"), m_wndEventListView.IsColumnVisible(EventsViewColumns::Color));
	theApp.WriteProfileInt(_T("Settings\\StockEventView"), _T("DurationColumnWidth"), m_wndEventListView.GetColWidth(EventsViewColumns::Duration));
	theApp.WriteProfileInt(_T("Settings\\StockEventView"), _T("DurationColumnVisible"), m_wndEventListView.IsColumnVisible(EventsViewColumns::Duration));
	theApp.WriteProfileInt(_T("Settings\\StockEventView"), _T("UsageColumnWidth"), m_wndEventListView.GetColWidth(EventsViewColumns::Usage));
	theApp.WriteProfileInt(_T("Settings\\StockEventView"), _T("UsageColumnVisible"), m_wndEventListView.IsColumnVisible(EventsViewColumns::Usage));
	theApp.WriteProfileInt(_T("Settings\\StockEventView"), _T("MaxCountColumnWidth"), m_wndEventListView.GetColWidth(EventsViewColumns::MaxCount));
	theApp.WriteProfileInt(_T("Settings\\StockEventView"), _T("MaxCountColumnVisible"), m_wndEventListView.IsColumnVisible(EventsViewColumns::MaxCount));
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
			CString maxCount;
			maxCount.Format(_T("%d"), GetEventMaxCount(event));
			m_wndEventListView.SetCellText(listInfo->item.iItem, EventsViewColumns::Duration, duration);
			m_wndEventListView.SetCellText(listInfo->item.iItem, EventsViewColumns::Color, color);
			m_wndEventListView.SetCellText(listInfo->item.iItem, EventsViewColumns::Usage, usage);
			m_wndEventListView.SetCellText(listInfo->item.iItem, EventsViewColumns::MaxCount, maxCount);
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
			CString maxCount;
			maxCount.Format(_T("%d"), GetEventMaxCount(ev.get()));

			m_wndEventListView.SetCellText(insertedIndex, EventsViewColumns::Duration,  duration);
			m_wndEventListView.SetCellText(insertedIndex, EventsViewColumns::Color,  color);
			m_wndEventListView.SetCellText(insertedIndex, EventsViewColumns::Usage,  usage);
			m_wndEventListView.SetCellText(insertedIndex, EventsViewColumns::MaxCount, maxCount);
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
			CString maxCount;
			maxCount.Format(_T("%d"), GetEventMaxCount(event));
			m_wndEventListView.SetCellText(i, EventsViewColumns::Name, event->GetName());
			m_wndEventListView.SetCellText(i, EventsViewColumns::Duration, duration);
			m_wndEventListView.SetCellText(i, EventsViewColumns::Color, color);
			m_wndEventListView.SetCellText(i, EventsViewColumns::Usage, usage);
			m_wndEventListView.SetCellText(i, EventsViewColumns::MaxCount, maxCount);
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
}

void CStockEventView::OnBeginDragEvent(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int dragItemIndex = pNMListView->iItem;
	CScheduleStockEvent* event = (CScheduleStockEvent*)m_wndEventListView.GetItemData(dragItemIndex);
	if (!event->CanScheduleGlobally())
	{
		*pResult = 1;
		return;
	}
	//// Create a drag image
	POINT pt = {0,0};
	CImageList*  dragImageList = m_wndEventListView.CreateDragImage(dragItemIndex, &pt);
	ASSERT(dragImageList); //make sure it was created
	GetMainFrame()->StartDraggingStockEvent(event->GetId(), dragImageList, pNMListView->ptAction);		

	*pResult = 0;
}


void CStockEventView::OnAddEvent()
{
	int index = m_wndEventListView.GetItemCount();
	CString newEventName(_T("New Event"));
	auto insertedIndex = m_wndEventListView.InsertItem(LVIF_TEXT | LVIF_STATE | LVIF_PARAM, index, newEventName, 0, 0, 0, 0);
	m_wndEventListView.SetCellText(insertedIndex, EventsViewColumns::Duration, _T(""));
	m_wndEventListView.SetCellText(insertedIndex, EventsViewColumns::Color, _T(""));
	m_wndEventListView.SetCellText(insertedIndex, EventsViewColumns::Usage, _T(""));
	m_wndEventListView.SetCellText(insertedIndex, EventsViewColumns::MaxCount, _T(""));

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

void CStockEventView::OnShowHideColumn(UINT id)
{
	EventsViewColumns col = EventsViewColumns::ColumnsCount;
	switch (id)
	{
	case ID_SHOWCOLS_DURATION:
		col = EventsViewColumns::Duration;
		break;
	case ID_SHOWCOLS_USAGE:
		col = EventsViewColumns::Usage;
		break;
	case ID_SHOWCOLS_COLOR:
		col = EventsViewColumns::Color;
		break;
	case ID_SHOWCOLS_MAXCOUNT:
		col = EventsViewColumns::MaxCount;
		break;
	default:
		break;
	}

	m_wndEventListView.SetColumnVisible(col, !m_wndEventListView.IsColumnVisible(col));
}
void CStockEventView::OnUpdateShowHideColumnsPopup(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);

	EventsViewColumns col = EventsViewColumns::ColumnsCount;
	switch (pCmdUI->m_nID)
	{
	case ID_SHOWCOLS_DURATION:
		col = EventsViewColumns::Duration;
		break;
	case ID_SHOWCOLS_USAGE:
		col = EventsViewColumns::Usage;
		break;
	case ID_SHOWCOLS_COLOR:
		col = EventsViewColumns::Color;
		break;
	case ID_SHOWCOLS_MAXCOUNT:
		col = EventsViewColumns::MaxCount;
		break;
	default:
		break;
	}
	pCmdUI->SetCheck(m_wndEventListView.IsColumnVisible(col));
}
