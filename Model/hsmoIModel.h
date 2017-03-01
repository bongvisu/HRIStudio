#pragma once

#include <hsmoModule.h>

namespace hsmo {

class HSMO_API IModel
{
public:
	virtual const std::wstring& GetDirectory() const = 0;
	
protected:
	virtual ~IModel();
};

} // namespace hsmo
