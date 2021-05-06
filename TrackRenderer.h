#pragma once

#include "CScheduleTrack.h"
#include "EventRenderer.h"

#include <vector>
#include <unordered_map>

class CTrackRenderer
{
public:
	CTrackRenderer(const CScheduleTrack& track, 
		CD2DTextFormat* textFormat, CD2DSolidColorBrush* backgroundColorBrush, CD2DSolidColorBrush* foregroundColorBrush);
	void Render(CHwndRenderTarget* renderTarget);
	D2D1_SIZE_F GetPreferredSize() const;
	void SetPreferredSize(D2D1_SIZE_F& size);
	void SetTrackBounds(D2D1_RECT_F& rect);
	CEventRenderer* GetEventAtPoint(const D2D1_POINT_2F& point) const;
	bool ContainsPoint(const D2D1_POINT_2F& point) const;
	bool IsSelected() const;
	void SetSelected(bool flag);
	void SetEventRenderers(std::vector<std::unique_ptr<CEventRenderer>> eventRenderers);
private:
	CString name;
	CD2DTextFormat* textFormat = nullptr;
	D2D1_SIZE_F trackLabelSize;
	CD2DSolidColorBrush* backgroundColorBrush = nullptr;
	CD2DSolidColorBrush* foregroundColorBrush = nullptr;
	D2D1_SIZE_F preferredSize;
	FLOAT margin = 0.f;
	FLOAT trackMargin = 10.f;
	std::vector<std::unique_ptr<CEventRenderer>> eventRenderers;
	D2D1_RECT_F trackBounds;
	bool selected = false;
};

