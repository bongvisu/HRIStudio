#pragma once

#include <xscConnectable.h>

namespace xsc {

class XSC_API IPort
{
public:
	virtual MPVector GetConnectionPoint() const = 0;

	virtual void AddConnectionJoint(Connectable* connection) = 0;
	virtual void RemoveConnectionJoint(Connectable* connection) = 0;
	virtual const ConnectableSet& GetConnectionJoints() const = 0;

	virtual void SetAttachSide(MPRectangle::Edge attachSide) = 0;
	virtual MPRectangle::Edge GetAttachSide() const = 0;

	virtual void SetLocation(const MPVector& location) = 0;
	virtual const MPVector& GetLocation() const = 0;

protected:
	virtual ~IPort();
};

} // namespace xsc
