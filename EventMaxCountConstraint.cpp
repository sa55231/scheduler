#include "pch.h"
#include "EventMaxCountConstraint.h"
#include "CScheduleStockEvent.h"

CEventMaxCountConstraint::CEventMaxCountConstraint(int count):count(count)
{
}

int CEventMaxCountConstraint::GetCount() const
{
	return count;
}
void CEventMaxCountConstraint::SetCount(int count)
{
	this->count = count;
}

bool CEventMaxCountConstraint::CanSchedule(const CScheduleStockEvent* event, const CScheduleTrack* track) const
{
	if (count > 0)
	{
		return event->GetUsage() < count;
	}
	return true;
}
void CEventMaxCountConstraint::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << count;
	}
	else
	{
		ar >> count;
	}
}

void CEventMaxCountConstraint::SetValue(const COleVariant& value)
{
	SetCount(value.intVal);
}