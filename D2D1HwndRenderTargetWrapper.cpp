#include "pch.h"
#include "D2D1HwndRenderTargetWrapper.h"


CD2D1HwndRenderTargetWrapper::CD2D1HwndRenderTargetWrapper() 
{

}

void CD2D1HwndRenderTargetWrapper::initialize(const CD2D1FactoryWrapper& factory, HWND& hwnd, const RECT& rc)
{
	if (! member)
	{
		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
		HRESULT hr = factory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat()
			),
			D2D1::HwndRenderTargetProperties(hwnd, size),
			&member);
		if (FAILED(hr)) 
		{
			member = nullptr;
		}
	}
}