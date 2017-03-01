#pragma once

#include <hsuiPropHandler.h>

namespace hsui {

class PHObject : public PHSingleton<PHObject, PropHandler>
{
public:
	virtual void Setup() override;
};

} // namespace hsui
