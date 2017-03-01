#pragma once

#include <hsmoModuleIOBlock.h>

namespace hsmo {

class HSMO_API IModuleBlock
{
public:
	virtual void HandleIOBlockAddition(const ModuleIOBlockSet& ioBlocks) = 0;
	virtual void HandleIOBlockRemoval(const ModuleIOBlockSet& ioBlocks) = 0;

	virtual int GetIndexFor(const xsc::Port* port) const = 0;

protected:
	virtual ~IModuleBlock();
};

} // namespace hsmo
