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
static constexpr uint32_t VERSION = 3;

// CSchedulerDoc

IMPLEMENT_DYNCREATE(CSchedulerDoc, CDocument)

BEGIN_MESSAGE_MAP(CSchedulerDoc, CDocument)
END_MESSAGE_MAP()


// CSchedulerDoc construction/destruction

CSchedulerDoc::CSchedulerDoc() noexcept
{
	generator.seed((unsigned int)std::chrono::steady_clock::now().time_since_epoch().count());
}

CSchedulerDoc::~CSchedulerDoc()
{
}
int CSchedulerDoc::GetNextEventId()
{
	return nextEventId++;
}
BOOL CSchedulerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	tracks.clear();
	stockEvents.clear();
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
	
#ifdef _DEBUG
	std::uniform_int_distribution<int> duration_distribution(60, 5 * 3600);
	for (int i = 0; i < 20; i++)
	{
		CString name;
		name.Format(_T("Event #%d"), i);
		std::chrono::seconds duration(duration_distribution(generator));
		stockEvents.emplace_back(std::make_unique<CScheduleStockEvent>(i, std::move(name), std::move(duration), color_distribution(generator)));
	}

	std::uniform_int_distribution<int> stock_distribution(0, (int)stockEvents.size() - 1);
	std::uniform_int_distribution<int> event_count_distribution(0, 20);

	for (int i = 0; i < 20; i++)
	{
		CString name(_T("Track: "));
		name.AppendFormat(_T("%d"), i);
		std::vector<CScheduleEventPtr> events;
		int event_count = event_count_distribution(generator);
		for (int j = 0; j < event_count; j++)
		{
			events.emplace_back(std::make_unique<CScheduleEvent>(stockEvents.at(stock_distribution(generator)).get(),GetNextEventId(),i));
		}

		tracks.emplace_back(std::make_unique<CScheduleTrack>(i, name, std::move(events)));
	}
	
#endif //_DEBUG

	AfxGetMainWnd()->PostMessage(WM_DOCUMENT_LOADED, 0, (LPARAM)this);
	return TRUE;
}

double CSchedulerDoc::GetTimeScale() const
{
	return timeScale;
}
void CSchedulerDoc::SetTimeScale(double scale, LPARAM lHint)
{
	timeScale = scale;
	SetModifiedFlag(TRUE);
	UpdateAllViews(nullptr, lHint);
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
float CSchedulerDoc::GetZoomLevel() const
{
	return zoomLevel;
}
void CSchedulerDoc::SetZoomLevel(float zoom, LPARAM lHint)
{
	zoomLevel = zoom;
	SetModifiedFlag(TRUE);
	UpdateAllViews(nullptr, lHint);
}

std::chrono::system_clock::time_point CSchedulerDoc::GetStartTime() const
{
	std::chrono::system_clock::time_point min = std::chrono::system_clock::time_point::max();
	for (const auto& track : tracks)
	{
		if (track->GetStartTime() < min)
		{
			min = track->GetStartTime();
		}
	}
	return min;
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
bool CSchedulerDoc::AreScheduledEvents() const
{
	for (const auto& track : tracks)
	{
		if (!track->GetEvents().empty()) return true;
	}
	return false;
}
void CSchedulerDoc::SortStockEvents(int sortedColumn, bool sortAscending)
{
	// sortedColumn: 0 - name, 1 - duration, 2 - color
	std::sort(stockEvents.begin(), stockEvents.end(), [sortedColumn, sortAscending](auto& s1, auto& s2) -> bool {
		switch (sortedColumn)
		{
		case 0:
			return sortAscending ? s1->GetName() < s2->GetName() : s1->GetName() > s2->GetName();
		case 1:
			return sortAscending ? s1->GetDuration() < s2->GetDuration() : s1->GetDuration() > s2->GetDuration();
		case 2:
			return sortAscending ? s1->GetColor() < s2->GetColor() : s1->GetColor() > s2->GetColor();
		default:
			return true;
		}
	});
}
void CSchedulerDoc::RemoveAllScheduledEvents()
{
	for (auto&& track : tracks)
	{
		track->RemoveAllEvents();
	}
	SetModifiedFlag(TRUE);
	UpdateAllViews(nullptr, -1);
}
void CSchedulerDoc::AddTrackEventAtIndex(int stockEventId, const CString& trackName, int index, LPARAM lHint)
{	
	auto stockEvent = GetStockEvent(stockEventId);

	auto trackIt = std::find_if(tracks.begin(), tracks.end(), [trackName](const auto& track) {
		return track->GetName() == trackName;
	});
	if (trackIt != tracks.end())
	{
		(*trackIt)->InsertEventAtIndex(index, std::make_unique<CScheduleEvent>(stockEvent, GetNextEventId(), (*trackIt)->GetId()));
	}
	SetModifiedFlag(TRUE);
	UpdateAllViews(nullptr, lHint);
}
void CSchedulerDoc::AddTrackEvent(int stockEventId, const CString& trackName, LPARAM lHint)
{
	auto stockEvent = GetStockEvent(stockEventId);

	auto trackIt = std::find_if(tracks.begin(), tracks.end(), [trackName](const auto& track) {
		return track->GetName() == trackName;
	});
	if (trackIt != tracks.end())
	{
		(*trackIt)->AddEvent(std::make_unique<CScheduleEvent>(stockEvent, GetNextEventId(), (*trackIt)->GetId()));
	}
	SetModifiedFlag(TRUE);
	UpdateAllViews(nullptr, lHint);
}
void CSchedulerDoc::RemoveEventFromTrack(CScheduleTrack* track, CScheduleEvent* event)
{
	track->RemoveEvent(event);
	SetModifiedFlag(TRUE);
	UpdateAllViews(nullptr, -1);
}

/*CScheduleStockEvent* CSchedulerDoc::GetStockEventAtIndex(int index) const
{
	if (index >= 0 && index < stockEvents.size())
	{
		return stockEvents.at(index).get();
	}
	return nullptr;
}*/

CScheduleEvent* CSchedulerDoc::GetEvent(int id) const
{
	for (const auto& track : tracks)
	{
		auto event = track->GetEvent(id);
		if (event) return event;
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
/*int CSchedulerDoc::GetStockEventIndex(int id) const
{
	auto it = std::find_if(stockEvents.begin(), stockEvents.end(), [id](const auto& ev) {
		return ev->GetId() == id;
	});
	if (it == stockEvents.end()) return -1;
	return (*it)->GetId();
}*/

void CSchedulerDoc::DeleteStockEvent(CScheduleStockEvent* event, LPARAM lHint)
{
	int eventId = event->GetId();
	auto it = std::find_if(stockEvents.begin(), stockEvents.end(), [event](const auto& ev) {
		return ev.get() == event;
	});
	if (it != stockEvents.end())
	{
		stockEvents.erase(it);
		for (auto&& track : tracks)
		{
			track->RemoveEvents(eventId);
		}
		SetModifiedFlag(TRUE);
		UpdateAllViews(nullptr, lHint);
	}
}
CScheduleStockEvent* CSchedulerDoc::AddEvent(const CString& newName, LPARAM lHint)
{
	auto maxIt = std::max_element(stockEvents.begin(), stockEvents.end(), [](const auto& tr1, const auto& tr2) -> bool {
		return tr1->GetId() < tr2->GetId();
		});

	std::chrono::seconds duration(3600);
	stockEvents.emplace_back(std::make_unique<CScheduleStockEvent>((*maxIt)->GetId() + 1, newName, std::move(duration), color_distribution(generator)));
	
	return stockEvents.back().get();
}
CScheduleTrack* CSchedulerDoc::AddTrack(const CString& newName, LPARAM lHint)
{
	std::vector<CScheduleEventPtr> events;
	auto maxIt = std::max_element(tracks.begin(), tracks.end(), [](const auto& tr1,const auto& tr2) -> bool {
		return tr1->GetId() < tr2->GetId();
	});
	tracks.emplace_back(std::make_unique<CScheduleTrack>((*maxIt)->GetId()+1,newName,std::move(events),std::chrono::system_clock::now()));
	return tracks.back().get();
}

void CSchedulerDoc::DeleteTrack(CScheduleTrack* track, LPARAM lHint)
{
	auto it = std::find_if(tracks.begin(), tracks.end(), [track](const auto& ev) {
		return ev.get() == track;
	});
	if (it != tracks.end())
	{
		tracks.erase(it);
		SetModifiedFlag(TRUE);
		UpdateAllViews(nullptr, lHint);
	}
}
void CSchedulerDoc::UpdateTrackStartTime(CScheduleTrack* track, const std::chrono::system_clock::time_point& time, LPARAM lHint)
{
	track->SetStartTime(time);
	SetModifiedFlag(TRUE);
	UpdateAllViews(nullptr, lHint);
}
void CSchedulerDoc::UpdateTrackName(CScheduleTrack* track, const CString& newName, LPARAM lHint)
{
	track->SetName(newName);
	SetModifiedFlag(TRUE);
	UpdateAllViews(nullptr, lHint);
}
void CSchedulerDoc::UpdateStockEventName(CScheduleStockEvent* event, const CString& newName, LPARAM lHint)
{
	event->SetName(newName);
	SetModifiedFlag(TRUE);
	UpdateAllViews(nullptr, lHint);
}
void CSchedulerDoc::SetModifiedFlag(BOOL bModified)
{
	CDocument::SetModifiedFlag(bModified);
	UpdateFrameCounts();
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
			ar << tr->GetStartTime().time_since_epoch().count();
			ar << tr->GetEvents().size();
			for (const auto& ev : tr->GetEvents())
			{
				ar << ev->GetStockId();
				ar << ev->GetId();
			}
		}
		ar << timeScale;
		ar << zoomLevel;
	}
	else
	{
		uint32_t magic;
		ar >> magic;
		if (magic != MAGIC)
		{
			throw new CArchiveException(CArchiveException::badClass,_T("Bad file opened"));
		}
		uint32_t version = 0;
		ar >> version;
		if (version < 1 || version > VERSION)
		{
			throw new CArchiveException(CArchiveException::badSchema, _T("Document is corrupted"));
		}

		int utcOffset;
		ar >> utcOffset;

		//first 2 versions had the start time document wide
		std::chrono::system_clock::time_point::duration::rep documentStartTime;
		if (version < 3)
		{
			ar >> documentStartTime;
		}

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
			std::chrono::system_clock::time_point::duration::rep trackStartTime;
			ar >> id;
			ar >> name;
			if (version >= 3)
			{
				ar >> trackStartTime;
			}
			else
			{
				trackStartTime = documentStartTime;
			}
			ar >> eventsSize;			
			std::vector<CScheduleEventPtr> events;
			for (size_t j = 0; j < eventsSize; j++)
			{
				int stockId;
				ar >> stockId;
				auto it = std::find_if(newStockEvents.begin(), newStockEvents.end(), [stockId](const auto& ev) {
					return ev->GetId() == stockId;
				});
				int eventId;
				if (version >= 3)
				{
					ar >> eventId;
					if (nextEventId < eventId)
					{
						nextEventId = eventId + 1;
					}
				}
				else
				{
					eventId = GetNextEventId();
				}
				if (it == newStockEvents.end())
				{
					throw new CArchiveException(CArchiveException::badSchema, _T("The document is corrupted"));
				}

				events.emplace_back(std::make_unique<CScheduleEvent>(it->get(),eventId,id));
			}
			newTracks.emplace_back(std::make_unique<CScheduleTrack>(id, name, std::move(events), 
				std::chrono::system_clock::time_point(std::chrono::system_clock::time_point::duration(trackStartTime))));
		}

		if (version > 1)
		{
			ar >> timeScale;
			ar >> zoomLevel;
		}

		tracks.clear();
		stockEvents.clear();
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
