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
	int stockId = 0;
	CString name;
	std::chrono::seconds duration;
	UINT32 color = 0;
};

