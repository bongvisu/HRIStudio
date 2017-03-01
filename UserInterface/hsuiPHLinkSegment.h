#pragma once

#include <xscLinkSegment.h>
#include <hsuiPHComponent.h>

namespace hsui {

class PHLinkSegment : public PHSingleton<PHLinkSegment, PHComponent>
{
public:
	virtual void Setup() override;
	virtual void ReflectChange(xsc::Entity::PropID prop) override;

protected:
	virtual const wchar_t* GetDisplayType() const override;

	struct SourceLocation : public PropGridCtrl::IMediator
	{
		void ToModel(PropGridProperty*);
		void FromModel(PropGridProperty*);
	};
	SourceLocation mSourceLocation;

	struct DestinationLocation : public PropGridCtrl::IMediator
	{
		void ToModel(PropGridProperty*);
		void FromModel(PropGridProperty*);
	};
	DestinationLocation mDestinationLocation;
};

} // namespace hsui
