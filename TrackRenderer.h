#pragma once

#include "D2D1HwndRenderTargetWrapper.h"
#include "D2D1WriteFactoryWrapper.h"
#include "D2D1WriteTextLayoutWrapper.h"
#include "D2D1WriteTextFormatWrapper.h"
#include "D2D1SolidColorBrushWrapper.h"

#include "CScheduleTrack.h"
#include "EventRenderer.h"

#include <vector>

class CTrackRenderer
{
public:
	CTrackRenderer(const CScheduleTrack& track, const CD2D1WriteFactoryWrapper& writeFactory);
	void Render(const CD2D1HwndRenderTargetWrapper& renderTarget, const D2D1_POINT_2F& position);
	D2D1_SIZE_F GetPreferredSize() const;
private:
	CD2D1WriteTextFormatWrapper textFormat;
	CD2D1WriteTextLayoutWrapper textLayout;
	D2D1_SIZE_F trackLabelSize;
	CD2D1SolidColorBrushWrapper backgroundColorBrush;
	CD2D1SolidColorBrushWrapper foregroundColorBrush;
	D2D1_SIZE_F preferredSize;
	FLOAT margin = 0.f;
	FLOAT trackMargin = 10.f;
	std::vector<CEventRenderer> eventRenderers;
};

