#include "pch.h"
#include "SchedulerDocumentRenderer.h"
#include "D2D1WriteTextLayoutWrapper.h"

CSchedulerDocumentRenderer::CSchedulerDocumentRenderer() : selectionStrokeStyle(factory), zoomLevel(D2D1::SizeF(1.0f, 1.0f))
{
}

D2D1_SIZE_F CSchedulerDocumentRenderer::UpdateLayout(CSchedulerDoc* doc)
{
	tracks.clear();
	D2D1_SIZE_F size = D2D1::SizeF();
	FLOAT maxWidth = 0.f;
	for (const auto& track : doc->GetTracks())
	{
		tracks.emplace_back(track, writeFactory);
		auto trackSize = tracks.back().GetPreferredSize();
		if (maxWidth < trackSize.width)
		{
			maxWidth = trackSize.width;
		}
		size.height += trackSize.height + margin;
	}
	size.width = maxWidth + margin*2;
	return size;
}

void CSchedulerDocumentRenderer::Render(const RECT& targetRect, HWND& wnd, const CPoint& scrollPosition)
{
	renderTarget.initialize(factory, wnd, targetRect);
	
	if (renderTarget)
	{
		renderTarget->BeginDraw();
		TRACE("scroll pos %d %d\n", scrollPosition.x, scrollPosition.y);
		renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

		D2D1_MATRIX_3X2_F matrix = D2D1::Matrix3x2F::Identity() *
			D2D1::Matrix3x2F::Translation(static_cast<float>(-scrollPosition.x), static_cast<float>(-scrollPosition.y))
			* D2D1::Matrix3x2F::Scale(zoomLevel)
			;
		renderTarget->SetTransform(matrix);		

		float yOffset = 10;
		for (auto&& track : tracks)
		{
			track.Render(renderTarget,D2D1::Point2F(10,yOffset));
			yOffset += track.GetPreferredSize().height + margin;
		}
		TRACE("last yOffset : %f\n", yOffset);
		renderTarget->EndDraw();
	}
	else
	{
		renderTarget.release();
	}

}

void CSchedulerDocumentRenderer::Resize(int cx, int cy)
{
	if (renderTarget)
	{
		D2D1_SIZE_U size;
		size.width = cx;
		size.height = cy;
		TRACE("RESIZE: %d %d\n", cx, cy);
		renderTarget->Resize(size);
	}
}