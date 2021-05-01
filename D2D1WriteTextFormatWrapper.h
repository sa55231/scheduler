#pragma once
#include "D2D1WrapperBase.h"
#include "D2D1WriteFactoryWrapper.h"

#include <d2d1_1.h>

class CD2D1WriteTextFormatWrapper :
	public CD2D1WrapperBase<IDWriteTextFormat>
{
public:
	CD2D1WriteTextFormatWrapper(const CD2D1WriteFactoryWrapper& writeFactory);	

};

