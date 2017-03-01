#pragma once

#include <xscModule.h>

namespace xsc {

class XSC_API MPVector
{
public:
	enum Dimension	{ X, Y };
	enum Axis		{ AXIS_X = 0, AXIS_Y = 1, AXIS_OTHER = 2 };
	enum Direction	{ POSITIVE_X = 0, POSITIVE_Y, NEGATIVE_X, NEGATIVE_Y, NUMBER_OF_DIRECTIONS };
	
	MPVector();
	MPVector(int x, int y);
	MPVector(const MPVector& vector);
	MPVector& operator=(const MPVector& vector);

	MPVector operator+(const MPVector& vector) const;
	MPVector operator-(const MPVector& vector) const;
	MPVector& operator+=(const MPVector& vector);
	MPVector& operator-=(const MPVector& vector);

	bool operator==(const MPVector& vector) const;
	bool operator!=(const MPVector& vector) const;

	int& operator[](int i);
	int operator[](int i) const;

	void MakeInvalid();
	bool IsInvalid() const;

	void MakeZero();
	bool IsZero() const;
	bool IsNotZero() const;

	bool IsAligned() const;
	bool IsNotAligned() const;

	Axis GetMajorAxis() const;
	Direction GetDirection() const;
	int GetAlignedLength() const;

	static Axis GetAlignment(const MPVector& point0, const MPVector& point1);

	int x, y;

public:
	D2D1_POINT_2F ToPointF() const;
};

} // namespace xsc
