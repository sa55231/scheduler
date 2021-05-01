#pragma once

#include "D2D1HwndRenderTargetWrapper.h"
#include "D2D1WriteFactoryWrapper.h"
#include "D2D1StrokeStyleWrapper.h"
#include "D2D1SolidColorBrushWrapper.h"
#include "D2D1WriteTextFormatWrapper.h"

#include "TrackRenderer.h"

#include "schedulerDoc.h"

class CSchedulerDocumentRenderer
{
public:
	CSchedulerDocumentRenderer();
	void Render(const RECT& targetRect, HWND& wnd, const CPoint& scrollPosition);
	void Resize(int cx, int cy);
	D2D1_SIZE_F UpdateLayout(CSchedulerDoc* doc);
private:
	CD2D1FactoryWrapper factory;
	CD2D1WriteFactoryWrapper writeFactory;
	CD2D1StrokeStyleWrapper selectionStrokeStyle;
	CD2D1HwndRenderTargetWrapper renderTarget;
	D2D1_SIZE_F zoomLevel;
	std::vector<CTrackRenderer> tracks;
	FLOAT margin = 10.f;
};

