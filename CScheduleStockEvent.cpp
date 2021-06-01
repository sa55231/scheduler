#include "pch.h"
#include "CScheduleStockEvent.h"

CScheduleStockEvent::CScheduleStockEvent() :duration{ 0 }
{
}

CScheduleStockEvent::CScheduleStockEvent(int id, CString name, std::chrono::seconds duration, UINT32 color):
	id(id),name(std::move(name)),duration(std::move(duration)),color(color)
{

}

int CScheduleStockEvent::GetId() const
{
	return id;
}
CString CScheduleStockEvent::GetName() const
{
	return name;
}
std::chrono::seconds CScheduleStockEvent::GetDuration() const
{
	return duration;
}
UINT32 CScheduleStockEvent::GetColor() const
{
	return color;
}	

void CScheduleStockEvent::SetName(const CString& newName)
{
	name = newName;
}
void CScheduleStockEvent::SetColor(UINT32 newColor)
{
	color = newColor;
}
void CScheduleStockEvent::SetDuration(const std::chrono::seconds& newDuration)
{
	duration = newDuration;
}
int CScheduleStockEvent::GetUsage() const
{
	return usage;
}
void CScheduleStockEvent::IncrementUsage()
{
	++usage;
}
void CScheduleStockEvent::DecrementUsage()
{
	--usage;
}
void CScheduleStockEvent::SetUsage(int usage)
{
	this->usage = usage;
}

const std::vector<CScheduleEventConstraintPtr>& CScheduleStockEvent::GetConstraints() const
{
	return constraints;
}
void CScheduleStockEvent::AddConstraint(CScheduleEventConstraintPtr constraint)
{
	constraints.push_back(std::move(constraint));
}
void CScheduleStockEvent::RemoveConstraint(CScheduleEventConstraint* constraint)
{
	constraints.erase(std::remove_if(constraints.begin(), constraints.end(), [constraint](const auto& c) {
		return c.get() == constraint;
	}), constraints.end());
}
bool CScheduleStockEvent::CanSchedule(CScheduleTrack* track) const
{
	for (const auto& c : constraints)
	{
		if (!c->CanSchedule(this, track)) return false;
	}
	return true;
}
void CScheduleStockEvent::RefreshGlobalSchedulingCapabilities(const std::vector<CScheduleTrackPtr>& tracks)
{
	canScheduleGlobally = true;
	for (const auto& c : constraints)
	{
		for (const auto& track : tracks)
		{
			if (c->IsGlobal() && !c->CanSchedule(this, track.get()))
			{
				canScheduleGlobally = false;
				break;
			}
		}
	}	
}
bool CScheduleStockEvent::CanScheduleGlobally() const
{
	return canScheduleGlobally;
}