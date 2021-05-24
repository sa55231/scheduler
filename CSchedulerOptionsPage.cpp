#include "pch.h"
#include "CSchedulerOptionsPage.h"
#include "scheduler.h"

IMPLEMENT_DYNCREATE(CSchedulerOptionsPage, CMFCPropertyPage)

BEGIN_MESSAGE_MAP(CSchedulerOptionsPage, CMFCPropertyPage)
	ON_BN_CLICKED(IDC_EVENTS_FONT_SELECT_BUTTON, &CSchedulerOptionsPage::OnBnClickedEventsFontSelectButton)
	ON_BN_CLICKED(IDC_TRACKS_FONT_SELECT_BUTTON, &CSchedulerOptionsPage::OnBnClickedTracksFontSelectButton)
	ON_BN_CLICKED(IDC_HEADER_FONT_SELECT_BUTTON, &CSchedulerOptionsPage::OnBnClickedHeaderFontSelectButton)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_PRINT_SCHEDULE_GRAPHICAL_RADIO, IDC_PRINT_SCHEDULE_TEXT_RADIO,&CSchedulerOptionsPage::OnBnClickedPrintRadio)
	ON_BN_CLICKED(IDC_PRINT_ONE_TRACK_PER_PAGE_CHECK, &CSchedulerOptionsPage::OnBnClickedPrintOneTrackPerPageCheck)
END_MESSAGE_MAP()

CSchedulerOptionsPage::CSchedulerOptionsPage():CMFCPropertyPage(CSchedulerOptionsPage::IDD)
{}
CSchedulerOptionsPage::~CSchedulerOptionsPage()
{}

BOOL CSchedulerOptionsPage::OnInitDialog()
{
	printScheduleType = theApp.GetProfileInt(_T("Settings"),_T("PrintType"), CSchedulerApp::PrintTypeGraphical);
	printTrackPerPage = theApp.GetProfileInt(_T("Settings"), _T("PrintTrackPerPage"), 0);
	if (!LoadFont(defaultEventsFont, _T("DefaultEventsFont")))
	{
		LOGFONT font = { 0 };
		font.lfHeight = -20;
		font.lfWeight = FW_NORMAL;
		_tcscpy_s(font.lfFaceName, _T("Times New Roman"));
		defaultEventsFont.CreateFontIndirect(&font);
	}
	if (!LoadFont(defaultTracksFont, _T("DefaultTracksFont")))
	{
		LOGFONT font = { 0 };
		font.lfHeight = -20;
		font.lfWeight = FW_NORMAL;
		_tcscpy_s(font.lfFaceName, _T("Times New Roman"));
		defaultTracksFont.CreateFontIndirect(&font);
	}
	if (!LoadFont(defaultHeaderFont, _T("DefaultHeadersFont")))
	{
		LOGFONT font = { 0 };
		font.lfHeight = -17;
		font.lfWeight = FW_NORMAL;
		_tcscpy_s(font.lfFaceName, _T("Times New Roman"));
		defaultHeaderFont.CreateFontIndirect(&font);
	}

	auto check = GetDlgItem(IDC_PRINT_ONE_TRACK_PER_PAGE_CHECK);
	ASSERT_VALID(check);
	check->EnableWindow(printScheduleType == CSchedulerApp::PrintTypeText);

	SetStaticFont(defaultHeaderFont, IDC_HEADER_FONT_PREVIEW_STATIC);
	SetStaticFont(defaultTracksFont, IDC_TRACKS_FONT_PREVIEW_STATIC);
	SetStaticFont(defaultEventsFont, IDC_EVENTS_FONT_PREVIEW_STATIC);

	return CMFCPropertyPage::OnInitDialog();
}
void CSchedulerOptionsPage::SetStaticFont(CFont& cFont, int ID)
{
	auto staticControl = GetDlgItem(ID);
	ASSERT_VALID(staticControl);
	LOGFONT font = { 0 };
	cFont.GetLogFont(&font);
	staticControl->SetWindowText(font.lfFaceName);
	staticControl->SetFont(&defaultHeaderFont);
}
void CSchedulerOptionsPage::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_PRINT_SCHEDULE_GRAPHICAL_RADIO, printScheduleType);
	DDX_Check(pDX, IDC_PRINT_ONE_TRACK_PER_PAGE_CHECK, printTrackPerPage);
}

void CSchedulerOptionsPage::OnBnClickedEventsFontSelectButton()
{	
	LOGFONT font = { 0 };
	defaultEventsFont.GetLogFont(&font);
	CFontDialog dialog(&font, CF_EFFECTS | CF_SCREENFONTS, nullptr, this);

	if (dialog.DoModal() == IDOK)
	{
		LOGFONT selectedFont = { 0 };
		dialog.GetCurrentFont(&selectedFont);
		defaultEventsFont.DeleteObject();
		defaultEventsFont.CreateFontIndirect(&selectedFont);
		SetStaticFont(defaultEventsFont, IDC_EVENTS_FONT_PREVIEW_STATIC);
		SetModified();
	}
}

void CSchedulerOptionsPage::OnBnClickedTracksFontSelectButton()
{
	LOGFONT font = { 0 };
	defaultTracksFont.GetLogFont(&font);
	CFontDialog dialog(&font, CF_EFFECTS | CF_SCREENFONTS, nullptr, this);
	if (dialog.DoModal() == IDOK)
	{
		LOGFONT selectedFont = { 0 };
		dialog.GetCurrentFont(&selectedFont);
		defaultTracksFont.DeleteObject();
		defaultTracksFont.CreateFontIndirect(&selectedFont);
		SetStaticFont(defaultTracksFont, IDC_TRACKS_FONT_PREVIEW_STATIC);
		SetModified();
	}
}

void CSchedulerOptionsPage::OnBnClickedHeaderFontSelectButton()
{
	LOGFONT font = { 0 };
	defaultHeaderFont.GetLogFont(&font);
	CFontDialog dialog(&font, CF_EFFECTS | CF_SCREENFONTS, nullptr, this);

	if (dialog.DoModal() == IDOK)
	{
		LOGFONT selectedFont = { 0 };
		dialog.GetCurrentFont(&selectedFont);
		defaultHeaderFont.DeleteObject();
		defaultHeaderFont.CreateFontIndirect(&selectedFont);
		SetStaticFont(defaultHeaderFont, IDC_HEADER_FONT_PREVIEW_STATIC);
		SetModified();
	}

}

void CSchedulerOptionsPage::OnBnClickedPrintRadio(UINT /**/)
{
	UpdateData();
	auto check = GetDlgItem(IDC_PRINT_ONE_TRACK_PER_PAGE_CHECK);
	ASSERT_VALID(check);
	check->EnableWindow(printScheduleType == CSchedulerApp::PrintTypeText);
	SetModified();
}

void CSchedulerOptionsPage::OnBnClickedPrintOneTrackPerPageCheck()
{
	UpdateData();
	SetModified();
}

BOOL CSchedulerOptionsPage::LoadFont(CFont& cFont, const CString& name)
{
	LOGFONT* font;
	UINT length;
	if (theApp.GetProfileBinary(_T("Settings"), name, (BYTE**)&font,&length))
	{
		cFont.CreateFontIndirect(font);
		delete font;
		return TRUE;
	}
	return FALSE;
}
void CSchedulerOptionsPage::StoreFont(CFont* cFont, const CString& name)
{
	LOGFONT font = { 0 };
	cFont->GetLogFont(&font);
	theApp.WriteProfileBinary(_T("Settings"), name,(BYTE*) &font, sizeof(font));
}
void CSchedulerOptionsPage::OnOK()
{
	theApp.WriteProfileInt(_T("Settings"), _T("PrintType"), printScheduleType);
	theApp.WriteProfileInt(_T("Settings"), _T("PrintTrackPerPage"), printTrackPerPage);	
	StoreFont(&defaultEventsFont, _T("DefaultEventsFont"));
	StoreFont(&defaultTracksFont, _T("DefaultTracksFont"));
	StoreFont(&defaultHeaderFont, _T("DefaultHeadersFont"));
}
