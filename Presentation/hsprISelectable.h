#pragma once

#include <hsprModule.h>

namespace hspr {

class HSPR_API ISelectable
{
public:
	virtual void SetSelected(bool selected) = 0;
	virtual bool GetSelected() const = 0;

protected:
	virtual ~ISelectable();
};

} // namespace hspr
