#include "pch.h"
#include "D2D1WriteTrimmingSignWrapper.h"

CD2D1WriteTrimmingSignWrapper::CD2D1WriteTrimmingSignWrapper(const CD2D1WriteFactoryWrapper& writeFactory,
	const CD2D1WriteTextFormatWrapper& textFormat) : CD2D1WrapperBase()
{
	HRESULT hr = writeFactory->CreateEllipsisTrimmingSign(textFormat.get(), &member);

	if (FAILED(hr))
	{
		member = nullptr;
	}
}