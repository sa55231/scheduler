#include "pch.h"
#include "D2D1StrokeStyleWrapper.h"


CD2D1StrokeStyleWrapper::CD2D1StrokeStyleWrapper(const CD2D1FactoryWrapper& factory) : CD2D1WrapperBase()
{
	float dashes[] = { 1.0f, 2.0f, 2.0f, 3.0f, 2.0f, 2.0f };
	HRESULT hr = factory->CreateStrokeStyle(
		D2D1::StrokeStyleProperties(
		D2D1_CAP_STYLE_FLAT,
		D2D1_CAP_STYLE_FLAT,
		D2D1_CAP_STYLE_FLAT,
		D2D1_LINE_JOIN_MITER_OR_BEVEL,
		10.0f,
		D2D1_DASH_STYLE_SOLID,
		0.0f),
		NULL,
		0,
		&member);

	if (FAILED(hr))
	{
		member = nullptr;
	}
}

