#pragma once

#include <memory>
#include <vector>
#include "CScheduleEvent.h"

class CScheduleTrack
{
public:
	CScheduleTrack();
	CScheduleTrack(int id, CString name, std::vector<CScheduleEventPtr> events);
	void RemoveEvents(int id);
	CString GetName() const;
	int GetId() const;
	const std::vector<CScheduleEventPtr>& GetEvents() const;
	void AddEvent(CScheduleEventPtr event);
	void InsertEventAtIndex(int index, CScheduleEventPtr event);
	std::chrono::seconds GetTotalDuration() const;
	std::chrono::seconds GetMaxEventDuration() const;
private:
	int id = -1;
	CString name;
	std::vector<CScheduleEventPtr> events;
	std::chrono::seconds totalDuration;
	std::chrono::seconds maxEventDuration;
};

using CScheduleTrackPtr = std::unique_ptr<CScheduleTrack>;