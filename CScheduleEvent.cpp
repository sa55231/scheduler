#include "pch.h"
#include "CScheduleEvent.h"

CScheduleEvent::CScheduleEvent(CScheduleStockEvent* eventSource):
	stockEvent(eventSource)
{
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