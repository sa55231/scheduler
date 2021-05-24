#pragma once
#include <afxpropertypage.h>
#include "Resource.h"
#include "scheduler.h"

class CSchedulerOptionsPage :
    public CMFCPropertyPage
{
    DECLARE_DYNCREATE(CSchedulerOptionsPage)

	// Construction
public:
	CSchedulerOptionsPage();
	~CSchedulerOptionsPage();

	enum { IDD = IDD_SCHEDULE_OPTIONS_DIALOG};

	// Overrides
public:
	virtual void OnOK();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
private:
	void StoreFont(CFont* cFont, const CString& name);
	BOOL LoadFont(CFont& cFont, const CString& name);
	void SetStaticFont(CFont& cFont, int ID);
private:
	CFont defaultEventsFont;
	CFont defaultTracksFont;
	CFont defaultHeaderFont;
	int printScheduleType = CSchedulerApp::PrintTypeGraphical;
	int printTrackPerPage = 0;
// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedEventsFontSelectButton();
	afx_msg void OnBnClickedTracksFontSelectButton();
	afx_msg void OnBnClickedHeaderFontSelectButton();
	afx_msg void OnBnClickedPrintRadio(UINT);	
	afx_msg void OnBnClickedPrintOneTrackPerPageCheck();
};

