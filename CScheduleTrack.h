#pragma once

#include <vector>
#include "CScheduleEvent.h"

class CScheduleTrack
{
public:
	CScheduleTrack();
	CScheduleTrack(CString name, std::vector<CScheduleEvent> events);

	CString GetName() const;
	const std::vector<CScheduleEvent>& GetEvents() const;
	void AddEvent(CScheduleEvent event);
	std::chrono::seconds GetTotalDuration() const;
	std::chrono::seconds GetMaxEventDuration() const;
private:
	CString name;
	std::vector<CScheduleEvent> events;
	std::chrono::seconds totalDuration;
	std::chrono::seconds maxEventDuration;
};

