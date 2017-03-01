#pragma once

#include <hsuiPHComponent.h>

namespace hsui {

class PHLinkJoint : public PHSingleton<PHLinkJoint, PHComponent>
{
public:
	virtual void Setup() override;
	virtual void ReflectChange(xsc::Entity::PropID prop) override;

protected:
	virtual const wchar_t* GetDisplayType() const override;

	struct LocationX : public PropGridCtrl::IMediator
	{
		void ToModel(PropGridProperty*);
		void FromModel(PropGridProperty*);
	};
	LocationX mLocationX;

	struct LocationY : public PropGridCtrl::IMediator
	{
		void ToModel(PropGridProperty*);
		void FromModel(PropGridProperty*);
	};
	LocationY mLocationY;
};

} // namespace hsui
