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
