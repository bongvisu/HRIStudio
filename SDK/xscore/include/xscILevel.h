#pragma once

#include <xscLink.h>
#include <xscBlock.h>

namespace xsc {

class XSC_API ILevel
{
public:
	virtual void HandlePortRemoval(const PortSet& ports) = 0;
	virtual void OnChildGeometryChange() = 0;

protected:
	virtual ~ILevel();
};

} // namespace xsc
