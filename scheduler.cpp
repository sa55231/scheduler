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

// scheduler.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "scheduler.h"
#include "MainFrm.h"

#include "schedulerDoc.h"
#include "schedulerView.h"

#include <strsafe.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSchedulerApp

BEGIN_MESSAGE_MAP(CSchedulerApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CSchedulerApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// CSchedulerApp construction

CSchedulerApp::CSchedulerApp() noexcept
{
	m_bHiColorIcons = TRUE;


	m_nAppLook = 0;
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// If the application is built using Common Language Runtime support (/clr):
	//     1) This additional setting is needed for Restart Manager support to work properly.
	//     2) In your project, you must add a reference to System.Windows.Forms in order to build.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("scheduler.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CSchedulerApp object

CSchedulerApp theApp;


// CSchedulerApp initialization

BOOL CSchedulerApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("sa55231"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)


	InitContextMenuManager();
	InitShellManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CSchedulerDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CSchedulerView));
	if (!pDocTemplate)
		return FALSE;

	

	AddDocTemplate(pDocTemplate);


	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);


	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();
	return TRUE;
}

int CSchedulerApp::ExitInstance()
{
	//TODO: handle additional resources you may have added
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// CSchedulerApp message handlers

void CSchedulerApp::OnFileOpen()
{
	/*TCHAR szFilters[] = _T("Event Scheduler Files (*.ess)|*.ess|Event Scheduler Template Files (*.est)|*.est|All Files (*.*)|*.*||");
	CFileDialog fileDlg(TRUE, _T("ess"), _T("*.ess"), OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, szFilters);
	if (fileDlg.DoModal() == IDOK)
	{
		CFile oldFile;
		ASSERT(oldFile != NULL);
		
		oldFile.Open(fileDlg.GetPathName(), CFile::modeRead | CFile::shareExclusive);
		FilePathName = fileDlg.GetPathName();
		SetTitle(FilePathName);
		CArchive loadArchive(&oldFile, CArchive::load | CArchive::bNoFlushOnDelete); // Create the archive to load data, the archive must be closed manually after the loading process      
		Serialize(loadArchive);
		loadArchive.Close();
		oldFile.Close();
		UpdateAllViews(0);
	}*/
}


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMClickGpl30Syslink(NMHDR* pNMHDR, LRESULT* pResult);
};

CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL CAboutDlg::OnInitDialog()
{
	auto versionStatic = GetDlgItem(IDC_VERSION_STATIC);
	if (versionStatic)
	{
		DWORD dwHandle;			// ignored 
		CString name;
		name.Format(_T("%s.exe"), AfxGetApp()->m_pszExeName);
		DWORD versionSize = GetFileVersionInfoSize(name, &dwHandle);

		if (versionSize != 0)
		{
			auto buf = std::make_unique<std::byte[]>(versionSize);
			GetFileVersionInfo(name, NULL, versionSize, buf.get());
			struct LANGANDCODEPAGE {
				WORD wLanguage;
				WORD wCodePage;
			} *lpTranslate;
			UINT cbTranslate;
			VerQueryValue(buf.get(), TEXT("\\VarFileInfo\\Translation"), (LPVOID*)&lpTranslate, &cbTranslate);

			for (int i = 0; i < (cbTranslate / sizeof(struct LANGANDCODEPAGE)); i++)
			{
				TCHAR SubBlock[50] = { 0 };
				HRESULT hr = StringCchPrintf(SubBlock, ARRAYSIZE(SubBlock),
					_T("\\StringFileInfo\\%04x%04x\\ProductVersion"),
					lpTranslate[i].wLanguage,lpTranslate[i].wCodePage);
				ASSERT(SUCCEEDED(hr));

				LPVOID lpBuffer = nullptr;
				UINT dwBytes = 0;
				VerQueryValue(buf.get(), SubBlock, &lpBuffer, &dwBytes);
				CString versionString((TCHAR*)lpBuffer,dwBytes);
				versionStatic->SetWindowText(versionString);
				break;
			}
		}
	}

	return TRUE;
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_NOTIFY(NM_CLICK, IDC_GPL30_SYSLINK, &CAboutDlg::OnNMClickGpl30Syslink)
END_MESSAGE_MAP()

// App command to run the dialog
void CSchedulerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CSchedulerApp customization load/save methods

void CSchedulerApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
	bNameValid = strName.LoadString(IDS_REMOVE_EVENT);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_REMOVE_SCHEDULED_EVENT);	
	GetContextMenuManager()->AddMenu(_T("Tracks"), IDR_POPUP_TRACKS);
}

void CSchedulerApp::LoadCustomState()
{
}

void CSchedulerApp::SaveCustomState()
{
}

// CSchedulerApp message handlers


void CAboutDlg::OnNMClickGpl30Syslink(NMHDR* pNMHDR, LRESULT* pResult)
{
	PNMLINK pNMLink = (PNMLINK)pNMHDR;
	
	if (pNMLink->item.szUrl)
	{
		ShellExecute(NULL, L"open", pNMLink->item.szUrl, NULL, NULL, SW_SHOW);
	}

	*pResult = 0;
}
