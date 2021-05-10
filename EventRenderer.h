#pragma once

#include "CScheduleEvent.h"

#include <memory>

class CEventRenderer
{
public:
	CEventRenderer(CScheduleEvent* event, CD2DTextFormat* textFormat, 
		CD2DSolidColorBrush* backgroundColorBrush, CD2DSolidColorBrush* foregroundColorBrush);
	void Render(CRenderTarget* renderTarget);
	float GetWidth() const;
	bool IsSelected() const;
	void SetSelected(bool flag);
	bool ContainsPoint(const D2D1_POINT_2F& point) const;
	void SetEventBounds(D2D1_RECT_F& rect);
	void SetMinimumTextRenderingWidth(FLOAT width);
	int GetEventId() const;
private:
	CD2DTextFormat* textFormat = nullptr;
	float eventWidth = 0;
	CD2DSolidColorBrush* backgroundColorBrush = nullptr;
	CD2DSolidColorBrush* foregroundColorBrush = nullptr;;
	CString name;
	UINT32 color = 0;
	bool selected = false;
	D2D1_RECT_F eventBounds;
	D2D1_ROUNDED_RECT eventBoundsRoundedRect;
	FLOAT minEventRenderWidth = 16.f;
	int eventId = -1;
};

