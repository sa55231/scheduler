#pragma once
#include "D2D1WrapperBase.h"
#include "D2D1HwndRenderTargetWrapper.h"

class CD2D1SolidColorBrushWrapper :
	public CD2D1WrapperBase<ID2D1SolidColorBrush>
{
public:
	CD2D1SolidColorBrushWrapper();
	void initialize(const CD2D1HwndRenderTargetWrapper& renderTarget, const D2D1_COLOR_F &color);
};

