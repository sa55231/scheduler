#pragma once

#include "CScheduleTrack.h"
#include "EventRenderer.h"

#include <vector>
#include <unordered_map>

class CTrackRenderer
{
public:
	CTrackRenderer(CScheduleTrack* track, 
		CD2DTextFormat* textFormat, CD2DSolidColorBrush* backgroundColorBrush, 
		CD2DSolidColorBrush* foregroundColorBrush, ID2D1StrokeStyle* dropTargetStrokeStyle);
	void Render(CHwndRenderTarget* renderTarget);
	void SetTrackLabelBounds(D2D1_RECT_F& rect);
	D2D1_RECT_F GetTrackLabelBounds() const;
	void SetTrackBounds(D2D1_RECT_F& rect);
	CEventRenderer* GetEventAtPoint(const D2D1_POINT_2F& point) const;
	bool ContainsPoint(const D2D1_POINT_2F& point) const;
	bool IsSelected() const;
	void SetSelected(bool flag);
	void SetEventRenderers(std::vector<std::unique_ptr<CEventRenderer>> eventRenderers);
	int GetEventRenderIndex(CEventRenderer* eventRenderer) const;
	CString GetName() const;
	int GetTrackId() const;
	bool IsDropTarget() const;
	void SetDropTarget(bool flag);
	CScheduleTrack* GetTrack() const;
private:
	int trackId = 0;
	CString name;
	CScheduleTrack* track = nullptr;
	CD2DTextFormat* textFormat = nullptr;
	CD2DSolidColorBrush* backgroundColorBrush = nullptr;
	CD2DSolidColorBrush* foregroundColorBrush = nullptr;
	ID2D1StrokeStyle* dropTargetStrokeStyle = nullptr;
	D2D1_RECT_F trackLabelBounds;
	FLOAT margin = 0.f;
	FLOAT trackMargin = 10.f;
	std::vector<std::unique_ptr<CEventRenderer>> eventRenderers;
	D2D1_RECT_F trackBounds;
	bool selected = false;
	bool dropTarget = false;
};

