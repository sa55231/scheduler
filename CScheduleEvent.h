#pragma once

#include <memory>
#include <chrono>

class CScheduleStockEvent;

class CScheduleEvent
{
public:
	CScheduleEvent(CScheduleStockEvent* eventSource, int id, int trackId);
	~CScheduleEvent();
	int GetId() const;
	int GetTrackId() const;
	int GetStockId() const;
	CString GetName() const;
	std::chrono::seconds GetDuration() const;
	UINT32 GetColor() const;
private:
	CScheduleStockEvent* stockEvent;
	int id = 0;
	int trackId = 0;
};

using CScheduleEventPtr = std::unique_ptr<CScheduleEvent>;