#include "pch.h"
#include "TrackRenderer.h"

CTrackRenderer::CTrackRenderer(const CScheduleTrack& track,
	CD2DTextFormat* textFormat, CD2DSolidColorBrush* backgroundColorBrush, CD2DSolidColorBrush* foregroundColorBrush):
	name(track.GetName()),
	textFormat(textFormat),
	backgroundColorBrush(backgroundColorBrush),
	foregroundColorBrush(foregroundColorBrush),
	trackBounds(D2D1::RectF()),
	preferredSize(D2D1::SizeF()),
	trackLabelSize(D2D1::SizeF())
{
}

void CTrackRenderer::SetEventRenderers(std::vector<std::unique_ptr<CEventRenderer>> eventRenderers)
{
	this->eventRenderers = std::move(eventRenderers);
}

bool CTrackRenderer::ContainsPoint(const D2D1_POINT_2F& point) const
{
	return (point.y >= trackBounds.top && point.y <= trackBounds.bottom);
}

CEventRenderer* CTrackRenderer::GetEventAtPoint(const D2D1_POINT_2F& point) const
{
	if (point.x >= trackBounds.left && point.x <= trackBounds.right && point.y >= trackBounds.top && point.y <= trackBounds.bottom)
	{
		for (const auto& event : eventRenderers)
		{
			if (event->ContainsPoint(point)) return event.get();
		}
	}
	return nullptr;
}
void CTrackRenderer::SetTrackBounds(D2D1_RECT_F& rect)
{
	trackBounds = rect;
}
void CTrackRenderer::SetPreferredSize(D2D1_SIZE_F& size)
{
	preferredSize = size;
}
D2D1_SIZE_F CTrackRenderer::GetPreferredSize() const
{
	return preferredSize;
}
void CTrackRenderer::Render(CHwndRenderTarget* renderTarget)
{	
	renderTarget->FillRectangle(trackBounds, backgroundColorBrush);
	renderTarget->DrawText(name,trackBounds,foregroundColorBrush,textFormat);

	for (auto& event : eventRenderers)
	{
		event->Render(renderTarget);
	}
	

	if (selected)
	{
		renderTarget->DrawRectangle(trackBounds, foregroundColorBrush);
	}
}


bool CTrackRenderer::IsSelected() const
{
	return selected;
}
void CTrackRenderer::SetSelected(bool flag)
{
	selected = flag;
}
