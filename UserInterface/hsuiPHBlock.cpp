#include "hsuiPrecompiled.h"
#include "hsuiPHBlock.h"
#include "hsuiPropManager.h"
#include <hsmoModel.h>
#include <hsmoSetBlockLocationCommand.h>
#include <hsmoSetBlockDimensionCommand.h>

using namespace std;
using namespace xsc;
using namespace hsmo;

namespace hsui {

const wchar_t* PHBlock::GetDisplayType() const
{
	return L"Block";
}

void PHBlock::Setup()
{
	PHComponent::Setup();

	PropGridProperty* pgProp;
	PropGridProperty* pgGroup;
	PropGridCtrl& view = thePropManager->GetView();

	pgGroup = view.FindItemByData(IDENTITY_KEY);
	{
		pgProp = new PropGridProperty(L"Name", L"", nullptr, AsData(&mName));
		pgGroup->AddSubItem(pgProp);
	}

	pgGroup = new PropGridProperty(L"Geometry");
	{
		pgProp = new PropGridProperty(L"Location X", L"", nullptr, AsData(&mLocationX));
		pgGroup->AddSubItem(pgProp);
		
		pgProp = new PropGridProperty(L"Location Y", L"", nullptr, AsData(&mLocationY));
		pgGroup->AddSubItem(pgProp);

		pgProp = new PropGridProperty(L"Width", L"", nullptr, AsData(&mWidth));
		pgGroup->AddSubItem(pgProp);

		pgProp = new PropGridProperty(L"Height", L"", nullptr, AsData(&mHeight));
		pgGroup->AddSubItem(pgProp);
	}
	view.AddProperty(pgGroup);

	ReflectChange(Component::PROP_NAME);
	ReflectChange(Block::PROP_GEOMETRY);
}

void PHBlock::ReflectChange(Entity::PropID prop)
{
	if (prop == Component::PROP_NAME)
	{
		PropGridCtrl& view = thePropManager->GetView();
		PropGridProperty* pgProp = view.FindItemByData(AsData(&mName));
		mName.FromModel(pgProp);
	}
	else if (prop == Block::PROP_GEOMETRY)
	{
		PropGridCtrl& view = thePropManager->GetView();
		PropGridProperty* pgProp;

		pgProp = view.FindItemByData(AsData(&mLocationX));
		mLocationX.FromModel(pgProp);
	
		pgProp = view.FindItemByData(AsData(&mLocationY));
		mLocationY.FromModel(pgProp);
	
		pgProp = view.FindItemByData(AsData(&mWidth));
		mWidth.FromModel(pgProp);
	
		pgProp = view.FindItemByData(AsData(&mHeight));
		mHeight.FromModel(pgProp);
	}
	else
	{
		PHComponent::ReflectChange(prop);
	}
}

// Name ---------------------------------------------------------------------------------------------------------------
void PHBlock::Name::FromModel(PropGridProperty* pgProp)
{
	pgProp->Enable(TRUE);

	const BlockSet& blocks = thePropManager->GetTargets<BlockSet>();
	BlockSet::iterator iter = blocks.begin();
	const auto& pivotValue = (*iter)->GetName();

	bool sameAll = true;
	while (++iter != blocks.end())
	{
		const auto& name = (*iter)->GetName();
		if (name != pivotValue)
		{
			sameAll = false;
			break;
		}
	}
	if (sameAll)
	{
		pgProp->SetValue(pivotValue.c_str());
	}
	else
	{
		pgProp->SetValue(L"");
	}
}

void PHBlock::Name::ToModel(PropGridProperty* pgProp)
{
	const ComponentSet& components = thePropManager->GetTargets<ComponentSet>();

	wstring value(pgProp->GetValue().bstrVal);
	for (auto& component : components)
	{
		component->SetName(value);
	}
}
// Name ---------------------------------------------------------------------------------------------------------------

// LocationX ----------------------------------------------------------------------------------------------------------
void PHBlock::LocationX::FromModel(PropGridProperty* pgProp)
{
	pgProp->Enable(TRUE);

	const BlockSet& blocks = thePropManager->GetTargets<BlockSet>();
	BlockSet::iterator iter = blocks.begin();
	const MPRectangle& pivotGeometry = (*iter)->GetGeometry();
	int pivotValue = pivotGeometry.l;

	bool sameAll = true;
	while (++iter != blocks.end())
	{
		const MPRectangle& otherGeometry = (*iter)->GetGeometry();
		if (otherGeometry.l != pivotValue)
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

void PHBlock::LocationX::ToModel(PropGridProperty* pgProp)
{
	const ComponentSet& components = thePropManager->GetTargets<ComponentSet>();
	int value = _wtoi(pgProp->GetValue().bstrVal);

	ModelToComponentSet modelToComponentSet;
	Model::SortOutPerContext(components, modelToComponentSet);

	for (auto& item : modelToComponentSet)
	{
		SetBlockLocationCommand* command = new SetBlockLocationCommand(
			std::move(reinterpret_cast<BlockSet&>(item.second)), MPVector::X, value);
		item.first->ExecuteCommand(command);
	}
}
// LocationX ----------------------------------------------------------------------------------------------------------

// LocationY ----------------------------------------------------------------------------------------------------------
void PHBlock::LocationY::FromModel(PropGridProperty* pgProp)
{
	pgProp->Enable(TRUE);

	const BlockSet& blocks = thePropManager->GetTargets<BlockSet>();
	BlockSet::iterator iter = blocks.begin();
	const MPRectangle& pivotGeometry = (*iter)->GetGeometry();
	int pivotValue = pivotGeometry.t;

	bool sameAll = true;
	while (++iter != blocks.end())
	{
		const MPRectangle& otherGeometry = (*iter)->GetGeometry();
		if (otherGeometry.t != pivotValue)
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

void PHBlock::LocationY::ToModel(PropGridProperty* pgProp)
{
	const ComponentSet& components = thePropManager->GetTargets<ComponentSet>();
	int value = _wtoi(pgProp->GetValue().bstrVal);

	ModelToComponentSet modelToComponentSet;
	Model::SortOutPerContext(components, modelToComponentSet);

	for (auto& item : modelToComponentSet)
	{
		SetBlockLocationCommand* command = new SetBlockLocationCommand(
			std::move(reinterpret_cast<BlockSet&>(item.second)), MPVector::Y, value);
		item.first->ExecuteCommand(command);
	}
}
// LocationX ----------------------------------------------------------------------------------------------------------

// Width --------------------------------------------------------------------------------------------------------------
void PHBlock::Width::FromModel(PropGridProperty* pgProp)
{
	pgProp->Enable(TRUE);

	const BlockSet& blocks = thePropManager->GetTargets<BlockSet>();

	BlockSet::iterator iter = blocks.begin();
	const MPRectangle& pivotGeometry = (*iter)->GetGeometry();
	int pivotValue = pivotGeometry.r - pivotGeometry.l;

	bool sameAll = true;
	while (++iter != blocks.end())
	{
		const MPRectangle& otherGeometry = (*iter)->GetGeometry();
		int otherValue = otherGeometry.r - otherGeometry.l;
		if (otherValue != pivotValue)
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

void PHBlock::Width::ToModel(PropGridProperty* pgProp)
{
	const ComponentSet& components = thePropManager->GetTargets<ComponentSet>();
	int value = _wtoi(pgProp->GetValue().bstrVal);

	ModelToComponentSet modelToComponentSet;
	Model::SortOutPerContext(components, modelToComponentSet);

	for (auto& item : modelToComponentSet)
	{
		SetBlockDimensionCommand* command = new SetBlockDimensionCommand(
			std::move(reinterpret_cast<BlockSet&>(item.second)), MPVector::X, value);
		item.first->ExecuteCommand(command);
	}
}
// Width --------------------------------------------------------------------------------------------------------------

// Height -------------------------------------------------------------------------------------------------------------
void PHBlock::Height::FromModel(PropGridProperty* pgProp)
{
	pgProp->Enable(TRUE);

	const BlockSet& blocks = thePropManager->GetTargets<BlockSet>();

	BlockSet::iterator iter = blocks.begin();
	const MPRectangle& pivotGeometry = (*iter)->GetGeometry();
	int pivotValue = pivotGeometry.b - pivotGeometry.t;

	bool sameAll = true;
	while (++iter != blocks.end())
	{
		const MPRectangle& otherGeometry = (*iter)->GetGeometry();
		int otherValue = otherGeometry.b - otherGeometry.t;
		if (otherValue != pivotValue)
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

void PHBlock::Height::ToModel(PropGridProperty* pgProp)
{
	const ComponentSet& components = thePropManager->GetTargets<ComponentSet>();
	int value = _wtoi(pgProp->GetValue().bstrVal);

	ModelToComponentSet modelToComponentSet;
	Model::SortOutPerContext(components, modelToComponentSet);

	for (auto& item : modelToComponentSet)
	{
		SetBlockDimensionCommand* command = new SetBlockDimensionCommand(
			std::move(reinterpret_cast<BlockSet&>(item.second)), MPVector::Y, value);
		item.first->ExecuteCommand(command);
	}
}
// Height -------------------------------------------------------------------------------------------------------------

} // namespace hsui
