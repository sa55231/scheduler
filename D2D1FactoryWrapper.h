#pragma once

#include "D2D1WrapperBase.h"
#include <d2d1_1.h>


class CD2D1FactoryWrapper:public CD2D1WrapperBase<ID2D1Factory1>
{
public:
	CD2D1FactoryWrapper();
};

