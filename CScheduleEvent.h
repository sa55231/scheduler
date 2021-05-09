#pragma once

#include <chrono>
#include "CScheduleStockEvent.h"

class CScheduleEvent
{
public:
	CScheduleEvent(const CScheduleStockEvent& eventSource);
	int GetStockId() const;
	CString GetName() const;
	std::chrono::seconds GetDuration() const;
	UINT32 GetColor() const;
private:
	const CScheduleStockEvent* stockEvent;
};

