#pragma once

#include <xscComponent.h>
#include <hsuiPHObject.h>

namespace hsui {

class PHComponent : public PHSingleton<PHComponent, PHObject>
{
public:
	virtual void Setup() override;

protected:
	virtual const wchar_t* GetDisplayType() const override;

	static const DWORD_PTR IDENTITY_KEY = 1;
};

} // namespace hsui
