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

// schedulerDoc.cpp : implementation of the CSchedulerDoc class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "scheduler.h"
#endif

#include "schedulerDoc.h"

#include <propkey.h>

#include <random>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static constexpr int32_t MAGIC = 0x3F3D72CB;

// CSchedulerDoc

IMPLEMENT_DYNCREATE(CSchedulerDoc, CDocument)

BEGIN_MESSAGE_MAP(CSchedulerDoc, CDocument)
END_MESSAGE_MAP()


// CSchedulerDoc construction/destruction

CSchedulerDoc::CSchedulerDoc() noexcept
{
}

CSchedulerDoc::~CSchedulerDoc()
{
}

BOOL CSchedulerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	tracks.clear();
	stockEvents.clear();
	startTime = std::chrono::steady_clock::now();

	std::default_random_engine generator;
	generator.seed(std::chrono::steady_clock::now().time_since_epoch().count());
	std::uniform_int_distribution<int> duration_distribution(60,5*3600);
	std::uniform_int_distribution<int> color_distribution(0x0000FF, 0xCCCCFF);
	for (int i = 0; i < 20; i++)
	{
		CString name;
		name.Format(_T("Event #%d"),i);
		std::chrono::seconds duration(duration_distribution(generator));
		stockEvents.emplace_back(i,std::move(name),std::move(duration), color_distribution(generator));
	}

	std::uniform_int_distribution<int> stock_distribution(0, (int)stockEvents.size()-1);
	std::uniform_int_distribution<int> event_count_distribution(2, 20);

	for (int i = 0; i < 20; i++)
	{
		CString name(_T("Track: "));
		name.AppendFormat(_T("%d"),i);
		std::vector<CScheduleEvent> events;
		int event_count = event_count_distribution(generator);
		for (int j = 0; j < event_count; j++)
		{
			events.emplace_back(stockEvents.at(stock_distribution(generator)));
		}

		tracks.emplace_back(name,events);
	}

	return TRUE;
}

const std::vector<CScheduleTrack>& CSchedulerDoc::GetTracks() const
{
	return tracks;
}


// CSchedulerDoc serialization

void CSchedulerDoc::Serialize(CArchive& ar)
{
TRY
{

	if (ar.IsStoring())
	{
		// TODO: add storing code here
		ar << MAGIC;
	}
	else
	{
		// TODO: add loading code here
		int32_t magic;
		ar >> magic;
		if (magic != MAGIC)
		{
			throw new CArchiveException(CArchiveException::badClass,_T("Bad file opened"));
		}
	}	
}
CATCH_ALL(e)
{
	// This will cause execution to continue to the next catch, in this 
	// case the one in the OnOpenDocument()
	THROW_LAST(); // dont just throw ! 
}
END_CATCH_ALL

}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CSchedulerDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CSchedulerDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data.
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CSchedulerDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CSchedulerDoc diagnostics

#ifdef _DEBUG
void CSchedulerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSchedulerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CSchedulerDoc commands
