#pragma once

#include <memory>

class CScheduleStockEvent;
class CScheduleTrack;

enum class ConstraintType : int
{
	MaxCountConstraint = 0
};

class CScheduleEventConstraint
{
public:
	CScheduleEventConstraint();
	virtual ~CScheduleEventConstraint();

	virtual ConstraintType GetType() const = 0;
	virtual bool CanSchedule(const CScheduleStockEvent* event, const CScheduleTrack* track) const = 0;
	virtual void Serialize(CArchive& ar) = 0;
	virtual bool IsGlobal() const = 0;
	virtual void SetValue(const COleVariant& value) = 0;
};

using CScheduleEventConstraintPtr = std::unique_ptr<CScheduleEventConstraint>;