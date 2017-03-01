#pragma once

#include <xscModule.h>

namespace xsc {

class XSC_API MPColor
{
public:
	MPColor();
	MPColor(float r, float g, float b, float a = 1.0f);
	MPColor(const MPColor& color);
	MPColor& operator=(const MPColor& color);

	bool operator==(const MPColor& color) const;
	bool operator!=(const MPColor& color) const;

	float r, g, b, a;

public:
	D2D1_COLOR_F ToColorF() const;
};

} // namespace xsc
