#pragma once

#include <memory>
#include <chrono>
#include <vector>

#include "CScheduleEventConstraint.h"
#include "CScheduleTrack.h"

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
	const std::vector<CScheduleEventConstraintPtr>& GetConstraints() const;
	void AddConstraint(CScheduleEventConstraintPtr constraint);
	void RemoveConstraint(CScheduleEventConstraint* constraint);
	bool CanScheduleGlobally() const;
	void RefreshGlobalSchedulingCapabilities(const std::vector<CScheduleTrackPtr>& tracks);
	bool CanSchedule(CScheduleTrack* track) const;
private:
	int id = 0;
	CString name;
	std::chrono::seconds duration;
	UINT32 color = 0;
	int usage = 0;
	bool canScheduleGlobally = false;
	std::vector<CScheduleEventConstraintPtr> constraints;
};

using CScheduleStockEventPtr = std::unique_ptr<CScheduleStockEvent>;