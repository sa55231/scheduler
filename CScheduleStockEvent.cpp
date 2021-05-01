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