#pragma once

#include <xscBlock.h>
#include <hsuiPHComponent.h>

namespace hsui {

class PHBlock : public PHSingleton<PHBlock, PHComponent>
{
public:
	virtual void Setup() override;
	virtual void ReflectChange(xsc::Entity::PropID prop) override;

protected:
	virtual const wchar_t* GetDisplayType() const override;

	struct Name : public PropGridCtrl::IMediator
	{
		void ToModel(PropGridProperty*);
		void FromModel(PropGridProperty*);
	};
	Name mName;

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

	struct Width : public PropGridCtrl::IMediator
	{
		void ToModel(PropGridProperty*);
		void FromModel(PropGridProperty*);
	};
	Width mWidth;

	struct Height : public PropGridCtrl::IMediator
	{
		void ToModel(PropGridProperty*);
		void FromModel(PropGridProperty*);
	};
	Height mHeight;
};

} // namespace hsui
