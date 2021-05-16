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

static constexpr uint32_t MAGIC = 0x3F3D72CB;
static constexpr uint32_t VERSION = 1;

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
	startTime = std::chrono::system_clock::now();
	DYNAMIC_TIME_ZONE_INFORMATION timezoneInfo = { 0 };
	auto tzInfo = GetDynamicTimeZoneInformation(&timezoneInfo);
	utcOffsetMinutes = timezoneInfo.Bias;
	if (tzInfo == TIME_ZONE_ID_DAYLIGHT)
	{
		utcOffsetMinutes += timezoneInfo.DaylightBias;
	}
	else if (tzInfo == TIME_ZONE_ID_STANDARD || tzInfo == TIME_ZONE_ID_UNKNOWN)
	{
		utcOffsetMinutes += timezoneInfo.StandardBias;
	}
	std::default_random_engine generator;
	generator.seed((unsigned int)std::chrono::steady_clock::now().time_since_epoch().count());
	std::uniform_int_distribution<int> duration_distribution(60,5*3600);
	std::uniform_int_distribution<int> color_distribution(0x0000FF, 0xCCCCFF);
	for (int i = 0; i < 20; i++)
	{
		CString name;
		name.Format(_T("Event #%d"),i);
		std::chrono::seconds duration(duration_distribution(generator));
		stockEvents.emplace_back(std::make_unique<CScheduleStockEvent>(i,std::move(name),std::move(duration), color_distribution(generator)));
	}

	std::uniform_int_distribution<int> stock_distribution(0, (int)stockEvents.size()-1);
	std::uniform_int_distribution<int> event_count_distribution(0, 20);

	for (int i = 0; i < 20; i++)
	{
		CString name(_T("Track: "));
		name.AppendFormat(_T("%d"),i);
		std::vector<CScheduleEventPtr> events;
		int event_count = event_count_distribution(generator);
		for (int j = 0; j < event_count; j++)
		{
			events.emplace_back(std::make_unique<CScheduleEvent>(stockEvents.at(stock_distribution(generator)).get()));
		}

		tracks.emplace_back(std::make_unique<CScheduleTrack>(i,name,std::move(events)));
	}

	AfxGetMainWnd()->PostMessage(WM_DOCUMENT_LOADED, 0, (LPARAM)this);
	return TRUE;
}

int CSchedulerDoc::GetUTCOffsetMinutes() const
{
	return utcOffsetMinutes;
}
void CSchedulerDoc::SetUTCOffsetMinutes(int offset, LPARAM lHint)
{
	utcOffsetMinutes = offset;
	SetModifiedFlag(TRUE);
	UpdateAllViews(nullptr, lHint);
}

std::chrono::system_clock::time_point CSchedulerDoc::GetStartTime() const
{
	return startTime;
}
void CSchedulerDoc::SetStartTime(const std::chrono::system_clock::time_point& time, LPARAM lHint)
{
	startTime = time;
	SetModifiedFlag(TRUE);
	UpdateAllViews(nullptr, lHint);
}
CScheduleTrack* CSchedulerDoc::GetTrack(int id) const
{
	auto it = std::find_if(tracks.begin(), tracks.end(), [id](const auto& track) {
		return track->GetId() == id;
		});
	if (it == tracks.end()) return nullptr;
	return it->get();
}
const std::vector<CScheduleTrackPtr>& CSchedulerDoc::GetTracks() const
{
	return tracks;
}
const std::vector<CScheduleStockEventPtr>& CSchedulerDoc::GetStockEvents() const
{
	return stockEvents;
}

void CSchedulerDoc::AddTrackEventAtIndex(int stockEventIndex, const CString& trackName, int index, LPARAM lHint)
{
	if (stockEventIndex < 0 || stockEventIndex >= stockEvents.size()) return;

	auto trackIt = std::find_if(tracks.begin(), tracks.end(), [trackName](const auto& track) {
		return track->GetName() == trackName;
	});
	if (trackIt != tracks.end())
	{
		(*trackIt)->InsertEventAtIndex(index, std::make_unique<CScheduleEvent>(stockEvents.at(stockEventIndex).get()));
	}
	SetModifiedFlag(TRUE);
	UpdateAllViews(nullptr, lHint);
}
void CSchedulerDoc::AddTrackEvent(int stockEventIndex, const CString& trackName, LPARAM lHint)
{
	if (stockEventIndex < 0 || stockEventIndex >= stockEvents.size()) return;

	auto trackIt = std::find_if(tracks.begin(), tracks.end(), [trackName](const auto& track) {
		return track->GetName() == trackName;
	});
	if (trackIt != tracks.end())
	{
		(*trackIt)->AddEvent(std::make_unique<CScheduleEvent>(stockEvents.at(stockEventIndex).get()));
	}
	SetModifiedFlag(TRUE);
	UpdateAllViews(nullptr, lHint);
}
CScheduleStockEvent* CSchedulerDoc::GetStockEventAtIndex(int index) const
{
	if (index >= 0 && index < stockEvents.size())
	{
		return stockEvents.at(index).get();
	}
	return nullptr;
}
CScheduleStockEvent* CSchedulerDoc::GetStockEvent(int id) const
{
	auto it = std::find_if(stockEvents.begin(), stockEvents.end(), [id](const auto& ev) {
		return ev->GetId() == id;
		});
	if (it == stockEvents.end()) return nullptr;
	return it->get();
}
int CSchedulerDoc::GetStockEventIndex(int id) const
{
	auto it = std::find_if(stockEvents.begin(), stockEvents.end(), [id](const auto& ev) {
		return ev->GetId() == id;
	});
	if (it == stockEvents.end()) return -1;
	return (*it)->GetId();
}

void CSchedulerDoc::DeleteStockEvent(int index, LPARAM lHint)
{
	ASSERT(index >= 0 && index < stockEvents.size());
	
	int id = stockEvents.at(index)->GetId();
	for (auto&& track : tracks)
	{
		track->RemoveEvents(id);
	}
	stockEvents.erase(stockEvents.begin() + index);

	SetModifiedFlag(TRUE);
	UpdateAllViews(nullptr, lHint);
}

void CSchedulerDoc::UpdateStockEventName(int index, const CString& newName, LPARAM lHint)
{
	ASSERT(index>=0 && index<stockEvents.size()  + 1);
	if (index == stockEvents.size())
	{
		std::chrono::seconds duration(3600);
		stockEvents.emplace_back(std::make_unique<CScheduleStockEvent>(index, newName, std::move(duration), 0x0000FF));
	}
	else
	{
		stockEvents.at(index)->SetName(newName);
	}
	SetModifiedFlag(TRUE);
	UpdateAllViews(nullptr, lHint);
}
void CSchedulerDoc::SetModifiedFlag(BOOL bModified)
{
	if (bModified != IsModified())
	{
		CDocument::SetModifiedFlag(bModified);

		CString strTitle = GetTitle();
		if (bModified)
		{
			// Add '*'
			strTitle += "*";
		}
		else
		{
			// Remove '*'
			strTitle = strTitle.Left(strTitle.GetLength() - 1);
		}
		SetTitle(strTitle);

		// Update Window title/Tab content
		POSITION pos = GetFirstViewPosition();
		if (pos != nullptr)
			GetNextView(pos)->GetParentFrame()->OnUpdateFrameTitle(TRUE);
	}
}
// CSchedulerDoc serialization

void CSchedulerDoc::Serialize(CArchive& ar)
{
TRY
{
	if (ar.IsStoring())
	{	
		ar << MAGIC;
		ar << VERSION;
		ar << utcOffsetMinutes;
		ar << startTime.time_since_epoch().count();
		ar << stockEvents.size();
		for (const auto& ev : stockEvents)
		{
			ar << ev->GetId();
			ar << ev->GetName();
			ar << ev->GetDuration().count();
			ar << ev->GetColor();
		}
		ar << tracks.size();
		for (const auto& tr : tracks)
		{
			ar << tr->GetId();
			ar << tr->GetName();
			ar << tr->GetEvents().size();
			for (const auto& ev : tr->GetEvents())
			{
				ar << ev->GetStockId();
			}
		}
	}
	else
	{
		uint32_t magic;
		ar >> magic;
		if (magic != MAGIC)
		{
			UpdateAllViews(nullptr, 0);
			throw new CArchiveException(CArchiveException::badClass,_T("Bad file opened"));
		}
		uint32_t version = 0;
		ar >> version;
		if (version != VERSION)
		{
			//we're loading a different version than the current one
			// TODO:: Add version handling code here, for now we just yell,
			// since we are only at version 1
			UpdateAllViews(nullptr, 0);
			throw new CArchiveException(CArchiveException::badSchema, _T("Document is corrupted"));
		}

		int utcOffset;
		ar >> utcOffset;
		std::chrono::system_clock::time_point::duration::rep rep;
		ar >> rep;		
		size_t stockEventsSize = 0;
		ar >> stockEventsSize;

		std::vector<CScheduleTrackPtr> newTracks;
		std::vector<CScheduleStockEventPtr> newStockEvents;

		for (size_t i = 0; i < stockEventsSize; ++i)
		{
			int id;
			CString name;
			long long secondsRaw;
			UINT32 color;
			ar >> id;
			ar >> name;
			ar >> secondsRaw;
			ar >> color;
			std::chrono::seconds duration(secondsRaw);
			newStockEvents.emplace_back(std::make_unique<CScheduleStockEvent>(id, std::move(name), std::move(duration), color));
		}
		size_t tracksSize = 0;
		ar >> tracksSize;
		for (size_t i = 0; i < tracksSize; ++i)
		{
			int id;
			CString name;
			size_t eventsSize = 0;
			ar >> id;
			ar >> name;
			ar >> eventsSize;			
			std::vector<CScheduleEventPtr> events;
			for (size_t j = 0; j < eventsSize; j++)
			{
				int stockId;
				ar >> stockId;
				auto it = std::find_if(newStockEvents.begin(), newStockEvents.end(), [stockId](const auto& ev) {
					return ev->GetId() == stockId;
				});
				if (it == newStockEvents.end())
				{
					throw new CArchiveException(CArchiveException::badSchema, _T("The document is corrupted"));
				}

				events.emplace_back(std::make_unique<CScheduleEvent>(it->get()));
			}
			newTracks.emplace_back(std::make_unique<CScheduleTrack>(id, name, std::move(events)));
		}

		tracks.clear();
		stockEvents.clear();
		startTime = std::chrono::system_clock::time_point(std::chrono::system_clock::time_point::duration(rep));
		utcOffsetMinutes = utcOffset;
		tracks = std::move(newTracks);
		stockEvents = std::move(newStockEvents);

		AfxGetMainWnd()->PostMessage(WM_DOCUMENT_LOADED, 0, (LPARAM)this);
		UpdateAllViews(nullptr, 0);
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
