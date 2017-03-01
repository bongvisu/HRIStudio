#pragma once

#include <xscConnectable.h>

namespace xsc {

class XSC_API ILinkSegment
{
public:
	virtual void SetSourceJoint(Connectable* connectable) = 0;
	virtual Connectable* GetSourceJoint() const = 0;

	virtual void SetDestinationJoint(Connectable* connectable) = 0;
	virtual Connectable* GetDestinationJoint() const = 0;

	virtual void SetSourceLocation(const MPVector& location) = 0;
	virtual const MPVector& GetSourceLocation() const = 0;

	virtual void SetDestinationLocation(const MPVector& location) = 0;
	virtual const MPVector& GetDestinationLocation() const = 0;

	virtual MPVector::Axis GetAlignment() const = 0;

protected:
	virtual ~ILinkSegment();
};

} // namespace xsc
