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

#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "scheduler.h"
#include "CScheduleEvent.h"
#include "PropertyGridDateProperty.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNAMIC(CCustomPropertyGrid, CMFCPropertyGridCtrl)

BEGIN_MESSAGE_MAP(CCustomPropertyGrid, CMFCPropertyGridCtrl)
	ON_NOTIFY(DTN_DATETIMECHANGE, AFX_PROPLIST_ID_INPLACE, &CCustomPropertyGrid::OnTimeChange)
END_MESSAGE_MAP()

void CCustomPropertyGrid::OnTimeChange(NMHDR* pNMHDR, LRESULT* pResult)
{
	ASSERT_VALID(this);

	if (m_pSel == NULL)
	{
		ASSERT(FALSE);
		return;
	}

	m_pSel->OnUpdateValue();
//	EndEditItem();
}

CPropertiesWnd::CPropertiesWnd() noexcept
{
	m_nComboHeight = 0;
}

CPropertiesWnd::~CPropertiesWnd()
{
}

IMPLEMENT_DYNAMIC(CPropertiesWnd, CViewDockingPane)

BEGIN_MESSAGE_MAP(CPropertiesWnd, CViewDockingPane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_MESSAGE(WM_STOCK_EVENT_OBJECT_SELECTED, OnStockEventObjectSelected)
	ON_MESSAGE(WM_EVENT_OBJECT_SELECTED, OnEventObjectSelected)
	ON_MESSAGE(WM_TRACK_OBJECT_SELECTED, OnTrackObjectSelected)
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar message handlers

void CPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd () == nullptr || (AfxGetMainWnd() != nullptr && AfxGetMainWnd()->IsIconic()))
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	m_wndPropList.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create combo:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	
	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("Failed to create Properties Grid \n");
		return -1;      // fail to create
	}

	InitPropList();

	AdjustLayout();
	return 0;
}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPropertiesWnd::OnInitialUpdate()
{
	CSchedulerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);	
}
void CPropertiesWnd::OnUpdate(const LPARAM lHint)
{
	CPropertiesWnd* potentialSource = reinterpret_cast<CPropertiesWnd*>(lHint);
	if (potentialSource && potentialSource == this) return;

	CSchedulerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
}
std::chrono::seconds CPropertiesWnd::GetDurationFromPropertyParent(CMFCPropertyGridProperty* property)
{
	long days = property->GetSubItem(0)->GetValue().intVal;
	long hours = property->GetSubItem(1)->GetValue().intVal;
	long minutes = property->GetSubItem(2)->GetValue().intVal;
	long seconds = property->GetSubItem(3)->GetValue().intVal;
	if (seconds > 59)
	{
		seconds = 59;
		property->GetSubItem(3)->SetValue((variant_t)seconds);
	}
	if (minutes > 59)
	{
		minutes = 59;
		property->GetSubItem(2)->SetValue((variant_t)minutes);
	}
	if (hours > 23)
	{
		hours = 23;
		property->GetSubItem(1)->SetValue((variant_t)hours);
	}		

	using days_t = std::chrono::duration<int, std::ratio<86400>>;
	return std::chrono::seconds(seconds) + std::chrono::minutes(minutes) +
		std::chrono::hours(hours) + days_t(days);
}
LRESULT CPropertiesWnd::OnPropertyChanged(WPARAM wparam, LPARAM lparam)
{	
	CMFCPropertyGridProperty* property = (CMFCPropertyGridProperty*)lparam;
	switch ((int)property->GetData())
	{
	case IDEventName:
	{
		CString newName = (LPCTSTR)(_bstr_t)property->GetValue();
		if (event)
		{
			event->SetName(newName);
			GetDocument()->SetModifiedFlag(TRUE);
			GetDocument()->UpdateAllViews(nullptr, (LPARAM)this);
		}
	}
		break;
	case IDEventColor:
	{
		CMFCPropertyGridColorProperty* colorProp = (CMFCPropertyGridColorProperty*)lparam;
		auto color = colorProp->GetColor();
		auto r = GetRValue(color);
		auto g = GetGValue(color);
		auto b = GetBValue(color);
		UINT32 eventColor = (r << 16) | (g << 8) | b;		
		event->SetColor(eventColor);
		GetDocument()->SetModifiedFlag(TRUE);
		GetDocument()->UpdateAllViews(nullptr, (LPARAM)this);
	}
		break;
	case IDEventDurationDays:
		[[fallthrough]];
	case IDEventDurationHours:
		[[fallthrough]];
	case IDEventDurationMinutes:
		[[fallthrough]];
	case IDEventDurationSeconds:
	{
		auto durationProperty = property->GetParent();
		event->SetDuration(GetDurationFromPropertyParent(property->GetParent()));
		GetDocument()->SetModifiedFlag(TRUE);
		GetDocument()->UpdateAllViews(nullptr, (LPARAM)this);
	}	
		break;
	case IDEventDuration:
	{
		event->SetDuration(GetDurationFromPropertyParent(property));
		GetDocument()->SetModifiedFlag(TRUE);
		GetDocument()->UpdateAllViews(nullptr, (LPARAM)this);
	}
		break;
	case IDTrackName:
	{
		CString newName = (LPCTSTR)(_bstr_t)property->GetValue();
		if (track)
		{
			GetDocument()->UpdateTrackName(track, newName, (LPARAM)this);
		}
	}
		break;
	case IDTrackStartTime:
		[[fallthrough]];
	case IDTrackStartDate:
	{
		auto trackPropertyGroup = m_wndPropList.GetProperty(0);		
		CPropertyGridDateProperty* dateProperty = (CPropertyGridDateProperty*)trackPropertyGroup->GetSubItem(1);
		CPropertyGridDateProperty* timeProperty = (CPropertyGridDateProperty*)trackPropertyGroup->GetSubItem(2);
		auto date = dateProperty->GetTime();
		auto time = timeProperty->GetTime();
		if (track)
		{
			auto hour = std::chrono::hours(time.GetHour());
			auto minute = std::chrono::minutes(time.GetMinute());
			auto seconds = std::chrono::seconds(time.GetSecond());
			auto year = date.GetYear();
			auto month = date.GetMonth();
			auto day = date.GetDay();
			std::chrono::minutes utcOffset(GetDocument()->GetUTCOffsetMinutes());
			auto local_time = date::make_time(hour + minute + seconds);
			auto local_date = date::year{ year } / date::month{ (unsigned int)month } / date::day{ (unsigned int)day };
			std::chrono::system_clock::time_point tp(((date::sys_days)local_date).time_since_epoch() + local_time.to_duration() + utcOffset);

			GetDocument()->UpdateTrackStartTime(track, tp,(LPARAM)this);
		}
	}
	break;
	}
	return (LRESULT)TRUE;
}

void CPropertiesWnd::SetupTrackPropertyControls(CScheduleTrack* track)
{
	auto trackPropertyGroup = new CMFCPropertyGridProperty(_T("Track"));
	auto trackNameProperty = new CMFCPropertyGridProperty(_T("Name"), (variant_t)track->GetName(), 
		_T("Specifies the name of the track"), IDTrackName);
	trackPropertyGroup->AddSubItem(trackNameProperty);
	std::chrono::minutes utcOffset(GetDocument()->GetUTCOffsetMinutes());
	COleDateTime time((__time64_t)std::chrono::duration_cast<std::chrono::seconds>(track->GetStartTime().time_since_epoch()).count());
	ASSERT(time.GetStatus() == COleDateTime::valid);
	auto trackStartDateProperty = new CPropertyGridDateProperty(_T("Start Date"), time,
		_T("Specifies the start date of the track"), IDTrackStartDate, false);
	trackPropertyGroup->AddSubItem(trackStartDateProperty);
	auto trackStartTimeProperty = new CPropertyGridDateProperty(_T("Start Time"), time,
		_T("Specifies the start time of the track"), IDTrackStartTime, true);
	trackPropertyGroup->AddSubItem(trackStartTimeProperty);

	m_wndPropList.AddProperty(trackPropertyGroup);
	m_wndPropList.ExpandAll();
}
void CPropertiesWnd::SetupEventPropertyControls(CScheduleStockEvent* event)
{	
	D2D1::ColorF color(event->GetColor());
	auto duration = event->GetDuration();
	using days_t = std::chrono::duration<int, std::ratio<86400>>;
	const auto d = std::chrono::duration_cast<days_t>(duration);
	duration -= d;
	const auto h = std::chrono::duration_cast<std::chrono::hours>(duration);
	duration -= h;
	const auto m = std::chrono::duration_cast<std::chrono::minutes>(duration);
	duration -= m;
	const auto s = std::chrono::duration_cast<std::chrono::seconds>(duration);	

	auto eventPropertyGroup = new CMFCPropertyGridProperty(_T("Event"));
	auto eventNameProperty = new CMFCPropertyGridProperty(_T("Name"), (variant_t)event->GetName(), _T("Specifies the name of the event"), IDEventName);
	eventPropertyGroup->AddSubItem(eventNameProperty);
	auto eventColorProperty = new CMFCPropertyGridColorProperty(_T("Color"), RGB(color.r * 255.f, color.g * 255.f, color.b * 255.f), nullptr, _T("Specifies the event color"), IDEventColor);
	eventColorProperty->EnableOtherButton(_T("Other..."));
	eventColorProperty->EnableAutomaticButton(_T("Default"), ::GetSysColor(COLOR_3DFACE));

	eventPropertyGroup->AddSubItem(eventColorProperty);

	auto eventDurationPropertyGroup = new CMFCPropertyGridProperty(_T("Duration"), IDEventDuration, FALSE);
	auto eventDaysProperty = new CMFCPropertyGridProperty(_T("Days"), (_variant_t)(long)d.count(), _T("Specifies the number of days this event will last"),IDEventDurationDays);
	eventDurationPropertyGroup->AddSubItem(eventDaysProperty);
	auto eventHoursProperty = new CMFCPropertyGridProperty(_T("Hours"), (_variant_t)(long)h.count(), _T("Specifies the number of hours this event will last"), IDEventDurationHours);
	eventHoursProperty->EnableSpinControl(TRUE, 0, 23);
	eventDurationPropertyGroup->AddSubItem(eventHoursProperty);
	auto eventMinutesProperty = new CMFCPropertyGridProperty(_T("Minutes"), (_variant_t)(long)m.count(), _T("Specifies the number of minutes this event will last"), IDEventDurationMinutes);
	eventMinutesProperty->EnableSpinControl(TRUE, 0, 59);
	eventDurationPropertyGroup->AddSubItem(eventMinutesProperty);
	auto eventSecondsProperty = new CMFCPropertyGridProperty(_T("Seconds"), (_variant_t)(long)s.count(), _T("Specifies the number of seconds this event will last"), IDEventDurationSeconds);
	eventSecondsProperty->EnableSpinControl(TRUE, 0, 59);
	eventDurationPropertyGroup->AddSubItem(eventSecondsProperty);

	eventPropertyGroup->AddSubItem(eventDurationPropertyGroup);

	m_wndPropList.AddProperty(eventPropertyGroup);
	m_wndPropList.ExpandAll();
}

void CPropertiesWnd::InitPropList()
{
	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	/*CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("Appearance"));

	pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("3D Look"), (_variant_t) false, _T("Specifies the window's font will be non-bold and controls will have a 3D border")));

	CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("Border"), _T("Dialog Frame"), _T("One of: None, Thin, Resizable, or Dialog Frame"));
	pProp->AddOption(_T("None"));
	pProp->AddOption(_T("Thin"));
	pProp->AddOption(_T("Resizable"));
	pProp->AddOption(_T("Dialog Frame"));
	pProp->AllowEdit(FALSE);

	pGroup1->AddSubItem(pProp);
	pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("Caption"), (_variant_t) _T("About"), _T("Specifies the text that will be displayed in the window's title bar")));

	m_wndPropList.AddProperty(pGroup1);

	CMFCPropertyGridProperty* pSize = new CMFCPropertyGridProperty(_T("Window Size"), 0, TRUE);

	pProp = new CMFCPropertyGridProperty(_T("Height"), (_variant_t) 250l, _T("Specifies the window's height"));
	pProp->EnableSpinControl(TRUE, 50, 300);
	pSize->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty( _T("Width"), (_variant_t) 150l, _T("Specifies the window's width"));
	pProp->EnableSpinControl(TRUE, 50, 200);
	pSize->AddSubItem(pProp);

	m_wndPropList.AddProperty(pSize);

	CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty(_T("Font"));

	LOGFONT lf;
	CFont* font = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	font->GetLogFont(&lf);

	_tcscpy_s(lf.lfFaceName, _T("Arial"));

	pGroup2->AddSubItem(new CMFCPropertyGridFontProperty(_T("Font"), lf, CF_EFFECTS | CF_SCREENFONTS, _T("Specifies the default font for the window")));
	pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("Use System Font"), (_variant_t) true, _T("Specifies that the window uses MS Shell Dlg font")));

	m_wndPropList.AddProperty(pGroup2);

	CMFCPropertyGridProperty* pGroup3 = new CMFCPropertyGridProperty(_T("Misc"));
	pProp = new CMFCPropertyGridProperty(_T("(Name)"), _T("Application"));
	pProp->Enable(FALSE);
	pGroup3->AddSubItem(pProp);

	CMFCPropertyGridColorProperty* pColorProp = new CMFCPropertyGridColorProperty(_T("Window Color"), RGB(210, 192, 254), nullptr, _T("Specifies the default window color"));
	pColorProp->EnableOtherButton(_T("Other..."));
	pColorProp->EnableAutomaticButton(_T("Default"), ::GetSysColor(COLOR_3DFACE));
	pGroup3->AddSubItem(pColorProp);

	static const TCHAR szFilter[] = _T("Icon Files(*.ico)|*.ico|All Files(*.*)|*.*||");
	pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("Icon"), TRUE, _T(""), _T("ico"), 0, szFilter, _T("Specifies the window icon")));

	pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("Folder"), _T("c:\\")));

	m_wndPropList.AddProperty(pGroup3);

	CMFCPropertyGridProperty* pGroup4 = new CMFCPropertyGridProperty(_T("Hierarchy"));

	CMFCPropertyGridProperty* pGroup41 = new CMFCPropertyGridProperty(_T("First sub-level"));
	pGroup4->AddSubItem(pGroup41);

	CMFCPropertyGridProperty* pGroup411 = new CMFCPropertyGridProperty(_T("Second sub-level"));
	pGroup41->AddSubItem(pGroup411);

	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 1"), (_variant_t) _T("Value 1"), _T("This is a description")));
	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 2"), (_variant_t) _T("Value 2"), _T("This is a description")));
	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 3"), (_variant_t) _T("Value 3"), _T("This is a description")));

	pGroup4->Expand(FALSE);
	m_wndPropList.AddProperty(pGroup4);*/
}

void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void CPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
}


LRESULT CPropertiesWnd::OnEventObjectSelected(WPARAM wparam, LPARAM lparam)
{
	m_wndPropList.RemoveAll();
	m_wndPropList.RedrawWindow();

	int eventId = (int)wparam;
	auto scheduledEvent = GetDocument()->GetEvent(eventId);
	if (!scheduledEvent) return 0;
	event = GetDocument()->GetStockEvent(scheduledEvent->GetStockId());
	track = nullptr;

	SetupEventPropertyControls(event);

	return 0;
}


LRESULT CPropertiesWnd::OnTrackObjectSelected(WPARAM wparam, LPARAM lparam)
{
	m_wndPropList.RemoveAll();
	m_wndPropList.RedrawWindow();

	int trackId = (int)wparam;
	track = GetDocument()->GetTrack(trackId);
	if (!track) return 0;
	event = nullptr;

	SetupTrackPropertyControls(track);

	return 0;
}

afx_msg LRESULT CPropertiesWnd::OnStockEventObjectSelected(WPARAM wparam, LPARAM lparam)
{
	m_wndPropList.RemoveAll();
	m_wndPropList.RedrawWindow();
	int eventId = (int)wparam;
	event = GetDocument()->GetStockEvent(eventId);
	if (!event) return 0;
	track = nullptr;

	SetupEventPropertyControls(event);

	return 0;
}