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
#include "EventMaxCountConstraint.h"

#include <chrono>

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
	int days = property->GetSubItem(0)->GetValue().intVal;
	int hours = property->GetSubItem(1)->GetValue().intVal;
	int minutes = property->GetSubItem(2)->GetValue().intVal;
	int seconds = property->GetSubItem(3)->GetValue().intVal;

	if (seconds > 59)
	{
		seconds = 59;
		property->GetSubItem(3)->SetValue(COleVariant((long)seconds, VT_I4));
	}
	if (minutes > 59)
	{
		minutes = 59;
		property->GetSubItem(2)->SetValue(COleVariant((long)minutes, VT_I4));
	}
	if (hours > 23)
	{
		hours = 23;
		property->GetSubItem(1)->SetValue(COleVariant((long)hours, VT_I4));
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
			GetDocument()->UpdateStockEventName(event, newName, (LPARAM)this);
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
		GetDocument()->UpdateStockEventDuration(event, GetDurationFromPropertyParent(property->GetParent()), (LPARAM)this);
	}	
		break;
	case IDEventDuration:
	{
		GetDocument()->UpdateStockEventDuration(event, GetDurationFromPropertyParent(property), (LPARAM)this);
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
		auto trackPropertyGroup = property->GetParent();
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
			auto local_time = hour + minute + seconds;
			auto local_date = std::chrono::year{ year } / std::chrono::month{ (unsigned int)month } / std::chrono::day{ (unsigned int)day };
			std::chrono::system_clock::time_point tp(((std::chrono::sys_days)local_date).time_since_epoch() + local_time + utcOffset);

			GetDocument()->UpdateTrackStartTime(track, tp,(LPARAM)this);
		}
	}
	break;
	case IDEventConstraintsMaxCount:
		GetDocument()->UpdateStockEventConstraint(event, ConstraintType::MaxCountConstraint, property->GetValue(), (LPARAM)this);
		break;
	}

	return (LRESULT)TRUE;
}

void CPropertiesWnd::SetupTrackPropertyControls(CScheduleTrack* track)
{

	for (int i = 0; i < m_wndPropList.GetPropertyCount(); i++)
	{
		auto prop = m_wndPropList.GetProperty(i);
		if (prop->GetData() == IDTrackGroup)
		{
			prop->GetSubItem(0)->SetOriginalValue((variant_t)track->GetName());
			prop->GetSubItem(0)->SetValue((variant_t)track->GetName());

			std::chrono::minutes utcOffset(GetDocument()->GetUTCOffsetMinutes());
			COleDateTime time((__time64_t)std::chrono::duration_cast<std::chrono::seconds>(track->GetStartTime().time_since_epoch()).count());
			ASSERT(time.GetStatus() == COleDateTime::valid);

			prop->GetSubItem(1)->SetOriginalValue(time);
			prop->GetSubItem(1)->SetValue(time);
			prop->GetSubItem(2)->SetOriginalValue(time);
			prop->GetSubItem(2)->SetValue(time);

			prop->Show(TRUE);
		}
	}
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

	for (int i = 0; i < m_wndPropList.GetPropertyCount(); i++)
	{
		auto prop = m_wndPropList.GetProperty(i);
		switch (prop->GetData())
		{
		case IDEventGroup:
			prop->GetSubItem(0)->SetOriginalValue((variant_t)event->GetName());
			prop->GetSubItem(1)->SetOriginalValue((LONG)RGB(color.r * 255.f, color.g * 255.f, color.b * 255.f));
			break;
		case IDEventDuration:
		{
			COleVariant days((long)d.count(), VT_I4);
			COleVariant hours((long)h.count(), VT_I4);
			COleVariant minutes((long)m.count(), VT_I4);
			COleVariant seconds((long)s.count(), VT_I4);
			prop->GetSubItem(0)->SetOriginalValue(days);
			prop->GetSubItem(1)->SetOriginalValue(hours);
			prop->GetSubItem(2)->SetOriginalValue(minutes);
			prop->GetSubItem(3)->SetOriginalValue(seconds);
		}
			break;
		case IDEventConstraints:
		{
			for (int i = 0; i < prop->GetSubItemsCount(); i++)
			{
				auto constraintProp = prop->GetSubItem(i);
				for (const auto& c : event->GetConstraints())
				{
					if (c->GetType() == ConstraintType::MaxCountConstraint && constraintProp->GetData() == IDEventConstraintsMaxCount)
					{
						auto maxCountConstraint = (CEventMaxCountConstraint*)c.get();
						COleVariant value((long)maxCountConstraint->GetCount(), VT_I4);
						constraintProp->SetOriginalValue(value);
					}
				}
			}
		}
			break;
		}
		prop->ResetOriginalValue();
		prop->Show(prop->GetData() != IDTrackGroup);
	}


	m_wndPropList.ExpandAll();
}

void CPropertiesWnd::InitPropList()
{
	SetPropListFont();
	m_wndPropList.SetOwner(this);
	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();
	CString str;
	auto eventPropertyGroup = new CMFCPropertyGridProperty(_T("Event"), IDEventGroup);
	auto eventNameProperty = new CMFCPropertyGridProperty(_T("Name"), (variant_t)str, _T("Specifies the name of the event"), IDEventName);
	eventPropertyGroup->AddSubItem(eventNameProperty);
	auto eventColorProperty = new CMFCPropertyGridColorProperty(_T("Color"), RGB(0, 0, 0), nullptr, _T("Specifies the event color"), IDEventColor);
	eventColorProperty->EnableOtherButton(_T("Other..."));
	eventColorProperty->EnableAutomaticButton(_T("Default"), ::GetSysColor(COLOR_3DFACE));

	eventPropertyGroup->AddSubItem(eventColorProperty);
	COleVariant number((long)0,VT_I4);

	auto eventDurationPropertyGroup = new CMFCPropertyGridProperty(_T("Duration"), IDEventDuration, FALSE);
	auto eventDaysProperty = new CMFCPropertyGridProperty(_T("Days"), number, _T("Specifies the number of days this event will last"), IDEventDurationDays);
	eventDurationPropertyGroup->AddSubItem(eventDaysProperty);
	auto eventHoursProperty = new CMFCPropertyGridProperty(_T("Hours"), number, _T("Specifies the number of hours this event will last"), IDEventDurationHours);
	eventHoursProperty->EnableSpinControl(TRUE, 0, 23);
	eventDurationPropertyGroup->AddSubItem(eventHoursProperty);
	auto eventMinutesProperty = new CMFCPropertyGridProperty(_T("Minutes"), number, _T("Specifies the number of minutes this event will last"), IDEventDurationMinutes);
	eventMinutesProperty->EnableSpinControl(TRUE, 0, 59);
	eventDurationPropertyGroup->AddSubItem(eventMinutesProperty);
	auto eventSecondsProperty = new CMFCPropertyGridProperty(_T("Seconds"), number, _T("Specifies the number of seconds this event will last"), IDEventDurationSeconds);
	eventSecondsProperty->EnableSpinControl(TRUE, 0, 59);
	eventDurationPropertyGroup->AddSubItem(eventSecondsProperty);

	auto eventConstraintsPropertyGroup = new CMFCPropertyGridProperty(_T("Constraints"), IDEventConstraints, FALSE);
	auto maxCountConstraintProperty = new CMFCPropertyGridProperty(_T("Max Count"), number, _T("Specifies the maximum count allowed"), IDEventConstraintsMaxCount);
	maxCountConstraintProperty->EnableSpinControl(TRUE, 0, INT_MAX);
	eventConstraintsPropertyGroup->AddSubItem(maxCountConstraintProperty);

	auto trackPropertyGroup = new CMFCPropertyGridProperty(_T("Track"), IDTrackGroup);
	auto trackNameProperty = new CMFCPropertyGridProperty(_T("Name"), (variant_t)str,
		_T("Specifies the name of the track"), IDTrackName);
	trackPropertyGroup->AddSubItem(trackNameProperty);

	COleDateTime time;	
	auto trackStartDateProperty = new CPropertyGridDateProperty(_T("Start Date"), time,
		_T("Specifies the start date of the track"), IDTrackStartDate, false);
	trackPropertyGroup->AddSubItem(trackStartDateProperty);
	auto trackStartTimeProperty = new CPropertyGridDateProperty(_T("Start Time"), time,
		_T("Specifies the start time of the track"), IDTrackStartTime, true);
	trackPropertyGroup->AddSubItem(trackStartTimeProperty);

	m_wndPropList.AddProperty(eventPropertyGroup);
	m_wndPropList.AddProperty(eventDurationPropertyGroup);
	m_wndPropList.AddProperty(eventConstraintsPropertyGroup);
	m_wndPropList.AddProperty(trackPropertyGroup);

	for (int i = 0; i < m_wndPropList.GetPropertyCount(); i++)
	{
		auto prop = m_wndPropList.GetProperty(i);
		prop->Show(FALSE);
	}
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
	for (int i = 0; i < m_wndPropList.GetPropertyCount(); i++)
	{
		auto prop = m_wndPropList.GetProperty(i);
		prop->Show(prop->GetData() != IDTrackGroup);
	}
	
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
	for (int i = 0; i < m_wndPropList.GetPropertyCount(); i++)
	{
		auto prop = m_wndPropList.GetProperty(i);
		prop->Show(prop->GetData() == IDTrackGroup);
	}
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
	for (int i = 0; i < m_wndPropList.GetPropertyCount(); i++)
	{
		auto prop = m_wndPropList.GetProperty(i);
		prop->Show(prop->GetData() != IDTrackGroup);
	}

	m_wndPropList.RedrawWindow();
	int eventId = (int)wparam;
	event = GetDocument()->GetStockEvent(eventId);
	if (!event) return 0;
	track = nullptr;

	SetupEventPropertyControls(event);

	return 0;
}