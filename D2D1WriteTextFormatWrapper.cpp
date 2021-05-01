#include "pch.h"
#include "D2D1WriteTextFormatWrapper.h"

CD2D1WriteTextFormatWrapper::CD2D1WriteTextFormatWrapper(const CD2D1WriteFactoryWrapper& writeFactory) : CD2D1WrapperBase()
{
	HRESULT hr = writeFactory->CreateTextFormat(L"Comic Sans MS",                // Font family name.
		NULL,                       // Font collection (NULL sets it to use the system font collection).
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		16.0f,
		L"en-us",
		&member);
	if (SUCCEEDED(hr))
	{
		hr = member->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	}

	if (SUCCEEDED(hr))
	{
		hr = member->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}

	if (SUCCEEDED(hr))
	{
		hr = member->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
	}

	if (FAILED(hr))
	{
		member = nullptr;
	}
}
