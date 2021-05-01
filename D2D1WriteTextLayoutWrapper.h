#pragma once
#include "D2D1WrapperBase.h"
#include "D2D1WriteFactoryWrapper.h"
#include "D2D1WriteTextFormatWrapper.h"

#include <d2d1_1.h>

class CD2D1WriteTextLayoutWrapper :
	public CD2D1WrapperBase<IDWriteTextLayout>
{
public:
	CD2D1WriteTextLayoutWrapper(const CD2D1WriteFactoryWrapper& writeFactory,
		const CD2D1WriteTextFormatWrapper& textFormat, const CString& text, float width, float height);
/*	CD2D1WriteTextLayoutWrapper(CD2D1WriteTextLayoutWrapper&& other) : CD2D1WrapperBase<IDWriteTextLayout>(std::move(other)) {}
	CD2D1WriteTextLayoutWrapper& operator=(CD2D1WriteTextLayoutWrapper&& other)
	{
		swap(other);
		return *this;
	}
	*/
};

