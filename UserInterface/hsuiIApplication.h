#pragma once

#include <hsuiModule.h>

namespace hsui {

class HSUI_API IApplication
{
public:
	virtual HINSTANCE GetApplicationInstanceHandle() const = 0;
	virtual CString GetApplicationTitle() const = 0;
	virtual UINT GetApplicationIconActive() const = 0;
	virtual UINT GetApplicationIconInactive() const = 0;
	virtual bool IsMdiActive() const = 0;

	virtual void UpdateLoopConsumers() = 0;

protected:
	virtual ~IApplication();
};

} // namespace hsui
