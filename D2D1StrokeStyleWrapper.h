#pragma once
#include "D2D1WrapperBase.h"
#include <d2d1_1.h>
#include "D2D1FactoryWrapper.h"

class CD2D1StrokeStyleWrapper :
	public CD2D1WrapperBase<ID2D1StrokeStyle>
{
public:
	CD2D1StrokeStyleWrapper(const CD2D1FactoryWrapper& factory);
	
};

