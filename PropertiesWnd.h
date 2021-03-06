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

#pragma once

#include "ViewDockingPane.h"
#include "CScheduleStockEvent.h"

#include <chrono>


class CCustomPropertyGrid : public CMFCPropertyGridCtrl
{
	DECLARE_DYNAMIC(CCustomPropertyGrid)

protected:
	afx_msg void OnTimeChange(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
};

class CPropertiesWnd : public CViewDockingPane
{
	DECLARE_DYNAMIC(CPropertiesWnd)
// Construction
public:
	CPropertiesWnd() noexcept;

	void AdjustLayout();

// Attributes
public:
	void SetVSDotNetLook(BOOL bSet)
	{
		m_wndPropList.SetVSDotNetLook(bSet);
		m_wndPropList.SetGroupNameFullWidth(bSet);
	}
protected:
	virtual void OnInitialUpdate();
	virtual void OnUpdate(const LPARAM lHint);

protected:
	CFont m_fntPropList;
	CCustomPropertyGrid m_wndPropList;
	enum PropertyControlsID
	{
		IDEventGroup = 0,
		IDEventName,
		IDEventColor,
		IDEventDuration,
		IDEventDurationDays,
		IDEventDurationHours,
		IDEventDurationMinutes,
		IDEventDurationSeconds,
		IDTrackGroup,
		IDTrackName,
		IDTrackStartDate,
		IDTrackStartTime,
		IDEventConstraints,
		IDEventConstraintsMaxCount,
	};
// Implementation
public:
	virtual ~CPropertiesWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg LRESULT OnEventObjectSelected(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnTrackObjectSelected(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnPropertyChanged(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnStockEventObjectSelected(WPARAM wparam, LPARAM lparam);
	DECLARE_MESSAGE_MAP()

private:
	void InitPropList();
	void SetPropListFont();
	void SetupEventPropertyControls(CScheduleStockEvent* event);
	void SetupTrackPropertyControls(CScheduleTrack* event);
	std::chrono::seconds GetDurationFromPropertyParent(CMFCPropertyGridProperty* property);

private:
	CScheduleStockEvent* event = nullptr;
	CScheduleTrack* track = nullptr;
	int m_nComboHeight;
};

