#pragma once

#include <hsprModule.h>

namespace hspr {

class Presenter;

class HSPR_API ILevelView
{
public:
	virtual void InvalidateInteractionFor(Presenter* const presenter) = 0;

protected:
	virtual ~ILevelView();
};

} // namespace hspr
