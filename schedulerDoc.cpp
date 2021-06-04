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
#include "utils.h"
#include "CScheduleEventConstraintFactory.h"

#include <propkey.h>

#include <random>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static constexpr uint32_t MAGIC = 0x3F3D72CB;
static constexpr uint32_t VERSION = 5;

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

	{
		LOGFONT* font;
		UINT length;
		if (theApp.GetProfileBinary(_T("Settings"), _T("DefaultEventsFont"), (BYTE**)&font, &length))
		{
			eventsFont = *font;
			delete font;
		}
		else
		{
			LOGFONT font = { 0 };
			font.lfHeight = -20;
			font.lfWeight = FW_NORMAL;
			_tcscpy_s(font.lfFaceName, _T("Times New Roman"));
			eventsFont = font;
		}
	}

	{
		LOGFONT* font;
		UINT length;
		if (theApp.GetProfileBinary(_T("Settings"), _T("DefaultTracksFont"), (BYTE**)&font, &length))
		{
			tracksFont = *font;
			delete font;
		}
		else
		{
			LOGFONT font = { 0 };
			font.lfHeight = -20;
			font.lfWeight = FW_NORMAL;
			_tcscpy_s(font.lfFaceName, _T("Times New Roman"));
			tracksFont = font;
		}
	}

	{
		LOGFONT* font;
		UINT length;
		if (theApp.GetProfileBinary(_T("Settings"), _T("DefaultHeadersFont"), (BYTE**)&font, &length))
		{
			headersFont = *font;
			delete font;
		}
		else
		{
			LOGFONT font = { 0 };
			font.lfHeight = -17;
			font.lfWeight = FW_NORMAL;
			_tcscpy_s(font.lfFaceName, _T("Times New Roman"));
			headersFont = font;
		}
	}


	
#ifdef _DEBUG
	std::uniform_int_distribution<int> duration_distribution(2*3600, 20 * 3600);
	for (int i = 0; i < 20; i++)
	{
		CString name;
		name.Format(_T("Event #%d"), i);
		std::chrono::seconds duration(duration_distribution(generator));
		auto event = AddEvent(name,0);
		event->SetDuration(std::move(duration));
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
	RefreshEventsSchedulingCapabilities();
	SetModifiedFlag(FALSE);
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
size_t CSchedulerDoc::GetScheduledEventsCount() const
{
	size_t count = 0;
	for (const auto& track : tracks)
	{
		count += track->GetEvents().size();
	}
	return count;
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
	std::sort(stockEvents.begin(), stockEvents.end(), [sortedColumn, sortAscending](auto& s1, auto& s2) -> bool {
		EventsViewColumns col = (EventsViewColumns)sortedColumn;

		switch (col)
		{
		case EventsViewColumns::Name:
			return sortAscending ? s1->GetName() < s2->GetName() : s1->GetName() > s2->GetName();
		case EventsViewColumns::Duration:
			return sortAscending ? s1->GetDuration() < s2->GetDuration() : s1->GetDuration() > s2->GetDuration();
		case EventsViewColumns::Color:
			return sortAscending ? s1->GetColor() < s2->GetColor() : s1->GetColor() > s2->GetColor();
		case EventsViewColumns::Usage:
			return sortAscending ? s1->GetUsage() < s2->GetUsage() : s1->GetUsage() > s2->GetUsage();
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
	RefreshEventsSchedulingCapabilities();
	SetModifiedFlag(TRUE);
	UpdateAllViews(nullptr, -1);
}
void CSchedulerDoc::AddTrackEventAtIndex(int stockEventId, CScheduleTrack* track, int index, LPARAM lHint)
{	
	auto stockEvent = GetStockEvent(stockEventId);

	track->InsertEventAtIndex(index, std::make_unique<CScheduleEvent>(stockEvent, GetNextEventId(), track->GetId()));
	RefreshEventsSchedulingCapabilities();
	SetModifiedFlag(TRUE);
	UpdateAllViews(nullptr, lHint);
}
void CSchedulerDoc::AddTrackEvent(int stockEventId, CScheduleTrack* track, LPARAM lHint)
{
	auto stockEvent = GetStockEvent(stockEventId);

	track->AddEvent(std::make_unique<CScheduleEvent>(stockEvent, GetNextEventId(), track->GetId()));
	RefreshEventsSchedulingCapabilities();
	SetModifiedFlag(TRUE);
	UpdateAllViews(nullptr, lHint);
}
void CSchedulerDoc::RemoveEventFromTrack(CScheduleTrack* track, CScheduleEvent* event)
{
	track->RemoveEvent(event);
	RefreshEventsSchedulingCapabilities();
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
		for (auto&& track : tracks)
		{
			track->RemoveEvents(eventId);
		}
		stockEvents.erase(it);
		SetModifiedFlag(TRUE);
		UpdateAllViews(nullptr, lHint);
	}
}
CScheduleStockEvent* CSchedulerDoc::AddEvent(const CString& newName, LPARAM lHint)
{
	auto maxIt = std::max_element(stockEvents.begin(), stockEvents.end(), [](const auto& tr1, const auto& tr2) -> bool {
		return tr1->GetId() < tr2->GetId();
		});
	int id = 0;
	if (maxIt != stockEvents.end())
	{
		id = (*maxIt)->GetId() + 1;
	}
	std::chrono::seconds duration(2*3600);
	stockEvents.emplace_back(std::make_unique<CScheduleStockEvent>(id, newName, std::move(duration), color_distribution(generator)));
	stockEvents.back()->AddConstraint(CScheduleEventConstraintFactory::Create(ConstraintType::MaxCountConstraint));
	RefreshEventsSchedulingCapabilities();
	SetModifiedFlag(TRUE);
	UpdateAllViews(nullptr, lHint);

	return stockEvents.back().get();
}
void CSchedulerDoc::RefreshEventsSchedulingCapabilities()
{
	for (auto&& event : stockEvents)
	{
		event->RefreshGlobalSchedulingCapabilities(tracks);
	}
}
CScheduleTrack* CSchedulerDoc::AddTrack(const CString& newName, LPARAM lHint)
{
	std::vector<CScheduleEventPtr> events;
	auto maxIt = std::max_element(tracks.begin(), tracks.end(), [](const auto& tr1,const auto& tr2) -> bool {
		return tr1->GetId() < tr2->GetId();
	});
	int id = 0;
	if (maxIt != tracks.end())
	{
		id = (*maxIt)->GetId() + 1;
	}
	tracks.emplace_back(std::make_unique<CScheduleTrack>(id,newName,std::move(events),std::chrono::system_clock::now()));

	RefreshEventsSchedulingCapabilities();
	SetModifiedFlag(TRUE);
	UpdateAllViews(nullptr, lHint);
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
		RefreshEventsSchedulingCapabilities();
		SetModifiedFlag(TRUE);
		UpdateAllViews(nullptr, lHint);
	}
}
void CSchedulerDoc::UpdateTrackStartTime(CScheduleTrack* track, const std::chrono::system_clock::time_point& time, LPARAM lHint)
{
	track->SetStartTime(time);
	RefreshEventsSchedulingCapabilities();
	SetModifiedFlag(TRUE);
	UpdateAllViews(nullptr, lHint);
}
void CSchedulerDoc::UpdateTrackName(CScheduleTrack* track, const CString& newName, LPARAM lHint)
{
	track->SetName(newName);
	SetModifiedFlag(TRUE);
	UpdateAllViews(nullptr, lHint);
}
void CSchedulerDoc::UpdateStockEventDuration(CScheduleStockEvent* event, const std::chrono::seconds& newDuration, LPARAM lHint)
{
	event->SetDuration(newDuration);
	RefreshEventsSchedulingCapabilities();
	SetModifiedFlag(TRUE);
	UpdateAllViews(nullptr, lHint);
}
void CSchedulerDoc::UpdateStockEventConstraint(CScheduleStockEvent* event, ConstraintType type, const COleVariant& value, LPARAM lHint)
{
	for (const auto& c : event->GetConstraints())
	{
		if (c->GetType() == type)
		{
			c->SetValue(value);
		}
	}
	
	RefreshEventsSchedulingCapabilities();
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
			ar << ev->GetConstraints().size();
			for (const auto& c : ev->GetConstraints())
			{
				ar << (int)c->GetType();
				c->Serialize(ar);
			}
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
		ar.Write(&eventsFont, sizeof(eventsFont));
		ar.Write(&tracksFont, sizeof(tracksFont));
		ar.Write(&headersFont, sizeof(headersFont));
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
			std::chrono::seconds::rep secondsRaw;
			UINT32 color;
			ar >> id;
			ar >> name;
			ar >> secondsRaw;
			ar >> color;
			std::chrono::seconds duration(secondsRaw);
			auto event = std::make_unique<CScheduleStockEvent>(id, std::move(name), std::move(duration), color);
			if (version >= 5)
			{
				size_t constraintsSize = 0;
				ar >> constraintsSize;
				for (size_t j = 0; j < constraintsSize; ++j)
				{
					int type;
					ar >> type;
					auto c = CScheduleEventConstraintFactory::Create((ConstraintType)type);
					if (c)
					{
						c->Serialize(ar);
						event->AddConstraint(std::move(c));
					}
				}
			}
			newStockEvents.push_back(std::move(event));
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
		if (version > 3)
		{
			ar.Read(&eventsFont, sizeof(eventsFont));
			ar.Read(&tracksFont, sizeof(tracksFont));
			ar.Read(&headersFont, sizeof(headersFont));
		}

		tracks.clear();
		stockEvents.clear();
		
		utcOffsetMinutes = utcOffset;
		tracks = std::move(newTracks);
		stockEvents = std::move(newStockEvents);

		RefreshEventsSchedulingCapabilities();
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

void CSchedulerDoc::SetEventsFont(LOGFONT font, LPARAM lHint)
{
	eventsFont = std::move(font);
	SetModifiedFlag(TRUE);
	UpdateAllViews(nullptr, lHint);
}
LOGFONT CSchedulerDoc::GetEventsFont() const
{
	return eventsFont;
}
void CSchedulerDoc::SetTracksFont(LOGFONT font, LPARAM lHint)
{
	tracksFont = std::move(font);
	SetModifiedFlag(TRUE);
	UpdateAllViews(nullptr, lHint);
}
LOGFONT CSchedulerDoc::GetTracksFont() const
{
	return tracksFont;
}
void CSchedulerDoc::SetHeadersFont(LOGFONT font, LPARAM lHint)
{
	headersFont = std::move(font);
	SetModifiedFlag(TRUE);
	UpdateAllViews(nullptr, lHint);
}
LOGFONT CSchedulerDoc::GetHeadersFont() const
{
	return headersFont;
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
