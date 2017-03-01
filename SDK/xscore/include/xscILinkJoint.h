#pragma once

#include <xscConnectable.h>

namespace xsc {

class XSC_API ILinkJoint
{
public:
	virtual void SetSource(Connectable* connectable) = 0;
	virtual Connectable* GetSource() const = 0;

	virtual void SetDestination(Connectable* connectable) = 0;
	virtual Connectable* GetDestination() const = 0;
		
	virtual void SetLocation(const MPVector& location) = 0;
	virtual const MPVector& GetLocation() const = 0;

protected:
	virtual ~ILinkJoint();
};

} // namespace xsc
