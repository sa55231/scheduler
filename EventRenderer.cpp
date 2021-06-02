#include "pch.h"
#include "EventRenderer.h"

CEventRenderer::CEventRenderer(CScheduleEvent* event, CD2DTextFormat* textFormat,
	CD2DSolidColorBrush* backgroundColorBrush, CD2DSolidColorBrush* foregroundColorBrush, 
	ID2D1StrokeStyle* dropTargetStrokeStyle) :
	event(event),
	name(event->GetName()),
	eventId(event->GetId()),
	textFormat(textFormat),
	backgroundColorBrush(backgroundColorBrush),
	foregroundColorBrush(foregroundColorBrush),
	dropTargetStrokeStyle(dropTargetStrokeStyle),
	eventBounds(D2D1::RectF()),
	eventBoundsRoundedRect(D2D1::RoundedRect(eventBounds,0.f,0.f))
{
	
}

bool CEventRenderer::ContainsPoint(const D2D1_POINT_2F& point) const
{
	return (point.x >= eventBounds.left && point.x <= eventBounds.right && point.y >= eventBounds.top && point.y <= eventBounds.bottom);
}
D2D1_SIZE_F CEventRenderer::GetSize() const
{
	return D2D1::SizeF(eventBounds.right-eventBounds.left,eventBounds.bottom-eventBounds.top);
}
D2D1_RECT_F CEventRenderer::GetEventBounds() const
{
	return eventBounds;
}
void CEventRenderer::Render(CRenderTarget* renderTarget, CD2DSolidColorBrush* backgroundColorBrush, 
	CD2DSolidColorBrush* foregroundColorBrush, CD2DTextFormat* textFormat)
{
	 D2D1_RECT_F bounds = D2D1::RectF(0,0,eventBounds.right-eventBounds.left, eventBounds.bottom - eventBounds.top);
	 auto height = bounds.top - bounds.bottom;
	 D2D1_ROUNDED_RECT boundsRoundedRect = D2D1::RoundedRect(bounds, height * 0.3f, height * 0.3f);
	 renderTarget->FillRoundedRectangle(boundsRoundedRect, backgroundColorBrush);
	 renderTarget->DrawRoundedRectangle(boundsRoundedRect, foregroundColorBrush);
	 renderTarget->DrawText(name, bounds, foregroundColorBrush, textFormat == nullptr ? this->textFormat : textFormat);
}
void CEventRenderer::Render(CRenderTarget* renderTarget)
{	
	if (event == nullptr) return;
	if (!renderTarget->IsValid()) return;
	if (!backgroundColorBrush->IsValid()) return;
	if (!foregroundColorBrush->IsValid()) return;
	if (!textFormat->IsValid()) return;
	//if (!dropTargetStrokeStyle->IsValid()) return;

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
void CEventRenderer::SetEventBounds(const D2D1_RECT_F& rect)
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
CScheduleEvent* CEventRenderer::GetEvent() const
{
	return event;
}
bool CEventRenderer::IsDropTarget() const
{
	return dropTarget;
}
void CEventRenderer::SetDropTarget(bool flag)
{
	dropTarget = flag;
}
