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

// MainFrm.cpp : implementation of the CMainFrame class
//

#include "pch.h"
#include "framework.h"
#include "scheduler.h"

#include "MainFrm.h"

#include "schedulerView.h"
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_COMMAND(ID_VIEW_CAPTION_BAR, &CMainFrame::OnViewCaptionBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CAPTION_BAR, &CMainFrame::OnUpdateViewCaptionBar)
	ON_COMMAND(ID_TOOLS_OPTIONS, &CMainFrame::OnOptions)
	ON_COMMAND(ID_VIEW_EVENTVIEW, &CMainFrame::OnViewEventView)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EVENTVIEW, &CMainFrame::OnUpdateViewEventView)
	ON_COMMAND(ID_VIEW_TRACKVIEW, &CMainFrame::OnViewTrackView)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TRACKVIEW, &CMainFrame::OnUpdateViewTrackView)
	ON_COMMAND(ID_VIEW_OUTPUTWND, &CMainFrame::OnViewOutputWindow)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OUTPUTWND, &CMainFrame::OnUpdateViewOutputWindow)
	ON_COMMAND(ID_VIEW_PROPERTIESWND, &CMainFrame::OnViewPropertiesWindow)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROPERTIESWND, &CMainFrame::OnUpdateViewPropertiesWindow)
	ON_COMMAND(ID_FILE_PRINT, &CMainFrame::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CMainFrame::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMainFrame::OnFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, &CMainFrame::OnUpdateFilePrintPreview)
	ON_WM_SETTINGCHANGE()
	ON_MESSAGE(WM_EVENT_OBJECT_SELECTED, &CMainFrame::OnEventObjectSelected)
	ON_MESSAGE(WM_TRACK_OBJECT_SELECTED, &CMainFrame::OnTrackObjectSelected)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_UPDATE_COMMAND_UI(ID_DOCUMENT_SETTINGS_START_DATE, &CMainFrame::OnUpdateSetStartDate)
	ON_UPDATE_COMMAND_UI(ID_DOCUMENT_SETTINGS_START_TIME, &CMainFrame::OnUpdateSetStartTime)
	ON_UPDATE_COMMAND_UI(ID_DOCUMENT_SETTINGS_UTC_OFFFSET, &CMainFrame::OnUpdateSetUTCOffset)
	ON_MESSAGE(WM_DOCUMENT_LOADED, &CMainFrame::OnDocumentLoaded)
	ON_NOTIFY(DTN_DATETIMECHANGE, ID_DOCUMENT_SETTINGS_START_DATE, &CMainFrame::OnStartTimeChange)
	ON_NOTIFY(DTN_DATETIMECHANGE, ID_DOCUMENT_SETTINGS_START_TIME, &CMainFrame::OnStartTimeChange)
	ON_COMMAND(ID_DOCUMENT_SETTINGS_UTC_OFFFSET, &CMainFrame::OnSetUTCOffset)
	//ON_REGISTERED_MESSAGE(AFX_WM_ON_RIBBON_CUSTOMIZE, OnRibbonCustomize)
END_MESSAGE_MAP()

// CMainFrame construction/destruction

namespace
{
	//TODO: Get the list from Windows itself, as surely it has them.
	// for now it will have to do, as while i found a c# way of doing it
	// there doesn't seem to be an easy c++ way.
	const std::vector<std::pair<CString, int>> utcOffsets = 
	{
		{_T("UTC-12:00"),12 * 60},
		{_T("UTC-11:00"),11 * 60},
		{_T("UTC-10:00"),10 * 60},
		{_T("UTC-09:30"),(int)(9.5 * 60)},
		{_T("UTC-09:00"),9 * 60},
		{_T("UTC-08:00"),8 * 60},
		{_T("UTC-07:00"),7 * 60},
		{_T("UTC-06:00"),6 * 60},
		{_T("UTC-05:00"),5 * 60},
		{_T("UTC-04:00"),4 * 60},
		{_T("UTC-03:30"),(int)(3.5 * 60)},
		{_T("UTC-03:00"),3 * 60},
		{_T("UTC-02:00"),2 * 60},
		{_T("UTC-01:00"),1 * 60},
		{_T("UTC+00:00"),0 * 60},
		{_T("UTC+01:00"),-1 * 60},
		{_T("UTC+02:00"),-2 * 60},
		{_T("UTC+03:00"),-3 * 60},
		{_T("UTC+03:30"),-(int)(3.5 * 60)},
		{_T("UTC+04:00"),4 * 60},
		{_T("UTC+04:30"),-(int)(4.5 * 60)},
		{_T("UTC+05:00"),-5 * 60},
		{_T("UTC+05:30"),-(int)(5.5 * 60)},
		{_T("UTC+05:45"),-(int)(5.75 * 60)},
		{_T("UTC+06:00"),-6 * 60},
		{_T("UTC+06:30"),-(int)(6.5 * 60)},
		{_T("UTC+07:00"),-7 * 60},
		{_T("UTC+08:00"),-8 * 60},
		{_T("UTC+08:45"),-(int)(8.75 * 60)},
		{_T("UTC+09:00"),-9 * 60},
		{_T("UTC+09:30"),-(int)(9.5 * 60)},
		{_T("UTC+10:00"),-10 * 60},
		{_T("UTC+10:30"),-(int)(10.5 * 60)},
		{_T("UTC+11:00"),-11 * 60},
		{_T("UTC+12:00"),-12 * 60},
		{_T("UTC+12:45"),-(int)(12.75 * 60)},
		{_T("UTC+13:00"),-13 * 60},
		{_T("UTC+14:00"),-14 * 60},
	};
}


CMainFrame::CMainFrame() noexcept
{
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_OFF_2007_BLUE);
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

	auto homeCategory = m_wndRibbonBar.GetCategory(1);
	CMFCRibbonPanel* documentTimePanel = homeCategory->AddPanel(_T("Start Time\nzc"));
	auto startDateButton = new CRibbonDateTimeControl(ID_DOCUMENT_SETTINGS_START_DATE, _T("Start date:"));
	auto startTimeButton = new CRibbonDateTimeControl(ID_DOCUMENT_SETTINGS_START_TIME, _T("Start time:"), true);
	auto utcOffsetsCombo = new CMFCRibbonComboBox(ID_DOCUMENT_SETTINGS_UTC_OFFFSET,FALSE,-1,_T("UTC Offset:"));
	for (const auto& offset : utcOffsets)
	{
		utcOffsetsCombo->AddItem(offset.first,(DWORD_PTR)&offset.second);
	}
	documentTimePanel->Add(startDateButton);
	documentTimePanel->Add(startTimeButton);
	documentTimePanel->Add(utcOffsetsCombo);
	
	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	CString strTitlePane1;
	CString strTitlePane2;
	bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
	ASSERT(bNameValid);
	bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
	ASSERT(bNameValid);
	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE), strTitlePane1);
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, strTitlePane2, TRUE), strTitlePane2);

	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// Navigation pane will be created at left, so temporary disable docking at the left side:
	EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM | CBRS_ALIGN_RIGHT);

	// Create a caption bar:
	if (!CreateCaptionBar())
	{
		TRACE0("Failed to create caption bar\n");
		return -1;      // fail to create
	}

	// Outlook bar is created and docking on the left side should be allowed.
	EnableDocking(CBRS_ALIGN_LEFT);
	EnableAutoHidePanes(CBRS_ALIGN_RIGHT);

	// Load menu item image (not placed on any standard toolbars):
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	// create docking windows
	if (!CreateDockingWindows())
	{
		TRACE0("Failed to create docking windows\n");
		return -1;
	}

	m_wndStockEventView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndTrackView.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndStockEventView);
	CDockablePane* pTabbedBar = nullptr;
	m_wndTrackView.AttachToTabWnd(&m_wndStockEventView, DM_SHOW, TRUE, &pTabbedBar);
	
	m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndProperties);
	//by default put the properties under the tabbed pane
	m_wndProperties.DockToWindow(pTabbedBar, CBRS_ALIGN_BOTTOM);

	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutput);

	m_wndTrackEventsListView.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndTrackEventsListView);

	// set the visual manager and style based on persisted value
	OnApplicationLook(theApp.m_nAppLook);
	
	return 0;
}


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs


	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;

	// Create class view
	CString strClassView;
	bNameValid = strClassView.LoadString(IDS_TRACKS_VIEW);
	ASSERT(bNameValid);
	if (!m_wndTrackView.Create(strClassView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_TRACKVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Class View window\n");
		return FALSE; // failed to create
	}

	// Create file view
	CString strFileView;
	bNameValid = strFileView.LoadString(IDS_EVENT_VIEW);
	ASSERT(bNameValid);
	if (!m_wndStockEventView.Create(strFileView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_EVENTVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create File View window\n");
		return FALSE; // failed to create
	}

	// Create properties window
	CString strPropertiesWnd;
	bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);
	if (!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 200, 200, 400), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Properties window\n");
		return FALSE; // failed to create
	}

	// Create track events window
	CString strTrackEventsWnd;
	bNameValid = strTrackEventsWnd.LoadString(IDS_VIEW_TRACKEVENTS_WND);
	ASSERT(bNameValid);
	if (!m_wndTrackEventsListView.Create(strTrackEventsWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_TRACKEVENTSWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Properties window\n");
		return FALSE; // failed to create
	}


	// Create output window
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
	ASSERT(bNameValid);
	if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Output window\n");
		return FALSE; // failed to create
	}


	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hFileViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_EVENT_ICON_HC : IDI_EVENT_ICON), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndStockEventView.SetIcon(hFileViewIcon, FALSE);

	HICON hClassViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_CLASS_VIEW_HC : IDI_CLASS_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndTrackView.SetIcon(hClassViewIcon, FALSE);

	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

	HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);

}

BOOL CMainFrame::CreateCaptionBar()
{
	if (!m_wndCaptionBar.Create(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, this, ID_VIEW_CAPTION_BAR, -1, TRUE))
	{
		TRACE0("Failed to create caption bar\n");
		return FALSE;
	}

	BOOL bNameValid;

	CString strTemp, strTemp2;
	bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButton(strTemp, ID_TOOLS_OPTIONS, CMFCCaptionBar::ALIGN_LEFT, FALSE);
	bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON_TIP);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButtonToolTip(strTemp);

	bNameValid = strTemp.LoadString(IDS_CAPTION_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetText(strTemp, CMFCCaptionBar::ALIGN_LEFT);

	m_wndCaptionBar.SetBitmap(IDB_INFO, RGB(255, 255, 255), FALSE, CMFCCaptionBar::ALIGN_LEFT);
	bNameValid = strTemp.LoadString(IDS_CAPTION_IMAGE_TIP);
	ASSERT(bNameValid);
	bNameValid = strTemp2.LoadString(IDS_CAPTION_IMAGE_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetImageToolTip(strTemp, strTemp2);

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG

void CMainFrame::InitializeSecondaryViews(CSchedulerDoc* doc)
{
	m_wndStockEventView.Initialize(doc);
	m_wndProperties.Initialize(doc);
	m_wndTrackEventsListView.Initialize(doc);
}

void CMainFrame::UpdateSecondaryViews(LPARAM lHint)
{
	m_wndStockEventView.Update(lHint);
	m_wndProperties.Update(lHint);
	m_wndTrackEventsListView.Update(lHint);
}

void CMainFrame::StartDraggingStockEvent(int dragItemIndex, CImageList* imageList, CPoint point)
{
	int nOffset = -10; //offset in pixels for drag image (positive is up and to the left; neg is down and to the right)
	dragImageList = imageList;
	dragImageList->BeginDrag(0, CPoint(nOffset, nOffset - 4));
	dragImageList->DragEnter(GetDesktopWindow(), point);
	this->dragItemIndex = dragItemIndex;
	dragging = true;	//we are in a drag and drop operation
	//m_nDropIndex = -1;	//we don't have a drop index yet
	//m_pDragList = &m_listL; //make note of which list we are dragging from
	//m_pDropWnd = &m_listL;	//at present the drag list is the drop list

	// Capture all mouse messages
	SetCapture();

}

void CMainFrame::OnMouseMove(UINT nFlags, CPoint point)
{
	if (dragging)
	{
		CPoint pt(point);	//get our current mouse coordinates
		ClientToScreen(&pt); //convert to screen coordinates
		dragImageList->DragMove(pt); //move the drag image to those coordinates
		// Unlock window updates (this allows the dragging image to be shown smoothly)
		dragImageList->DragShowNolock(false);

		CWnd* pDropWnd = WindowFromPoint(pt);
		ASSERT(pDropWnd); //make sure we have a window

		if (pDropWnd->IsKindOf(RUNTIME_CLASS(CSchedulerView)))
		{
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			CSchedulerView* view = (CSchedulerView*)pDropWnd;
			view->DraggingEventAtPoint(dragItemIndex, pt);
		}
		else
		{
			SetCursor(LoadCursor(NULL, IDC_NO));
		}

		dragImageList->DragShowNolock(true);
	}

	CFrameWndEx::OnMouseMove(nFlags, point);
}

void CMainFrame::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (dragging)
	{
		ReleaseCapture();
		dragImageList->DragLeave(GetDesktopWindow());
		dragImageList->EndDrag();
		delete dragImageList; //must delete it because it was created at the beginning of the drag
		CPoint pt(point); //Get current mouse coordinates
		ClientToScreen(&pt); //Convert to screen coordinates
		// Get the CWnd pointer of the window that is under the mouse cursor
		CWnd* pDropWnd = WindowFromPoint(pt);
		ASSERT(pDropWnd); //make sure we have a window pointer
		// If window is CListCtrl, we perform the drop
		if (pDropWnd->IsKindOf(RUNTIME_CLASS(CSchedulerView)))
		{
			CSchedulerView* view = (CSchedulerView*)pDropWnd;
			view->AddEventAtPoint(dragItemIndex, pt);
			//m_pDropList = (CListCtrl*)pDropWnd; //Set pointer to the list we are dropping on
			//DropItemOnList(m_pDragList, m_pDropList); //Call routine to perform the actual drop

		}

	}
	dragItemIndex = -1;
	dragging = false;
}
// CMainFrame message handlers

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(TRUE);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
	}

	m_wndOutput.UpdateFonts();
	RedrawWindow(nullptr, nullptr, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

void CMainFrame::OnViewCaptionBar()
{
	m_wndCaptionBar.ShowWindow(m_wndCaptionBar.IsVisible() ? SW_HIDE : SW_SHOW);
	RecalcLayout(FALSE);
}

void CMainFrame::OnUpdateViewCaptionBar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndCaptionBar.IsVisible());
}

void CMainFrame::OnOptions()
{
	CMFCRibbonCustomizeDialog *pOptionsDlg = new CMFCRibbonCustomizeDialog(this, &m_wndRibbonBar);
	ASSERT(pOptionsDlg != nullptr);

	pOptionsDlg->DoModal();
	delete pOptionsDlg;
}

void CMainFrame::OnViewEventView()
{
	// Show or activate the pane, depending on current state.  The
	// pane can only be closed via the [x] button on the pane frame.
	m_wndStockEventView.ShowPane(TRUE, FALSE, TRUE);
	m_wndStockEventView.SetFocus();
}

void CMainFrame::OnUpdateViewEventView(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnViewTrackView()
{
	// Show or activate the pane, depending on current state.  The
	// pane can only be closed via the [x] button on the pane frame.
	m_wndTrackView.ShowPane(TRUE, FALSE, TRUE);
	m_wndTrackView.SetFocus();
}

void CMainFrame::OnUpdateViewTrackView(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnViewOutputWindow()
{
	// Show or activate the pane, depending on current state.  The
	// pane can only be closed via the [x] button on the pane frame.
	m_wndOutput.ShowPane(TRUE, FALSE, TRUE);
	m_wndOutput.SetFocus();
}

void CMainFrame::OnUpdateViewOutputWindow(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnViewPropertiesWindow()
{
	// Show or activate the pane, depending on current state.  The
	// pane can only be closed via the [x] button on the pane frame.
	m_wndProperties.ShowPane(TRUE, FALSE, TRUE);
	m_wndProperties.SetFocus();
}

void CMainFrame::OnUpdateViewPropertiesWindow(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}


void CMainFrame::OnFilePrint()
{
	if (IsPrintPreview())
	{
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_PRINT);
	}
}

void CMainFrame::OnFilePrintPreview()
{
	if (IsPrintPreview())
	{
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_CLOSE);  // force Print Preview mode closed
	}
}

void CMainFrame::OnUpdateFilePrintPreview(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(IsPrintPreview());
}

void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CFrameWndEx::OnSettingChange(uFlags, lpszSection);
	m_wndOutput.UpdateFonts();
}

LRESULT CMainFrame::OnEventObjectSelected(WPARAM wparam, LPARAM lparam)
{
	m_wndProperties.PostMessage(WM_EVENT_OBJECT_SELECTED, wparam, lparam);
	m_wndStockEventView.PostMessage(WM_EVENT_OBJECT_SELECTED, wparam, lparam);
	return 0;
}

LRESULT CMainFrame::OnTrackObjectSelected(WPARAM wparam, LPARAM lparam)
{
	m_wndTrackEventsListView.PostMessage(WM_TRACK_OBJECT_SELECTED, wparam, lparam);
	return 0;
}

void CMainFrame::OnUpdateSetStartTime(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
}
void CMainFrame::OnUpdateSetStartDate(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
}
void CMainFrame::OnUpdateSetUTCOffset(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
}
LRESULT CMainFrame::OnDocumentLoaded(WPARAM wparam, LPARAM lparam)
{
	//called by CSchedulerDoc when it initialized or when it loaded
	CSchedulerDoc* pDoc = reinterpret_cast<CSchedulerDoc*>(lparam);
	ASSERT_VALID(pDoc);
	if (!pDoc) return (LRESULT)FALSE;

	auto utcOffset  = pDoc->GetUTCOffsetMinutes();
	auto startTime = pDoc->GetStartTime();
	CTime time(std::chrono::duration_cast<std::chrono::seconds>(startTime.time_since_epoch()).count());

	CRibbonDateTimeControl* startDateButton = DYNAMIC_DOWNCAST(CRibbonDateTimeControl, GetRibbonBar()->FindByID(ID_DOCUMENT_SETTINGS_START_DATE));
	CRibbonDateTimeControl* startTimeButton = DYNAMIC_DOWNCAST(CRibbonDateTimeControl, GetRibbonBar()->FindByID(ID_DOCUMENT_SETTINGS_START_TIME));
	CMFCRibbonComboBox* utcOffsetsCombo = DYNAMIC_DOWNCAST(CMFCRibbonComboBox, GetRibbonBar()->FindByID(ID_DOCUMENT_SETTINGS_UTC_OFFFSET));

	startDateButton->SetTime(time);
	startTimeButton->SetTime(time);
	for (int i = 0; i < utcOffsets.size(); ++i)
	{
		if (utcOffsets.at(i).second == utcOffset)
		{
			utcOffsetsCombo->SelectItem(i);
			break;
		}
	}
	

	return (LRESULT)TRUE;
}
void CMainFrame::OnSetUTCOffset()
{
	CSchedulerDoc* pDoc = reinterpret_cast<CSchedulerDoc*>(GetActiveDocument());
	ASSERT_VALID(pDoc);
	if (!pDoc) return;
	CMFCRibbonComboBox* utcOffsetsCombo = DYNAMIC_DOWNCAST(CMFCRibbonComboBox, GetRibbonBar()->FindByID(ID_DOCUMENT_SETTINGS_UTC_OFFFSET));
	auto data = (int*)utcOffsetsCombo->GetItemData(utcOffsetsCombo->GetCurSel());
	pDoc->SetUTCOffsetMinutes(*data, 0);
}
void CMainFrame::OnStartTimeChange(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	CSchedulerDoc* pDoc = reinterpret_cast<CSchedulerDoc*>(GetActiveDocument());
	ASSERT_VALID(pDoc);
	if (!pDoc) return;
	CRibbonDateTimeControl* startDateButton = DYNAMIC_DOWNCAST(CRibbonDateTimeControl, GetRibbonBar()->FindByID(ID_DOCUMENT_SETTINGS_START_DATE));
	CRibbonDateTimeControl* startTimeButton = DYNAMIC_DOWNCAST(CRibbonDateTimeControl, GetRibbonBar()->FindByID(ID_DOCUMENT_SETTINGS_START_TIME));

	auto date = startDateButton->GetTime();
	auto time = startTimeButton->GetTime();
	auto hour = std::chrono::hours(time.GetHour());
	auto minute = std::chrono::minutes(time.GetMinute());
	auto seconds = std::chrono::seconds(time.GetSecond());
	auto year = date.GetYear();
	auto month = date.GetMonth();
	auto day = date.GetDay();

	std::chrono::minutes utcOffset(pDoc->GetUTCOffsetMinutes());
	auto local_time = date::make_time(hour+minute+seconds);
	auto local_date = date::year{ year } / date::month{ (unsigned int)month } / date::day{ (unsigned int)day };
	std::chrono::system_clock::time_point tp (((date::sys_days)local_date).time_since_epoch() + local_time.to_duration() + utcOffset);
	pDoc->SetStartTime(tp,0);
}