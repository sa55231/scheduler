#include "pch.h"
#include "D2D1SolidColorBrushWrapper.h"


CD2D1SolidColorBrushWrapper::CD2D1SolidColorBrushWrapper()
{
}


void CD2D1SolidColorBrushWrapper::initialize(const CD2D1HwndRenderTargetWrapper& renderTarget, const D2D1_COLOR_F &color)
{
	if (renderTarget && !member)
	{
		HRESULT hr = renderTarget->CreateSolidColorBrush(
			color,
			&member);
		if (FAILED(hr))
		{
			member = nullptr;
		}
	}
}