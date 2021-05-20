#pragma once

#include <memory>
#include <vector>
#include <chrono>
#include "CScheduleEvent.h"

class CScheduleTrack
{
public:
	CScheduleTrack();
	CScheduleTrack(int id, CString name, std::vector<CScheduleEventPtr> events);
	CScheduleTrack(int id, CString name, std::vector<CScheduleEventPtr> events, std::chrono::system_clock::time_point time);
	void RemoveEvents(int id);
	void RemoveEvent(CScheduleEvent* event);
	void RemoveAllEvents();
	CString GetName() const;
	void SetName(const CString& name);
	int GetId() const;
	const std::vector<CScheduleEventPtr>& GetEvents() const;
	void AddEvent(CScheduleEventPtr event);
	void InsertEventAtIndex(int index, CScheduleEventPtr event);
	std::chrono::seconds GetTotalDuration() const;
	std::chrono::seconds GetMaxEventDuration() const;
	std::chrono::system_clock::time_point GetStartTime() const;
	void SetStartTime(const std::chrono::system_clock::time_point& time);
	CScheduleEvent* GetEvent(int id) const;
private:
	int id = -1;
	CString name;
	std::vector<CScheduleEventPtr> events;
	std::chrono::seconds totalDuration;
	std::chrono::seconds maxEventDuration;
	std::chrono::system_clock::time_point startTime;
};

using CScheduleTrackPtr = std::unique_ptr<CScheduleTrack>;