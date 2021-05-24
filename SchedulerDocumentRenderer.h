#pragma once

#include "TrackRenderer.h"
#include "schedulerDoc.h"

#include <vector>
#include <unordered_map>
#include <chrono>

class CSchedulerDocumentRenderer
{
public:
	CSchedulerDocumentRenderer();
	void Render(CRenderTarget* renderTarget, const D2D1_POINT_2F& scrollPosition);
	void Resize(int cx, int cy);
	D2D1_SIZE_F UpdateLayout(CSchedulerDoc* doc, CRenderTarget* renderTarget, IDWriteFactory* directWriteFactory,
		ID2D1Factory* factory);
	D2D1_SIZE_F GetZoomLevel() const;
	//CEventRenderer* GetEventAtPoint(const D2D1_POINT_2F& point);
	CTrackRenderer* GetTrackAtPoint(const D2D1_POINT_2F& point);
	D2D1_COLOR_F GetEventForegroundColor() const;
private:	
	void CreateD2D1Resources(CSchedulerDoc* doc, CRenderTarget* renderTarget, IDWriteFactory* directWriteFactory ,ID2D1Factory* factory);
	template <typename T> using Line = std::pair<T, T>;
	template<typename T>
	constexpr Line<T> makeLine(T t1, T t2) noexcept
	{
		return std::make_pair<T, T>(std::move(t1),std::move(t2));
	}
private:
	D2D1_SIZE_F zoomLevel;
	std::vector<std::unique_ptr<CTrackRenderer>> tracks;
	FLOAT margin = 10.f;
	FLOAT trackHeight = 16.f;
	FLOAT minEventRenderWidth = 16.f;
	CD2DSolidColorBrush* gridLinesColorBrush = nullptr;
	CD2DTextFormat* trackTextFormat = nullptr;
	CD2DSolidColorBrush* trackBackgroundColorBrush = nullptr;
	CD2DSolidColorBrush* trackForegroundColorBrush = nullptr;
	CD2DTextFormat* eventTextFormat = nullptr;
	CD2DSolidColorBrush* eventForegroundColorBrush = nullptr;
	CD2DTextFormat* headerTimelineTextFormat = nullptr;
	CD2DSolidColorBrush* headerTimelineForegroundColorBrush = nullptr;
	CComPtr<IDWriteInlineObject> eventTrimmingSign;
	CComPtr<IDWriteInlineObject> trackTrimmingSign;
	CComPtr<ID2D1StrokeStyle> dropTargetStrokeStyle;
	std::unordered_map<UINT32,CD2DSolidColorBrush*> eventBackgroundColorBrushes;

	D2D1_SIZE_F surfaceSize = {D2D1::SizeF()};
	bool resourcesCreated = false;
	D2D1_RECT_F headerRect = { D2D1::RectF() };
	D2D1_POINT_2F startCorner = { D2D1::Point2F() };
	D2D1_POINT_2F headerSeparationLine[2] = { D2D1::Point2F(), D2D1::Point2F() };
	D2D1_RECT_F surfaceRect = { D2D1::RectF() };	
	std::vector<Line<D2D1_POINT_2F>> trackSeparationLines;
	struct HeaderTimelineItem
	{
		CString text;
		D2D1_RECT_F textBounds;
		Line<D2D1_POINT_2F> line;
	};
	std::vector<HeaderTimelineItem> headerTimelineItems;
};

