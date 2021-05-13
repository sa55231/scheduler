#include "pch.h"
#include "framework.h"
#include "TrackEventsListView.h"
#include "Resource.h"

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

	OnChangeVisualStyle();


	// Fill in some static tree view data (dummy code, nothing magic here)	
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
}

LRESULT CTrackEventsListView::OnTrackObjectSelected(WPARAM wParam, LPARAM lParam)
{
	CTrackEventsListView* sender = reinterpret_cast<CTrackEventsListView*>(lParam);
	if (sender && sender == this) return (LRESULT)FALSE;

	m_wndEventsList.DeleteAllItems();

	int trackId = (int)wParam;
	auto track = GetDocument()->GetTrack(trackId);
	if (!track) return (LRESULT)FALSE;

	CSchedulerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	auto startTime = pDoc->GetStartTime();
	std::chrono::minutes utcOffset(pDoc->GetUTCOffsetMinutes());
	date::local_seconds start{ std::chrono::duration_cast<std::chrono::seconds>(startTime.time_since_epoch()) - utcOffset };
	std::locale loc;
	int index = 0;
	for (const auto& ev : track->GetEvents())
	{
		CString startText(date::format(loc, "%Y %b %d %R", start).c_str());
		auto end = start + ev->GetDuration();
		CString endText(date::format(loc, "%Y %b %d %R", end).c_str());
		start = end;
		auto insertedIndex = m_wndEventsList.InsertItem(index++, ev->GetName());
		m_wndEventsList.SetItem(insertedIndex, 1, LVIF_TEXT, startText, 0, 0, 0, 0);
		m_wndEventsList.SetItem(insertedIndex, 2, LVIF_TEXT, endText, 0, 0, 0, 0);
		
	}

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
	
	m_wndEventsList.SetWindowPos(nullptr, rectClient.left + 1, rectClient.top + 1, 
		rectClient.Width() - 2, rectClient.Height() - 2, SWP_NOACTIVATE | SWP_NOZORDER);

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

}
