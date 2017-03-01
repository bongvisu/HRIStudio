#pragma once

#include <xscMPRectangle.h>
#include <hsprModule.h>

namespace hspr {

class HSPR_API IPointSnapTarget
{
public:
	virtual const xsc::MPVector& GetSnapPoint() const = 0;
	virtual const xsc::MPRectangle& GetSnapBound() const = 0;

protected:
	virtual ~IPointSnapTarget();

protected:
	static ID2D1Bitmap1* msTargetImage;

private:
	friend Module;
	static void InitIPointSnapTarget();
	static void TermIPointSnapTarget();
};

} // namespace hspr
