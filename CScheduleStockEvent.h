#pragma once

#include <memory>
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
	int GetUsage() const;
	void IncrementUsage();
	void DecrementUsage();
	void SetUsage(int usage);
private:
	int id = 0;
	CString name;
	std::chrono::seconds duration;
	UINT32 color = 0;
	int usage = 0;
};

using CScheduleStockEventPtr = std::unique_ptr<CScheduleStockEvent>;