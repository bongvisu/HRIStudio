#pragma once

#include <xscModule.h>

namespace xsc {

class XSC_API DWrite
{
public:
	static IDWriteFactory1* factory;

	static int MeasureTextExtent(IDWriteTextFormat* textFormat, const std::wstring& text);

	static IDWriteTextFormat*	segoe12R;
	static IDWriteTextFormat*	segoe14R;
	static IDWriteTextFormat*	segoe16R;
	static IDWriteTextFormat*	segoe18R;
	static IDWriteTextFormat*	segoe20R;

	static IDWriteTextFormat*	segoe12B;
	static IDWriteTextFormat*	segoe14B;
	static IDWriteTextFormat*	segoe16B;
	static IDWriteTextFormat*	segoe18B;
	static IDWriteTextFormat*	segoe20B;

public:
	static void Init_DWrite();
	static void Term_DWrite();
};

} // namespace xsc
