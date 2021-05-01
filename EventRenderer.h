#pragma once
#include "D2D1HwndRenderTargetWrapper.h"
#include "D2D1WriteFactoryWrapper.h"
#include "D2D1SolidColorBrushWrapper.h"
#include "D2D1WriteTextLayoutWrapper.h"
#include "D2D1WriteTextFormatWrapper.h"
#include "D2D1WriteTrimmingSignWrapper.h"

#include "CScheduleEvent.h"

class CEventRenderer
{
public:
	CEventRenderer(const CScheduleEvent& event, const CD2D1WriteFactoryWrapper& writeFactory);
	void Render(const CD2D1HwndRenderTargetWrapper& renderTarget, const D2D1_POINT_2F& position);
	D2D1_SIZE_F GetPreferredSize() const;
private:
	CD2D1WriteTextFormatWrapper textFormat;
	CD2D1WriteTextLayoutWrapper textLayout;
	D2D1_SIZE_F eventLabelSize;
	CD2D1SolidColorBrushWrapper backgroundColorBrush;
	CD2D1SolidColorBrushWrapper foregroundColorBrush;
	CD2D1WriteTrimmingSignWrapper trimmingSign;
	DWRITE_TEXT_METRICS minMetrics;
	UINT32 color = 0;
};

