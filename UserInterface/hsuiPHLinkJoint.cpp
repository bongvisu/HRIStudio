#include "hsuiPrecompiled.h"
#include "hsuiPHLinkJoint.h"
#include "hsuiPropManager.h"
#include <xscLinkJoint.h>
#include <hsmoModel.h>
#include <hsmoSetJointLocationCommand.h>

using namespace std;
using namespace xsc;
using namespace hsmo;

namespace hsui {

const wchar_t* PHLinkJoint::GetDisplayType() const
{
	return L"Joint";
}

void PHLinkJoint::Setup()
{
	PHComponent::Setup();

	PropGridProperty* pgProp;
	PropGridProperty* pgGroup;
	PropGridCtrl& view = thePropManager->GetView();

	pgGroup = new PropGridProperty(L"Geometry");
	{
		pgProp = new PropGridProperty(L"Location X", L"", nullptr, AsData(&mLocationX));
		pgGroup->AddSubItem(pgProp);

		pgProp = new PropGridProperty(L"Location Y", L"", nullptr, AsData(&mLocationY));
		pgGroup->AddSubItem(pgProp);
	}
	view.AddProperty(pgGroup);

	ReflectChange(LinkJoint::PROP_LOCATION);
}

void PHLinkJoint::ReflectChange(Entity::PropID prop)
{
	if (prop == LinkJoint::PROP_LOCATION)
	{
		PropGridCtrl& view = thePropManager->GetView();
		PropGridProperty* pgProp;

		pgProp = view.FindItemByData(AsData(&mLocationX));
		mLocationX.FromModel(pgProp);
	
		pgProp = view.FindItemByData(AsData(&mLocationY));
		mLocationY.FromModel(pgProp);
	}
	else
	{
		PHComponent::ReflectChange(prop);
	}
}

// LocationX ----------------------------------------------------------------------------------------------------------
void PHLinkJoint::LocationX::FromModel(PropGridProperty* pgProp)
{
	pgProp->Enable(TRUE);

	const LinkJointSet& joints = thePropManager->GetTargets<LinkJointSet>();
	LinkJointSet::iterator iter = joints.begin();
	const MPVector& pivotLocation = (*iter)->GetLocation();
	int pivotValue = pivotLocation.x;

	bool sameAll = true;
	while (++iter != joints.end())
	{
		const MPVector& otherLocation = (*iter)->GetLocation();
		if (otherLocation.x != pivotValue)
		{
			sameAll = false;
			break;
		}
	}
	if (sameAll)
	{
		CString value;
		value.Format(L"%d", pivotValue);
		pgProp->SetValue(value);
	}
	else
	{
		pgProp->SetValue(L"");
	}
}

void PHLinkJoint::LocationX::ToModel(PropGridProperty* pgProp)
{
	const ComponentSet& components = thePropManager->GetTargets<ComponentSet>();
	int value = _wtoi(pgProp->GetValue().bstrVal);

	ModelToComponentSet modelToComponentSet;
	Model::SortOutPerContext(components, modelToComponentSet);

	for (auto& item : modelToComponentSet)
	{
		SetJointLocationCommand* command = new SetJointLocationCommand(
			std::move(reinterpret_cast<LinkJointSet&>(item.second)), MPVector::X, value);
		item.first->ExecuteCommand(command);
	}
}
// LocationX ----------------------------------------------------------------------------------------------------------

// LocationY ----------------------------------------------------------------------------------------------------------
void PHLinkJoint::LocationY::FromModel(PropGridProperty* pgProp)
{
	pgProp->Enable(TRUE);

	const LinkJointSet& joints = thePropManager->GetTargets<LinkJointSet>();
	LinkJointSet::iterator iter = joints.begin();
	const MPVector& pivotLocation = (*iter)->GetLocation();
	int pivotValue = pivotLocation.y;

	bool sameAll = true;
	while (++iter != joints.end())
	{
		const MPVector& otherLocation = (*iter)->GetLocation();
		if (otherLocation.y != pivotValue)
		{
			sameAll = false;
			break;
		}
	}
	if (sameAll)
	{
		CString value;
		value.Format(L"%d", pivotValue);
		pgProp->SetValue(value);
	}
	else
	{
		pgProp->SetValue(L"");
	}
}

void PHLinkJoint::LocationY::ToModel(PropGridProperty* pgProp)
{
	const ComponentSet& components = thePropManager->GetTargets<ComponentSet>();
	int value = _wtoi(pgProp->GetValue().bstrVal);

	ModelToComponentSet modelToComponentSet;
	Model::SortOutPerContext(components, modelToComponentSet);

	SetJointLocationCommand* command;
	for (auto& item : modelToComponentSet)
	{
		command = new SetJointLocationCommand(
			std::move(reinterpret_cast<LinkJointSet&>(item.second)), MPVector::Y, value);
		item.first->ExecuteCommand(command);
	}
}
// LocationY ----------------------------------------------------------------------------------------------------------

} // namespace hsui
