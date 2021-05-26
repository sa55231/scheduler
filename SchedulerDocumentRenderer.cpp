#include "pch.h"
#include "SchedulerDocumentRenderer.h"
#include <algorithm>
#include <sstream>
#include <type_traits>

namespace
{
	constexpr float TRACK_LABEL_WIDTH = 50.f;
	constexpr float HEADER_HEIGHT = 50.f;
	constexpr float DURATION_COLUMN_WIDTH = 100.f;
}

CSchedulerDocumentRenderer::CSchedulerDocumentRenderer() : zoomLevel(D2D1::SizeF(1.0f, 1.0f))
{
	
}

D2D1_SIZE_F CSchedulerDocumentRenderer::UpdateLayout(CSchedulerDoc* doc, CRenderTarget* renderTarget,
	IDWriteFactory* directWriteFactory, ID2D1Factory* factory)
{
	tracks.clear();
	trackSeparationLines.clear();
	headerTimelineItems.clear();
	CreateD2D1Resources(doc,renderTarget, directWriteFactory,factory);
	zoomLevel = D2D1::SizeF(doc->GetZoomLevel(),doc->GetZoomLevel());
	surfaceSize = D2D1::SizeF(margin,margin+HEADER_HEIGHT);
	FLOAT maxWidth = 0.f;
	std::chrono::seconds maxDuration = std::chrono::seconds::min();
	trackSeparationLines.emplace_back(D2D1::Point2F(0.f, surfaceSize.height - margin / 2.f), D2D1::Point2F(maxWidth, surfaceSize.height - margin / 2.f));
	auto earliestStartTime = doc->GetStartTime();
	for (const auto& track : doc->GetTracks())
	{
		auto trackStartTime = track->GetStartTime();
		auto startTimeDiffSeconds = std::chrono::duration_cast<std::chrono::seconds>(trackStartTime - earliestStartTime);
		auto startEventsWidth = (float)startTimeDiffSeconds.count() / (float)doc->GetTimeScale();
		auto trackRenderer = std::make_unique<CTrackRenderer>(track.get(), trackTextFormat, trackBackgroundColorBrush,
			trackForegroundColorBrush, dropTargetStrokeStyle.p);

		float trackWidth = TRACK_LABEL_WIDTH + margin;
		trackRenderer->SetTrackLabelBounds(D2D1::RectF(0.f, surfaceSize.height,trackWidth, surfaceSize.height+trackHeight));
		std::vector<std::unique_ptr<CEventRenderer>> eventRenderers;
		trackWidth += startEventsWidth;
		auto xOffset = trackWidth;
		std::chrono::seconds trackDuration = std::chrono::seconds::zero();
		for (const auto& event : track->GetEvents())
		{
			auto color = event->GetColor();
			if (eventBackgroundColorBrushes.find(color) == eventBackgroundColorBrushes.end())
			{
				eventBackgroundColorBrushes[color] = new CD2DSolidColorBrush(renderTarget, D2D1::ColorF(color,1.f));
			}
			auto eventRenderer = std::make_unique<CEventRenderer>(event.get(), eventTextFormat, 
				eventBackgroundColorBrushes[color], eventForegroundColorBrush, dropTargetStrokeStyle.p);
			eventRenderer->SetMinimumTextRenderingWidth(minEventRenderWidth);
			auto eventDurationInSeconds = event->GetDuration().count();
			auto eventWidth = (float)eventDurationInSeconds/(float)doc->GetTimeScale() + margin;
			eventRenderer->SetEventBounds(D2D1::RectF(xOffset, surfaceSize.height,xOffset+eventWidth, surfaceSize.height+trackHeight));
			trackWidth += eventWidth;
			xOffset += eventWidth;
			eventRenderers.push_back(std::move(eventRenderer));
			trackDuration += event->GetDuration();
		}
		trackRenderer->SetTrackBounds(D2D1::RectF(0.f, surfaceSize.height,xOffset,trackHeight + surfaceSize.height));
		trackRenderer->SetEventRenderers(std::move(eventRenderers));

		if (maxWidth < trackWidth)
		{
			maxWidth = trackWidth;
		}
		if (maxDuration < trackDuration)
		{
			maxDuration = trackDuration;
		}
		surfaceSize.height += trackHeight + margin;
		tracks.push_back(std::move(trackRenderer));
		trackSeparationLines.emplace_back(D2D1::Point2F(0.f, surfaceSize.height - margin / 2.f), D2D1::Point2F(maxWidth, surfaceSize.height - margin / 2.f));
	}
	surfaceSize.width += maxWidth + margin;
	//update the grid lines' second point to the max width
	std::for_each(trackSeparationLines.begin(), trackSeparationLines.end(), 
	[this](auto& el) {
		el.second.x = this->surfaceSize.width;
	});
	headerRect = D2D1::RectF(0.f,0.f,surfaceSize.width, HEADER_HEIGHT) ;
	headerSeparationLine[0].x = headerSeparationLine[1].x = TRACK_LABEL_WIDTH + margin;
	headerSeparationLine[1].y = surfaceSize.height;
	surfaceRect.bottom = surfaceSize.height;
	surfaceRect.right = surfaceSize.width;
		
	auto startTime = doc->GetStartTime();
	std::chrono::minutes utcOffset(doc->GetUTCOffsetMinutes());
	date::local_seconds start{ std::chrono::duration_cast<std::chrono::seconds>(startTime.time_since_epoch()) - utcOffset };

	const auto columnsCount = (surfaceRect.right - TRACK_LABEL_WIDTH) / DURATION_COLUMN_WIDTH;
	if ((int)columnsCount > 0)
	{
		auto secondsInColumnCount = maxDuration.count() / static_cast<std::chrono::seconds::rep>(columnsCount);
		std::chrono::seconds secondsInColumn(secondsInColumnCount);
		for (float i = TRACK_LABEL_WIDTH + margin; i < surfaceRect.right; i += DURATION_COLUMN_WIDTH)
		{
			HeaderTimelineItem item;
			item.textBounds = D2D1::RectF(i - 50.f, 0.f, i + 50.f, HEADER_HEIGHT);
			CString text(date::format("%Y %b %d\n%R", start).c_str());
			start += secondsInColumn;

			item.text = text;
			item.line = makeLine<D2D1_POINT_2F>(D2D1::Point2F(i, HEADER_HEIGHT), D2D1::Point2F(i, surfaceRect.bottom));
			headerTimelineItems.push_back(std::move(item));
		}
	}
	TRACE("Zoom level: %f\n", doc->GetZoomLevel());
	TRACE("Timescale: %f\n", doc->GetTimeScale());
	TRACE("Surface size : %fx%f\n", surfaceSize.width , surfaceSize.height);
	TRACE("Zoomed surface size : %fx%f\n", surfaceSize.width * zoomLevel.width, surfaceSize.height * zoomLevel.height);
	return D2D1::SizeF(surfaceSize.width * zoomLevel.width, surfaceSize.height * zoomLevel.height);
}

D2D1_COLOR_F CSchedulerDocumentRenderer::GetEventForegroundColor() const
{
	if (eventForegroundColorBrush != nullptr)
	{
		return eventForegroundColorBrush->GetColor();
	}
	return D2D1::ColorF(0.f,0.f,0.f);
}

void CSchedulerDocumentRenderer::CreateD2D1Resources(CSchedulerDoc* doc, CRenderTarget* renderTarget, IDWriteFactory* directWriteFactory, ID2D1Factory* factory)
{
	if (trackTextFormat)
	{
		delete trackTextFormat;
		trackTextFormat = nullptr;
		trackTrimmingSign.Release();
	}
	
	{
		auto font = doc->GetTracksFont();
		FLOAT fontSize = (FLOAT) font.lfHeight;
		if (fontSize < 0)
		{
			fontSize = std::abs(font.lfHeight * 96.f) / std::min(renderTarget->GetDpi().width, renderTarget->GetDpi().height);
		}		
		trackTextFormat = new CD2DTextFormat(renderTarget, font.lfFaceName, fontSize,(DWRITE_FONT_WEIGHT)font.lfWeight,font.lfItalic == 1 ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL);
		trackTextFormat->Get()->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		trackTextFormat->Get()->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		trackTextFormat->Get()->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
		directWriteFactory->CreateEllipsisTrimmingSign(trackTextFormat->Get(), &trackTrimmingSign);
		DWRITE_TRIMMING trimming = { DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0 };
		trackTextFormat->Get()->SetTrimming(&trimming, trackTrimmingSign);
	}
	if (!trackBackgroundColorBrush)
	{
		trackBackgroundColorBrush = new CD2DSolidColorBrush(renderTarget, D2D1::ColorF(D2D1::ColorF::LightGray, 0.5f));
	}
	if (!trackForegroundColorBrush)
	{
		trackForegroundColorBrush = new CD2DSolidColorBrush(renderTarget, D2D1::ColorF(D2D1::ColorF::Black));
	}
	if (!gridLinesColorBrush)
	{
		gridLinesColorBrush = new CD2DSolidColorBrush(renderTarget, D2D1::ColorF(D2D1::ColorF::Black, 0.5f));
	}
	if (eventTextFormat)
	{
		delete eventTextFormat;
		eventTextFormat = nullptr;
		eventTrimmingSign.Release();
	}	
	{
		auto font = doc->GetEventsFont();
		FLOAT fontSize = (FLOAT)font.lfHeight;
		if (fontSize < 0)
		{
			fontSize = std::abs(font.lfHeight * 96.f) / std::min(renderTarget->GetDpi().width, renderTarget->GetDpi().height);
		}
		eventTextFormat = new CD2DTextFormat(renderTarget, font.lfFaceName, fontSize, (DWRITE_FONT_WEIGHT)font.lfWeight, font.lfItalic == 1 ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL);
		eventTextFormat->Get()->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		eventTextFormat->Get()->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		eventTextFormat->Get()->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

		directWriteFactory->CreateEllipsisTrimmingSign(eventTextFormat->Get(), &eventTrimmingSign);
		DWRITE_TRIMMING trimming = { DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0 };
		eventTextFormat->Get()->SetTrimming(&trimming, eventTrimmingSign);
	}
	if (!eventForegroundColorBrush)
	{
		eventForegroundColorBrush = new CD2DSolidColorBrush(renderTarget, D2D1::ColorF(D2D1::ColorF::Black));
	}
	if (headerTimelineTextFormat)
	{
		delete headerTimelineTextFormat;
		headerTimelineTextFormat = nullptr;	
	}
	{
		auto font = doc->GetHeadersFont();
		FLOAT fontSize = (FLOAT)font.lfHeight;
		if (fontSize < 0)
		{
			fontSize = std::abs(font.lfHeight * 96.f) / std::min(renderTarget->GetDpi().width, renderTarget->GetDpi().height);
		}
		headerTimelineTextFormat = new CD2DTextFormat(renderTarget, font.lfFaceName, fontSize, (DWRITE_FONT_WEIGHT)font.lfWeight, font.lfItalic == 1 ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL);
		headerTimelineTextFormat->Get()->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		headerTimelineTextFormat->Get()->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		headerTimelineTextFormat->Get()->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
	}
	if (!headerTimelineForegroundColorBrush)
	{
		headerTimelineForegroundColorBrush = new CD2DSolidColorBrush(renderTarget, D2D1::ColorF(D2D1::ColorF::Black, 0.8f));
	}
	if (!dropTargetStrokeStyle)
	{
		float dashes[] = { 2.0f, 2.0f, 0.f, 2.f};
		HRESULT hr = factory->CreateStrokeStyle(
			D2D1::StrokeStyleProperties(
				D2D1_CAP_STYLE_ROUND,
				D2D1_CAP_STYLE_ROUND,
				D2D1_CAP_STYLE_ROUND,
				D2D1_LINE_JOIN_MITER,
				10.0f,
				D2D1_DASH_STYLE_DASH,
				0.0f),
			nullptr,
			0,
			&dropTargetStrokeStyle);
		ASSERT(SUCCEEDED(hr));
	}
	{
		DWRITE_TEXT_METRICS minMetrics;
		CD2DTextLayout tmpTextLayout(renderTarget, _T("XjqX"), *trackTextFormat, CD2DSizeF(100, 100));
		tmpTextLayout.Get()->GetMetrics(&minMetrics);
		trackHeight = minMetrics.height;
		minEventRenderWidth = minMetrics.width;
	}
	{
		DWRITE_TEXT_METRICS minMetrics;
		CD2DTextLayout tmpTextLayout(renderTarget, _T("XjqX"), *eventTextFormat, CD2DSizeF(100, 100));
		tmpTextLayout.Get()->GetMetrics(&minMetrics);
		auto eventsHeight = minMetrics.height;
		if (eventsHeight > trackHeight)
		{
			trackHeight = eventsHeight;
		}
	}
	resourcesCreated = true;
}

void CSchedulerDocumentRenderer::Render(CRenderTarget* renderTarget, const D2D1_POINT_2F& scrollPosition)
{	
	if (renderTarget && renderTarget->IsValid() && resourcesCreated)
	{		
		renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

		D2D1_MATRIX_3X2_F matrix = D2D1::Matrix3x2F::Identity() *
			D2D1::Matrix3x2F::Translation(-scrollPosition.x, -scrollPosition.y)
			* D2D1::Matrix3x2F::Scale(zoomLevel)
			;
		renderTarget->SetTransform(matrix);		
		
		renderTarget->FillRectangle(headerRect, trackBackgroundColorBrush);
		renderTarget->DrawRectangle(surfaceRect, gridLinesColorBrush, 1.f);
		for (const auto& trackLine : trackSeparationLines)
		{
			renderTarget->DrawLine(trackLine.first, trackLine.second, gridLinesColorBrush, 0.5f);
		}
		for (const auto& item : headerTimelineItems)
		{
			renderTarget->DrawText(item.text, item.textBounds, headerTimelineForegroundColorBrush, headerTimelineTextFormat);
			renderTarget->DrawLine(item.line.first, item.line.second, gridLinesColorBrush, 0.5f);
		}
		for (auto& track : tracks)
		{
			track->Render(renderTarget);			
		}
	}
}

D2D1_SIZE_F CSchedulerDocumentRenderer::GetZoomLevel() const
{
	return zoomLevel;
}
CTrackRenderer* CSchedulerDocumentRenderer::GetTrackAtPoint(const D2D1_POINT_2F& point)
{	
	for (const auto& track : tracks)
	{
		if (track->ContainsPoint(point)) return track.get();
	}
	return nullptr;
}

/*CEventRenderer* CSchedulerDocumentRenderer::GetEventAtPoint(const D2D1_POINT_2F& point)
{
	D2D1_POINT_2F zoomedPoint = D2D1::Point2F(point.x / zoomLevel.width, point.y / zoomLevel.height);
	for (const auto& track : tracks)
	{
		CEventRenderer* event = track->GetEventAtPoint(zoomedPoint);
		if (event != nullptr) return event;
	}
	return nullptr;
}
*/
void CSchedulerDocumentRenderer::Resize(int cx, int cy)
{	
	{
		D2D1_SIZE_U size;
		size.width = cx;
		size.height = cy;
		TRACE("RESIZE: %d %d\n", cx, cy);
//		renderTarget->Resize(size);
	}
}