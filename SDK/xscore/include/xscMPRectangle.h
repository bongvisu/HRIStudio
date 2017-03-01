#pragma once

#include <xscMPVector.h>

namespace xsc {

class XSC_API MPRectangle
{
public:
	enum Edge
	{
		EDGE_L = 0,
		EDGE_T,
		EDGE_R,
		EDGE_B,
		NUMBER_OF_EDGES
	};

	enum Corner
	{
		CORNER_LT = 0,
		CORNER_RT,
		CORNER_RB,
		CORNER_LB,
		NUMBER_OF_CORNERS
	};
	
	MPRectangle();
	MPRectangle(int l, int t, int r, int b);
	MPRectangle(const MPRectangle& rectangle);
	MPRectangle(const MPVector& pt0, const MPVector& pt1);
	MPRectangle& operator=(const MPRectangle& rectangle);

	bool operator==(const MPRectangle& rectangle) const;
	bool operator!=(const MPRectangle& rectangle) const;

	void MakeInvalid();
	bool IsInvalid() const;

	void MakeZero();
	bool IsZero() const;
	
	void Set(int l, int t, int r, int b);
	void Get(int& l, int& t, int& r, int& b) const;

	int GetWidth() const;
	int GetHeight() const;

	void Inflate(const MPVector& delta);
	void Offset(const MPVector& delta);

	bool Contains(const MPVector& point) const;
	bool Contains(const MPRectangle& rectangle) const;

	bool Intersects(const MPVector& point0, const MPVector& point1) const;

	MPVector GetLeftTop() const;
	MPVector GetLeftBottom() const;
	MPVector GetRighTop() const;
	MPVector GetRightBottom() const;

	void Combine(const MPRectangle& rect);

	int l, t, r, b;

public:
	D2D1_RECT_F ToRectF() const;
};

} // namespace xsc
