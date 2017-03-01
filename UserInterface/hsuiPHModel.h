#pragma once

#include <hsmoModel.h>
#include <hsuiPHComponent.h>

namespace hsui {

class PHModel : public PHSingleton<PHModel, PHComponent>
{
public:
	virtual void Setup() override;
	virtual void ReflectChange(xsc::Entity::PropID prop) override;

protected:
	virtual const wchar_t* GetDisplayType() const override;

	struct Path : public PropGridCtrl::IMediator
	{
		void ToModel(PropGridProperty*);
		void FromModel(PropGridProperty*);
	};
	Path mPath;

	struct Name : public PropGridCtrl::IMediator
	{
		void ToModel(PropGridProperty*);
		void FromModel(PropGridProperty*);
	};
	Name mName;
};

} // namespace hsui
