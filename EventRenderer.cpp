#include "pch.h"
#include "EventRenderer.h"

CEventRenderer::CEventRenderer(const CScheduleEvent& event, const CD2D1WriteFactoryWrapper& writeFactory):
	textFormat(writeFactory), textLayout(writeFactory, textFormat, event.GetName(), 10000, 10000),
	trimmingSign(writeFactory, textFormat), color(event.GetColor())
{
	DWRITE_TRIMMING trimming = { DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0 };
	textFormat->SetTrimming(&trimming, trimmingSign.get());
	
	DWRITE_TEXT_METRICS metrics;
	textLayout->GetMetrics(&metrics);
	eventLabelSize.width = std::chrono::duration_cast<std::chrono::minutes>(event.GetDuration()).count();
	eventLabelSize.height = metrics.height;
	textLayout = std::move(CD2D1WriteTextLayoutWrapper(writeFactory, textFormat, event.GetName(), eventLabelSize.width, eventLabelSize.height));

	{
		CD2D1WriteTextLayoutWrapper tmpTextLayout(writeFactory, textFormat, _T("XXX"), 10000, 10000);
		tmpTextLayout->GetMetrics(&minMetrics);
	}
}
D2D1_SIZE_F CEventRenderer::GetPreferredSize() const
{
	return eventLabelSize;
}
void CEventRenderer::Render(const CD2D1HwndRenderTargetWrapper& renderTarget, const D2D1_POINT_2F& position)
{
	backgroundColorBrush.initialize(renderTarget, D2D1::ColorF(color, 0.5f));
	foregroundColorBrush.initialize(renderTarget, D2D1::ColorF(D2D1::ColorF::Black));
	auto rect = D2D1::RectF(position.x, position.y, position.x + eventLabelSize.width, position.y + eventLabelSize.height);
	auto roundedRect = D2D1::RoundedRect(rect,eventLabelSize.height * 0.5, eventLabelSize.height * 0.5);
	renderTarget->FillRoundedRectangle(roundedRect, backgroundColorBrush.get());
	renderTarget->DrawRoundedRectangle(roundedRect, foregroundColorBrush.get());
	if (eventLabelSize.width > minMetrics.width)
	{
		renderTarget->DrawTextLayout(position, textLayout.get(), foregroundColorBrush.get());
	}
}
