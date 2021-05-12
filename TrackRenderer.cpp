#include "pch.h"
#include "TrackRenderer.h"

CTrackRenderer::CTrackRenderer(CScheduleTrack* track,
	CD2DTextFormat* textFormat, CD2DSolidColorBrush* backgroundColorBrush, 
	CD2DSolidColorBrush* foregroundColorBrush, ID2D1StrokeStyle* dropTargetStrokeStyle):
	track(track),
	textFormat(textFormat),
	backgroundColorBrush(backgroundColorBrush),
	foregroundColorBrush(foregroundColorBrush),
	dropTargetStrokeStyle(dropTargetStrokeStyle),
	trackBounds(D2D1::RectF()),
	trackLabelBounds(D2D1::RectF())
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
int CTrackRenderer::GetEventRenderIndex(CEventRenderer* eventRenderer) const
{
	int index = 0;
	for (const auto& event : eventRenderers)
	{
		if (event.get() == eventRenderer) return index;
		++index;
	}
	return -1;
}
CScheduleTrack* CTrackRenderer::GetTrack() const
{
	return track;
}
int CTrackRenderer::GetTrackId() const
{
	return track->GetId();
}
CString CTrackRenderer::GetName() const
{
	return track->GetName();
}
void CTrackRenderer::SetTrackBounds(D2D1_RECT_F& rect)
{
	trackBounds = rect;
}

void CTrackRenderer::SetTrackLabelBounds(D2D1_RECT_F& rect)
{
	trackLabelBounds = rect;
}
D2D1_RECT_F CTrackRenderer::GetTrackLabelBounds() const
{
	return trackLabelBounds;
}

void CTrackRenderer::Render(CHwndRenderTarget* renderTarget)
{	
	if (track == nullptr) return;
	if (!renderTarget->IsValid()) return;
	if (!backgroundColorBrush->IsValid()) return;
	if (!foregroundColorBrush->IsValid()) return;
	if (!textFormat->IsValid()) return;

	renderTarget->FillRectangle(trackLabelBounds, backgroundColorBrush);
	renderTarget->DrawText(track->GetName(), trackLabelBounds,foregroundColorBrush,textFormat);

	for (auto& event : eventRenderers)
	{
		event->Render(renderTarget);
	}
	

	if (selected && !dropTarget)
	{
		renderTarget->DrawRectangle(trackBounds, foregroundColorBrush);
	}

	if (dropTarget)
	{
		renderTarget->DrawRectangle(trackBounds, foregroundColorBrush,2.f,dropTargetStrokeStyle);
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
bool CTrackRenderer::IsDropTarget() const
{
	return dropTarget;
}
void CTrackRenderer::SetDropTarget(bool flag)
{
	dropTarget = flag;
}
