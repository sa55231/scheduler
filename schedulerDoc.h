// schedulerDoc.h : interface of the CSchedulerDoc class
//


#pragma once

#include <vector>
#include <chrono>

#include "CScheduleTrack.h"
#include "CScheduleStockEvent.h"

#include <random>

class CSchedulerDoc : public CDocument
{
protected: // create from serialization only
	CSchedulerDoc() noexcept;
	DECLARE_DYNCREATE(CSchedulerDoc)

// Attributes
public:
	int GetUTCOffsetMinutes() const;
	void SetUTCOffsetMinutes(int offset, LPARAM lHint);
	std::chrono::system_clock::time_point GetStartTime() const;
	void SetStartTime(const std::chrono::system_clock::time_point& time, LPARAM lHint);
	const std::vector<CScheduleTrackPtr>& GetTracks() const;
	const std::vector<CScheduleStockEventPtr>& GetStockEvents() const;
	void UpdateStockEventName(int index, const CString& newName, LPARAM lHint);
	void UpdateTrackName(int index, const CString& newName, LPARAM lHint);
	void DeleteStockEvent(int index, LPARAM lHint);
	void DeleteTrack(int index, LPARAM lHint);
	int GetStockEventIndex(int id) const;
	CScheduleTrack* GetTrack(int id) const;
	CScheduleStockEvent* GetStockEvent(int id) const;
	CScheduleStockEvent* GetStockEventAtIndex(int index) const;
	CScheduleTrack* GetTrackAtIndex(int index) const;
	void AddTrackEventAtIndex(int stockEventIndex, const CString& trackName, int index, LPARAM lHint);
	void AddTrackEvent(int stockEventIndex, const CString& trackName, LPARAM lHint);
	void RemoveEventFromTrack(CScheduleTrack* track, CScheduleEvent* event);
	bool AreScheduledEvents() const;
	void RemoveAllScheduledEvents();
// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	void SetModifiedFlag(BOOL bModified = TRUE);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CSchedulerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

private:
	std::vector<CScheduleTrackPtr> tracks;
	std::vector<CScheduleStockEventPtr> stockEvents;
	std::chrono::system_clock::time_point startTime;
	int utcOffsetMinutes = 0;
	std::default_random_engine generator;
	std::uniform_int_distribution<int> color_distribution{ 0x0000FF, 0xCCCCFF };

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
