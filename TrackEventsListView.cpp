#include "pch.h"
#include "framework.h"
#include "TrackEventsListView.h"
#include "Resource.h"

#include <chrono>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CTrackEventsListView::CTrackEventsListView() noexcept
{

}
CTrackEventsListView::~CTrackEventsListView()
{

}
IMPLEMENT_DYNAMIC(CTrackEventsListView, CViewDockingPane)

BEGIN_MESSAGE_MAP(CTrackEventsListView, CViewDockingPane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_MESSAGE(WM_TRACK_OBJECT_SELECTED, OnTrackObjectSelected)
	ON_MESSAGE(WM_EVENT_OBJECT_SELECTED, OnEventObjectSelected)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_EVENTS_CSV, OnUpdateCommandToolbarButtons)
	ON_COMMAND(ID_EXPORT_EVENTS_CSV, OnExportToCSV)
END_MESSAGE_MAP()


int CTrackEventsListView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create view:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL;

	if (!m_wndEventsList.Create(dwViewStyle, rectDummy, this, IDC_TRACK_EVENT_LIST))
	{
		TRACE0("Failed to create file view\n");
		return -1;      // fail to create
	}
	m_wndEventsList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CString strTemp;
	BOOL bNameValid = strTemp.LoadString(IDS_TRACK_EVENT_NAME);
	ASSERT(bNameValid);
	m_wndEventsList.InsertColumn(0, strTemp, LVCFMT_RIGHT);
	bNameValid = strTemp.LoadString(IDS_TRACK_EVENT_START_DATE);
	ASSERT(bNameValid);
	m_wndEventsList.InsertColumn(1, strTemp, LVCFMT_CENTER);
	bNameValid = strTemp.LoadString(IDS_TRACK_EVENT_END_DATE);
	ASSERT(bNameValid);
	m_wndEventsList.InsertColumn(2, strTemp, LVCFMT_CENTER);

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_TRACK_EVENTS_LIST);
	m_wndToolBar.LoadToolBar(IDR_TRACK_EVENTS_LIST, 0, 0, TRUE /* Is locked */);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	
	AdjustLayout();

	return 0;
}

void CTrackEventsListView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CTrackEventsListView::OnInitialUpdate()
{
	CSchedulerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
}
void CTrackEventsListView::OnUpdate(const LPARAM lHint)
{
	CSchedulerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	auto track = pDoc->GetTrack(trackId);
	if (track)
	{
		m_wndEventsList.DeleteAllItems();
		FillEventsList(track);
	}
}

LRESULT CTrackEventsListView::OnEventObjectSelected(WPARAM wParam, LPARAM lParam)
{
	CTrackEventsListView* sender = reinterpret_cast<CTrackEventsListView*>(lParam);
	if (sender && sender == this) return (LRESULT)FALSE;
	m_wndEventsList.DeleteAllItems();
	int eventId = (int)wParam;
	CSchedulerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	auto event = pDoc->GetEvent(eventId);
	if(!event) return (LRESULT)FALSE;
	auto track = pDoc->GetTrack(event->GetTrackId());
	if (!track) return (LRESULT)FALSE;
	trackId = track->GetId();
	FillEventsList(track);
	return (LRESULT)TRUE;
}
void CTrackEventsListView::FillEventsList(CScheduleTrack* track)
{
	CSchedulerDoc* pDoc = GetDocument();
	auto startTime = track->GetStartTime();
	std::chrono::minutes utcOffset(pDoc->GetUTCOffsetMinutes());
	std::chrono::local_seconds start{ std::chrono::duration_cast<std::chrono::seconds>(startTime.time_since_epoch()) - utcOffset };
	int index = 0;
	for (const auto& ev : track->GetEvents())
	{
		CString startText(std::format("{:%Y %b %d %R}", start).c_str());
		auto end = start + ev->GetDuration();
		CString endText(std::format("{:%Y %b %d %R}", end).c_str());
		start = end;
		auto insertedIndex = m_wndEventsList.InsertItem(index++, ev->GetName());
		m_wndEventsList.SetItem(insertedIndex, 1, LVIF_TEXT, startText, 0, 0, 0, 0);
		m_wndEventsList.SetItem(insertedIndex, 2, LVIF_TEXT, endText, 0, 0, 0, 0);
	}
}
LRESULT CTrackEventsListView::OnTrackObjectSelected(WPARAM wParam, LPARAM lParam)
{
	CTrackEventsListView* sender = reinterpret_cast<CTrackEventsListView*>(lParam);
	if (sender && sender == this) return (LRESULT)FALSE;

	m_wndEventsList.DeleteAllItems();
	trackId = (int)wParam;
	CSchedulerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	auto track = pDoc->GetTrack(trackId);
	if (!track) return (LRESULT)FALSE;

	FillEventsList(track);

	return (LRESULT)TRUE;
}

void CTrackEventsListView::AdjustLayout()
{
	if (GetSafeHwnd() == nullptr)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;
	
	m_wndToolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndEventsList.SetWindowPos(nullptr, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);

	m_wndEventsList.SetColumnWidth(0, rectClient.Width()/3);
	m_wndEventsList.SetColumnWidth(1, rectClient.Width() / 3);
	m_wndEventsList.SetColumnWidth(2, rectClient.Width() / 3);
}

void CTrackEventsListView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rectTree;
	m_wndEventsList.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CTrackEventsListView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndEventsList.SetFocus();
}

void CTrackEventsListView::OnChangeVisualStyle()
{
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(IDB_TRACK_EVENTS_LIST_24, 0, 0, TRUE /* Locked */);

}
void CTrackEventsListView::OnUpdateCommandToolbarButtons(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_wndEventsList.GetSafeHwnd() != nullptr && m_wndEventsList.GetItemCount() > 0);
}
void CTrackEventsListView::OnExportToCSV()
{
	if (m_wndEventsList.GetItemCount() <= 0) return;
	char strFilter[] = { "CSV Files (*.csv)|*.csv|" };
	CFileDialog fileSaveDlg(FALSE, CString(".csv"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, CString(strFilter));

	if (fileSaveDlg.DoModal() == IDOK)
	{
		auto file = fileSaveDlg.GetPathName();		
		CSchedulerDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);

		auto track = pDoc->GetTrack(trackId);
		if (!track) return;

		TRY
		{
			CStdioFile cFile(file, CFile::modeWrite| CFile::modeCreate);			
			cFile.WriteString(_T("TRACK, EVENT, START, END\n"));			
			auto startTime = pDoc->GetStartTime();
			std::chrono::minutes utcOffset(pDoc->GetUTCOffsetMinutes());
			std::chrono::local_seconds start{ std::chrono::duration_cast<std::chrono::seconds>(startTime.time_since_epoch()) - utcOffset };
			int index = 0;
			for (const auto& ev : track->GetEvents())
			{
				CString startText(std::format("{:%Y %b %d %R}", start).c_str());
				auto end = start + ev->GetDuration();
				CString endText(std::format("{:%Y %b %d %R}", end).c_str());
				start = end;				
				cFile.WriteString(track->GetName());
				cFile.WriteString(_T(","));
				cFile.WriteString(ev->GetName());
				cFile.WriteString(_T(","));
				cFile.WriteString(startText);
				cFile.WriteString(_T(","));
				cFile.WriteString(endText);
				cFile.WriteString(_T("\n"));
			}

		}
		CATCH_ALL(e)
		{
			MessageBox(_T("Cannot write to file"),_T("Error"), MB_OK| MB_ICONERROR);
		}
		END_CATCH_ALL


	}
}