#pragma once

#include "CScheduleEventConstraint.h"

class CScheduleEventConstraintFactory
{
public:
	static CScheduleEventConstraintPtr Create(ConstraintType type);
};

