#include "pch.h"
#include "CScheduleEventConstraintFactory.h"

CScheduleEventConstraintPtr CScheduleEventConstraintFactory::Create(ConstraintType type)
{
	switch (type)
	{
	case ConstraintType::UsageConstraint:
		break;
	default:
		break;
	}
	return CScheduleEventConstraintPtr();
}