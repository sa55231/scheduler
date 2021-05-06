#pragma once

#include "TrackRenderer.h"
#include "schedulerDoc.h"

#include <vector>
#include <unordered_map>

class CSchedulerDocumentRenderer
{
public:
	CSchedulerDocumentRenderer();
	void Render(CHwndRenderTarget* renderTarget, const D2D1_POINT_2F& scrollPosition);
	void Resize(int cx, int cy);
	D2D1_SIZE_F UpdateLayout(CSchedulerDoc* doc, CHwndRenderTarget* renderTarget, IDWriteFactory* directWriteFactory);
	CEventRenderer* GetEventAtPoint(const D2D1_POINT_2F& point);
	CTrackRenderer* GetTrackAtPoint(const D2D1_POINT_2F& point);
private:
	void CreateD2D1Resources(CHwndRenderTarget* renderTarget, IDWriteFactory* directWriteFactory);
private:
	D2D1_SIZE_F zoomLevel;
	std::vector<std::unique_ptr<CTrackRenderer>> tracks;
	FLOAT margin = 10.f;
	FLOAT trackHeight = 16.f;
	FLOAT minEventRenderWidth = 16.f;
	CD2DTextFormat* trackTextFormat = nullptr;
	CD2DSolidColorBrush* trackBackgroundColorBrush = nullptr;
	CD2DSolidColorBrush* trackForegroundColorBrush = nullptr;
	CD2DTextFormat* eventTextFormat = nullptr;
	CD2DSolidColorBrush* eventForegroundColorBrush = nullptr;
	CComPtr<IDWriteInlineObject> trimmingSign;
	std::unordered_map<UINT32,CD2DSolidColorBrush*> eventBackgroundColorBrushes;
};

