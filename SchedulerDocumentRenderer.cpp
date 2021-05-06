#include "pch.h"
#include "SchedulerDocumentRenderer.h"


CSchedulerDocumentRenderer::CSchedulerDocumentRenderer() : zoomLevel(D2D1::SizeF(1.0f, 1.0f))
{
}

D2D1_SIZE_F CSchedulerDocumentRenderer::UpdateLayout(CSchedulerDoc* doc, CHwndRenderTarget* renderTarget, IDWriteFactory* directWriteFactory)
{
	CreateD2D1Resources(renderTarget, directWriteFactory);

	tracks.clear();
	D2D1_SIZE_F size = D2D1::SizeF(margin,margin);
	FLOAT maxWidth = 0.f;
	
	for (const auto& track : doc->GetTracks())
	{
		auto trackRenderer = std::make_unique<CTrackRenderer>(track, trackTextFormat, trackBackgroundColorBrush,
			trackForegroundColorBrush);		
		float trackWidth = 60.f + margin;
		trackRenderer->SetPreferredSize(D2D1::SizeF(trackWidth,trackHeight));
		std::vector<std::unique_ptr<CEventRenderer>> eventRenderers;
		auto xOffset = trackWidth;
		for (const auto& event : track.GetEvents())
		{
			auto color = event.GetColor();
			if (eventBackgroundColorBrushes.find(color) == eventBackgroundColorBrushes.end())
			{
				eventBackgroundColorBrushes[color] = new CD2DSolidColorBrush(renderTarget, D2D1::ColorF(color));
			}
			auto eventRenderer = std::make_unique<CEventRenderer>(event, eventTextFormat, 
				eventBackgroundColorBrushes[color], eventForegroundColorBrush);
			eventRenderer->SetMinimumTextRenderingWidth(minEventRenderWidth);
			auto eventWidth = eventRenderer->GetWidth() + margin;
			eventRenderer->SetEventBounds(D2D1::RectF(xOffset,size.height,xOffset+eventWidth,size.height+trackHeight));
			trackWidth += eventWidth;
			xOffset += eventWidth;
			eventRenderers.push_back(std::move(eventRenderer));
		}
		trackRenderer->SetTrackBounds(D2D1::RectF(0.f, size.height,xOffset,trackHeight + size.height));
		trackRenderer->SetEventRenderers(std::move(eventRenderers));

		if (maxWidth < trackWidth)
		{
			maxWidth = trackWidth;
		}
		size.height += trackHeight + margin;
		tracks.push_back(std::move(trackRenderer));
	}
	size.width += maxWidth + margin;
	
	TRACE("Surface size : %fx%f\n", size.width , size.height);
	return size;
}

void CSchedulerDocumentRenderer::CreateD2D1Resources(CHwndRenderTarget* renderTarget, IDWriteFactory* directWriteFactory)
{
	if (!trackTextFormat)
	{
		trackTextFormat = new CD2DTextFormat(renderTarget, _T("Times New Roman"), 16.f);
	}
	if (!trackBackgroundColorBrush)
	{
		trackBackgroundColorBrush = new CD2DSolidColorBrush(renderTarget, D2D1::ColorF(D2D1::ColorF::LightGray, 0.5f));
	}
	if (!trackForegroundColorBrush)
	{
		trackForegroundColorBrush = new CD2DSolidColorBrush(renderTarget, D2D1::ColorF(D2D1::ColorF::Black));
	}
	if (!eventTextFormat)
	{
		eventTextFormat = new CD2DTextFormat(renderTarget, _T("Times New Roman"), 16.f);
		eventTextFormat->Get()->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		eventTextFormat->Get()->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		eventTextFormat->Get()->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

		directWriteFactory->CreateEllipsisTrimmingSign(eventTextFormat->Get(), &trimmingSign);
		DWRITE_TRIMMING trimming = { DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0 };
		eventTextFormat->Get()->SetTrimming(&trimming, trimmingSign);
	}
	if (!eventForegroundColorBrush)
	{
		eventForegroundColorBrush = new CD2DSolidColorBrush(renderTarget, D2D1::ColorF(D2D1::ColorF::Black));
	}
	{
		DWRITE_TEXT_METRICS minMetrics;
		CD2DTextLayout tmpTextLayout(renderTarget, _T("XjqX"), *trackTextFormat, CD2DSizeF(100, 100));
		tmpTextLayout.Get()->GetMetrics(&minMetrics);
		trackHeight = minMetrics.height;
		minEventRenderWidth = minMetrics.width;
	}

}

void CSchedulerDocumentRenderer::Render(CHwndRenderTarget* renderTarget, const D2D1_POINT_2F& scrollPosition)
{	
	if (renderTarget && renderTarget->IsValid())
	{
		TRACE("scroll pos %d %d\n", scrollPosition.x, scrollPosition.y);
		renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

		D2D1_MATRIX_3X2_F matrix = D2D1::Matrix3x2F::Identity() *
			D2D1::Matrix3x2F::Translation(-scrollPosition.x, -scrollPosition.y)
			* D2D1::Matrix3x2F::Scale(zoomLevel)
			;
		renderTarget->SetTransform(matrix);		
		
		for (auto&& track : tracks)
		{
			track->Render(renderTarget);	
		}
	}
}

CTrackRenderer* CSchedulerDocumentRenderer::GetTrackAtPoint(const D2D1_POINT_2F& point)
{
	for (const auto& track : tracks)
	{
		if (track->ContainsPoint(point)) return track.get();
	}
	return nullptr;
}

CEventRenderer* CSchedulerDocumentRenderer::GetEventAtPoint(const D2D1_POINT_2F& point)
{
	for (const auto& track : tracks)
	{
		CEventRenderer* event = track->GetEventAtPoint(point);
		if (event != nullptr) return event;
	}
	return nullptr;
}

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