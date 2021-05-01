#include "pch.h"
#include "CScheduleTrack.h"

CScheduleTrack::CScheduleTrack():totalDuration(0), maxEventDuration(0)
{}
CScheduleTrack::CScheduleTrack(CString name, std::vector<CScheduleEvent> events):
	name(std::move(name)),events(std::move(events)), totalDuration(0), maxEventDuration(0)
{
}
CString CScheduleTrack::GetName() const
{
	return name;
}
const std::vector<CScheduleEvent>& CScheduleTrack::GetEvents() const
{
	return events;
}
void CScheduleTrack::AddEvent(CScheduleEvent event)
{
	events.push_back(std::move(event));
	totalDuration += events.back().GetDuration();
	if (maxEventDuration.count() < events.back().GetDuration().count())
	{
		maxEventDuration = events.back().GetDuration();
	}
}
std::chrono::seconds CScheduleTrack::GetTotalDuration() const
{
	return totalDuration;
}
std::chrono::seconds CScheduleTrack::GetMaxEventDuration() const
{
	return maxEventDuration;
}
