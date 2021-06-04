#pragma once
#include "CScheduleEventConstraint.h"
class CEventMaxCountConstraint :
    public CScheduleEventConstraint
{
public:
	CEventMaxCountConstraint(int count);
	virtual ConstraintType GetType() const
	{
		return ConstraintType::MaxCountConstraint;
	}
	virtual bool CanSchedule(const CScheduleStockEvent* event, const CScheduleTrack* track) const;
	virtual void Serialize(CArchive& ar);
	virtual bool IsGlobal() const
	{
		return true;
	}
	int GetCount() const;
	void SetCount(int count);
	virtual void SetValue(const COleVariant& value);
private:
	int count;
};

