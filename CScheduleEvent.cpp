#include "pch.h"
#include "CScheduleEvent.h"

CScheduleEvent::CScheduleEvent(const CScheduleStockEvent& eventSource):
	stockId(eventSource.GetId()),name(eventSource.GetName()),duration(eventSource.GetDuration()),
	color(eventSource.GetColor())
{
}

int CScheduleEvent::GetStockId() const
{
	return stockId;
}
CString CScheduleEvent::GetName() const
{
	return name;
}
std::chrono::seconds CScheduleEvent::GetDuration() const
{
	return duration;
}
UINT32 CScheduleEvent::GetColor() const
{
	return color;
}