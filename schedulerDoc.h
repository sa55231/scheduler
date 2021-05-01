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

	const std::vector<CScheduleTrack>& GetTracks() const;

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
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
	std::vector<CScheduleTrack> tracks;
	std::vector<CScheduleStockEvent> stockEvents;
	std::chrono::steady_clock::time_point startTime;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
