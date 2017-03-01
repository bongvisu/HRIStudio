#pragma once

#include <hsmoModuleIOBlock.h>

namespace hsmo {

class HSMO_API IModuleIOPort
{
public:
	virtual void SetComapionBlock(ModuleIOBlock* companionBlock) = 0;
	virtual ModuleIOBlock* GetCompanionBlock() const = 0;

protected:
	virtual ~IModuleIOPort();
};

} // namespace hsmo
