#include "pch.h"
#include "TrackRenderer.h"

CTrackRenderer::CTrackRenderer(const CScheduleTrack& track, const CD2D1WriteFactoryWrapper& writeFactory):
	textFormat(writeFactory), textLayout(writeFactory,textFormat,track.GetName(),10000,10000)
{
	DWRITE_TEXT_METRICS metrics;
	textLayout->GetMetrics(&metrics);
	trackLabelSize.width = metrics.widthIncludingTrailingWhitespace;
	trackLabelSize.height = metrics.height;
	textLayout = std::move(CD2D1WriteTextLayoutWrapper(writeFactory, textFormat, track.GetName(), trackLabelSize.width, trackLabelSize.height));
	preferredSize = trackLabelSize;
	preferredSize.width += trackMargin;
	for (const auto& event : track.GetEvents())
	{
		eventRenderers.emplace_back(event, writeFactory);
		preferredSize.width += eventRenderers.back().GetPreferredSize().width + margin;
	}
}

D2D1_SIZE_F CTrackRenderer::GetPreferredSize() const
{
	return preferredSize;
}

void CTrackRenderer::Render(const CD2D1HwndRenderTargetWrapper& renderTarget, const D2D1_POINT_2F& position)
{
	backgroundColorBrush.initialize(renderTarget, D2D1::ColorF(D2D1::ColorF::LightGray, 0.5f));
	foregroundColorBrush.initialize(renderTarget, D2D1::ColorF(D2D1::ColorF::Black));
	auto rect = D2D1::RectF(position.x, position.y, position.x + trackLabelSize.width, position.y+trackLabelSize.height);
	renderTarget->FillRectangle(rect, backgroundColorBrush.get());
	renderTarget->DrawTextLayout(position,textLayout.get(),foregroundColorBrush.get());

	auto xOffset = position.x + trackLabelSize.width + trackMargin;
	for (auto& event : eventRenderers)
	{
		event.Render(renderTarget,D2D1::Point2F(xOffset,position.y));
		xOffset += event.GetPreferredSize().width + margin;
	}
}
