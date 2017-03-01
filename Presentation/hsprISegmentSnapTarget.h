#pragma once

#include <xscMPRectangle.h>

namespace hspr {

class HSPR_API ISegmentSnapTarget
{
public:
	virtual int GetSegmentLength() const = 0;
	virtual xsc::MPVector::Axis GetSegmentAlignment() const = 0;
	virtual const xsc::MPVector& GetSnapPoint0() const = 0;
	virtual const xsc::MPVector& GetSnapPoint1() const = 0;
	virtual xsc::MPRectangle GetSnapBound() const = 0;

protected:
	virtual ~ISegmentSnapTarget();
};

} // namespace hspr
