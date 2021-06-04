#include "pch.h"
#include "CScheduleEventConstraintFactory.h"
#include "EventMaxCountConstraint.h"

CScheduleEventConstraintPtr CScheduleEventConstraintFactory::Create(ConstraintType type)
{
	switch (type)
	{
	case ConstraintType::MaxCountConstraint:
		return std::make_unique<CEventMaxCountConstraint>(0);
	default:
		break;
	}
	return CScheduleEventConstraintPtr();
}