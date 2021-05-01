#include "pch.h"
#include "D2D1FactoryWrapper.h"

CD2D1FactoryWrapper::CD2D1FactoryWrapper()
{
	HRESULT hr;
#if defined(DEBUG) || defined(_DEBUG)
	D2D1_FACTORY_OPTIONS options;
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;	
	hr = D2D1CreateFactory<ID2D1Factory1>(D2D1_FACTORY_TYPE_SINGLE_THREADED, options, &member);
#else
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &member);
#endif
	if (FAILED(hr))
	{
		member = nullptr;
	}
}

