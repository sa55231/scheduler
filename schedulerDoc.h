// schedulerDoc.h : interface of the CSchedulerDoc class
//


#pragma once

#include <vector>
#include <chrono>

#include "CScheduleTrack.h"
#include "CScheduleStockEvent.h"



class CSchedulerDoc : public CDocument
{
protected: // create from serialization only
	CSchedulerDoc() noexcept;
	DECLARE_DYNCREATE(CSchedulerDoc)

// Attributes
public:

	const std::vector<CScheduleTrackPtr>& GetTracks() const;
	const std::vector<CScheduleStockEventPtr>& GetStockEvents() const;
	void UpdateStockEventName(int index, const CString& newName, LPARAM lHint);
	void DeleteStockEvent(int index, LPARAM lHint);
	int GetStockEventIndex(int id) const;
	void AddTrackEventAtIndex(int stockEventIndex, const CString& trackName, int index);
	void AddTrackEvent(int stockEventIndex, const CString& trackName);
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
	std::chrono::steady_clock::time_point startTime;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
