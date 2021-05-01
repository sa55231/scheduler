#pragma once
#include "D2D1WrapperBase.h"
#include "D2D1WriteFactoryWrapper.h"
#include "D2D1WriteTextFormatWrapper.h"

#include <d2d1_1.h>

class CD2D1WriteTrimmingSignWrapper :
    public CD2D1WrapperBase<IDWriteInlineObject>
{
public:
	CD2D1WriteTrimmingSignWrapper(const CD2D1WriteFactoryWrapper& writeFactory,
		const CD2D1WriteTextFormatWrapper& textFormat);

};

