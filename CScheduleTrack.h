#pragma once

#include <memory>
#include <vector>
#include "CScheduleEvent.h"

class CScheduleTrack
{
public:
	CScheduleTrack();
	CScheduleTrack(CString name, std::vector<CScheduleEventPtr> events);
	void RemoveEvents(int id);
	CString GetName() const;
	const std::vector<CScheduleEventPtr>& GetEvents() const;
	void AddEvent(CScheduleEventPtr event);
	void InsertEventAtIndex(int index, CScheduleEventPtr event);
	std::chrono::seconds GetTotalDuration() const;
	std::chrono::seconds GetMaxEventDuration() const;
private:
	CString name;
	std::vector<CScheduleEventPtr> events;
	std::chrono::seconds totalDuration;
	std::chrono::seconds maxEventDuration;
};

using CScheduleTrackPtr = std::unique_ptr<CScheduleTrack>;