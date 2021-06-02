#pragma once

#include "CScheduleEvent.h"

#include <memory>

class CEventRenderer
{
public:
	CEventRenderer(CScheduleEvent* event, CD2DTextFormat* textFormat, 
		CD2DSolidColorBrush* backgroundColorBrush, CD2DSolidColorBrush* foregroundColorBrush,
		ID2D1StrokeStyle* dropTargetStrokeStyle);
	void Render(CRenderTarget* renderTarget);
	void Render(CRenderTarget* renderTarget, CD2DSolidColorBrush* backgroundColorBrush, CD2DSolidColorBrush* foregroundColorBrush, CD2DTextFormat* textFormat);
	D2D1_SIZE_F GetSize() const;
	bool IsSelected() const;
	void SetSelected(bool flag);
	bool ContainsPoint(const D2D1_POINT_2F& point) const;
	void SetEventBounds(const D2D1_RECT_F& rect);
	D2D1_RECT_F GetEventBounds() const;
	void SetMinimumTextRenderingWidth(FLOAT width);
	int GetEventId() const;
	bool IsDropTarget() const;
	void SetDropTarget(bool flag);
	CScheduleEvent* GetEvent() const;
private:
	CScheduleEvent* event = nullptr;	
	CString name;
	int eventId = -1;
	CD2DTextFormat* textFormat = nullptr;
	CD2DSolidColorBrush* backgroundColorBrush = nullptr;
	CD2DSolidColorBrush* foregroundColorBrush = nullptr;
	ID2D1StrokeStyle* dropTargetStrokeStyle = nullptr;
	bool selected = false;
	D2D1_RECT_F eventBounds;
	D2D1_ROUNDED_RECT eventBoundsRoundedRect;
	FLOAT minEventRenderWidth = 16.f;
	bool dropTarget = false;
};

