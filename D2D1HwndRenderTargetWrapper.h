#pragma once
#include "D2D1WrapperBase.h"
#include <d2d1_1.h>
#include "D2D1FactoryWrapper.h"

class CD2D1HwndRenderTargetWrapper :
	public CD2D1WrapperBase<ID2D1HwndRenderTarget>
{
public:
	CD2D1HwndRenderTargetWrapper();
	void initialize(const CD2D1FactoryWrapper& factory, HWND& hwnd , const RECT& rc);
};

