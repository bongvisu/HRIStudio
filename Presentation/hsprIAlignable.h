#pragma once

#include <hsprModule.h>

namespace hspr {

class HSPR_API IAlignable
{
public:
	virtual void SetPrimary(bool primary) = 0;
	virtual bool GetPrimary() const = 0;
	
protected:
	virtual ~IAlignable();
};

} // namespace hspr
