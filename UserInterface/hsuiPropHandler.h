#pragma once

#include <xscEntity.h>
#include <hsuiPropGridCtrl.h>

namespace hsui {

class PropHandler
{
public:
	virtual void Setup();
	virtual void ReflectChange(xsc::Entity::PropID prop);

protected:
	virtual const wchar_t* GetDisplayType() const;

	inline DWORD_PTR AsData(PropGridCtrl::IMediator* const mediator)
	{
		return reinterpret_cast<DWORD_PTR>(mediator);
	}
};

template <typename T, typename Base>
class PHSingleton : public Base
{
public:
	static T* GetInstance()
	{
		static T theInstance;
		return &theInstance;
	}
};

} // namespace hsui
