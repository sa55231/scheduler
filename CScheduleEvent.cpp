#include "pch.h"
#include "CScheduleEvent.h"

CScheduleEvent::CScheduleEvent(CScheduleStockEvent* eventSource,int id, int trackId):
	stockEvent(eventSource),id(id),trackId(trackId)
{
}
int CScheduleEvent::GetTrackId() const
{
	return trackId;
}
int CScheduleEvent::GetId() const
{
	return id;
}
int CScheduleEvent::GetStockId() const
{
	return stockEvent->GetId();
}
CString CScheduleEvent::GetName() const
{
	return stockEvent->GetName();
}
std::chrono::seconds CScheduleEvent::GetDuration() const
{
	return stockEvent->GetDuration();
}
UINT32 CScheduleEvent::GetColor() const
{
	return stockEvent->GetColor();
}