#pragma once

#include <hsprModule.h>

namespace hspr {

class HSPR_API IVisual
{
public:
	virtual void SetHot(bool hot) = 0;
	virtual bool GetHot() const = 0;

protected:
	virtual ~IVisual();
};

} // namespace hspr
