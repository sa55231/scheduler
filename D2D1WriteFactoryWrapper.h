#pragma once
#include "D2D1WrapperBase.h"
#include <d2d1_1.h>
#include <dwrite.h>

class CD2D1WriteFactoryWrapper :
	public CD2D1WrapperBase<IDWriteFactory>
{
public:
	CD2D1WriteFactoryWrapper();	
};

