#include "pch.h"
#include "EventRenderer.h"

CEventRenderer::CEventRenderer(CScheduleEvent* event, CD2DTextFormat* textFormat,
	CD2DSolidColorBrush* backgroundColorBrush, CD2DSolidColorBrush* foregroundColorBrush, 
	ID2D1StrokeStyle* dropTargetStrokeStyle) :
	textFormat(textFormat),
	backgroundColorBrush(backgroundColorBrush),
	foregroundColorBrush(foregroundColorBrush),
	dropTargetStrokeStyle(dropTargetStrokeStyle),
	name(event->GetName()),
	color(event->GetColor()),
	eventBounds(D2D1::RectF()),
	eventBoundsRoundedRect(D2D1::RoundedRect(eventBounds,0.f,0.f)),
	eventId(event->GetStockId())
{
	eventWidth = (float)std::chrono::duration_cast<std::chrono::minutes>(event->GetDuration()).count();
}
bool CEventRenderer::ContainsPoint(const D2D1_POINT_2F& point) const
{
	return (point.x >= eventBounds.left && point.x <= eventBounds.right && point.y >= eventBounds.top && point.y <= eventBounds.bottom);
}
float CEventRenderer::GetWidth() const
{
	return eventWidth;
}
void CEventRenderer::Render(CRenderTarget* renderTarget)
{	
	renderTarget->FillRoundedRectangle(eventBoundsRoundedRect, backgroundColorBrush);
	
	if (selected && !dropTarget)
	{
		renderTarget->DrawRoundedRectangle(eventBoundsRoundedRect, foregroundColorBrush);
	}

	if (dropTarget)
	{
		renderTarget->DrawRoundedRectangle(eventBoundsRoundedRect, foregroundColorBrush, 2.f, dropTargetStrokeStyle);
	}

	if (eventBounds.right - eventBounds.left > minEventRenderWidth)
	{
		renderTarget->DrawText(name,eventBounds, foregroundColorBrush, textFormat);
	}
}
void CEventRenderer::SetMinimumTextRenderingWidth(FLOAT width)
{
	minEventRenderWidth = width;
}
void CEventRenderer::SetEventBounds(D2D1_RECT_F& rect)
{
	eventBounds = rect;
	auto height = eventBounds.top - eventBounds.bottom;
	eventBoundsRoundedRect = D2D1::RoundedRect(eventBounds, height * 0.3f, height * 0.3f);
}
bool CEventRenderer::IsSelected() const
{
	return selected;
}
void CEventRenderer::SetSelected(bool flag)
{
	selected = flag;
}
int CEventRenderer::GetEventId() const
{
	return eventId;
}
bool CEventRenderer::IsDropTarget() const
{
	return dropTarget;
}
void CEventRenderer::SetDropTarget(bool flag)
{
	dropTarget = flag;
}
