#include "pch.h"
#include "D2D1WriteTextLayoutWrapper.h"


CD2D1WriteTextLayoutWrapper::CD2D1WriteTextLayoutWrapper(const CD2D1WriteFactoryWrapper& writeFactory,
	const CD2D1WriteTextFormatWrapper& textFormat, const CString& text, float width, float height) : CD2D1WrapperBase()
{
	HRESULT hr = writeFactory->CreateTextLayout(text, text.GetLength(),
		textFormat.get(), width, height,
		&member);
	if (SUCCEEDED(hr))
	{
		hr = member->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	}

	if (SUCCEEDED(hr))
	{
		hr = member->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}
	if (FAILED(hr))
	{
		member = nullptr;
	}
}

