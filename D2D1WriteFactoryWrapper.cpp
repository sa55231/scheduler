#include "pch.h"
#include "D2D1WriteFactoryWrapper.h"

CD2D1WriteFactoryWrapper::CD2D1WriteFactoryWrapper()
{
	HRESULT hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&member));

	if (FAILED(hr))
	{
		member = nullptr;
	}
}

