#include "pch.h"
#include "CScheduleTrack.h"

CScheduleTrack::CScheduleTrack():totalDuration(0), maxEventDuration(0)
{}
CScheduleTrack::CScheduleTrack(int id, CString name, std::vector<CScheduleEventPtr> events):
	id(id),name(std::move(name)),events(std::move(events)), totalDuration(0), maxEventDuration(0)
{
}
CString CScheduleTrack::GetName() const
{
	return name;
}
int CScheduleTrack::GetId() const
{
	return id;
}
const std::vector<CScheduleEventPtr>& CScheduleTrack::GetEvents() const
{
	return events;
}
void CScheduleTrack::AddEvent(CScheduleEventPtr event)
{
	events.push_back(std::move(event));
	totalDuration += events.back()->GetDuration();
	if (maxEventDuration.count() < events.back()->GetDuration().count())
	{
		maxEventDuration = events.back()->GetDuration();
	}
}

void CScheduleTrack::InsertEventAtIndex(int index, CScheduleEventPtr event)
{
	if (index < 0 || index >= events.size()) return;
	totalDuration += event->GetDuration();
	if (maxEventDuration.count() < event->GetDuration().count())
	{
		maxEventDuration = event->GetDuration();
	}
	events.insert(events.begin()+index,std::move(event));
}
std::chrono::seconds CScheduleTrack::GetTotalDuration() const
{
	return totalDuration;
}
std::chrono::seconds CScheduleTrack::GetMaxEventDuration() const
{
	return maxEventDuration;
}
void CScheduleTrack::RemoveEvents(int id)
{
	events.erase(std::remove_if(events.begin(), events.end(), [id](const auto& ev) {
		return ev->GetStockId() == id;
	}),
	events.end());
}