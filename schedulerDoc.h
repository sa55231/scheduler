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
	const std::vector<CScheduleTrackPtr>& GetTracks() const;
	const std::vector<CScheduleStockEventPtr>& GetStockEvents() const;
	void UpdateStockEventName(int index, const CString& newName, LPARAM lHint);
	CScheduleTrack* AddTrack(const CString& newName, LPARAM lHint);
	void UpdateTrackName(CScheduleTrack* track, const CString& newName, LPARAM lHint);
	void UpdateTrackStartTime(CScheduleTrack* track, const std::chrono::system_clock::time_point& time, LPARAM lHint);
	void DeleteTrack(CScheduleTrack* track, LPARAM lHint);

	void DeleteStockEvent(int index, LPARAM lHint);
	
	int GetStockEventIndex(int id) const;
	CScheduleTrack* GetTrack(int id) const;
	CScheduleEvent* GetEvent(int id) const;
	CScheduleStockEvent* GetStockEvent(int id) const;
	CScheduleStockEvent* GetStockEventAtIndex(int index) const;

	void AddTrackEventAtIndex(int stockEventIndex, const CString& trackName, int index, LPARAM lHint);
	void AddTrackEvent(int stockEventIndex, const CString& trackName, LPARAM lHint);

	void RemoveEventFromTrack(CScheduleTrack* track, CScheduleEvent* event);
	bool AreScheduledEvents() const;
	void RemoveAllScheduledEvents();
	double GetTimeScale() const;
	void SetTimeScale(double scale, LPARAM lHint);
	float GetZoomLevel() const;
	void SetZoomLevel(float zoom, LPARAM lHint);
	int GetNextEventId();
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
	
	int utcOffsetMinutes = 0;
	std::default_random_engine generator;
	std::uniform_int_distribution<int> color_distribution{ 0x0000FF, 0xCCCCFF };
	//how many seconds are per pixel displayed
	double timeScale = 60.0;
	float zoomLevel = 1.f;
	int nextEventId = 0;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
