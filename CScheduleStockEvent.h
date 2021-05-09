#pragma once

#include <chrono>

class CScheduleStockEvent
{
public:
	CScheduleStockEvent();
	CScheduleStockEvent(int id, CString name, std::chrono::seconds duration, UINT32 color);
	int GetId() const;
	CString GetName() const;
	std::chrono::seconds GetDuration() const;
	UINT32 GetColor() const;
	void SetName(const CString& newName);
	void SetColor(UINT32 newColor);
	void SetDuration(const std::chrono::seconds& newDuration);
private:
	int id = 0;
	CString name;
	std::chrono::seconds duration;
	UINT32 color = 0;
};

