#include "hsuiPrecompiled.h"
#include "hsuiPHModel.h"
#include "hsuiPropManager.h"

using namespace std;
using namespace xsc;
using namespace hsmo;

namespace hsui {

const wchar_t* PHModel::GetDisplayType() const
{
	return L"Model";
}

void PHModel::Setup()
{
	PHComponent::Setup();

	PropGridProperty* pgProp;
	PropGridProperty* pgGroup;
	PropGridCtrl& view = thePropManager->GetView();

	pgGroup = view.FindItemByData(IDENTITY_KEY);
	{
		pgProp = new PropGridProperty(L"Path", L"", nullptr, AsData(&mPath));
		mPath.FromModel(pgProp);
		pgGroup->AddSubItem(pgProp);

		pgProp = new PropGridProperty(L"Name", L"", nullptr, AsData(&mName));
		pgGroup->AddSubItem(pgProp);
	}

	ReflectChange(Component::PROP_NAME);

	view.ExpandAll();
}

void PHModel::ReflectChange(Entity::PropID prop)
{
	if (prop == Component::PROP_NAME)
	{
		PropGridCtrl& view = thePropManager->GetView();
		PropGridProperty* pgProp = view.FindItemByData(AsData(&mName));
		mName.FromModel(pgProp);
	}
}

// Path ---------------------------------------------------------------------------------------------------------------
void PHModel::Path::FromModel(PropGridProperty* pgProp)
{
	pgProp->Enable(FALSE);

	const ModelSet& models = thePropManager->GetTargets<ModelSet>();

	if (models.size() == 1)
	{
		pgProp->SetValue((*models.begin())->GetFilePath().c_str());
	}
	else
	{
		pgProp->SetValue(L"");
	}
}

void PHModel::Path::ToModel(PropGridProperty*)
{
	// read-only
}
// Path ---------------------------------------------------------------------------------------------------------------

// Name ---------------------------------------------------------------------------------------------------------------
void PHModel::Name::FromModel(PropGridProperty* pgProp)
{
	pgProp->Enable(TRUE);

	const BlockSet& blocks = thePropManager->GetTargets<BlockSet>();
	BlockSet::iterator iter = blocks.begin();
	const auto& pivotName = (*iter)->GetName();

	bool sameAll = true;
	while (++iter != blocks.end())
	{
		const auto& name = (*iter)->GetName();
		if (name != pivotName)
		{
			sameAll = false;
			break;
		}
	}
	if (sameAll)
	{
		pgProp->SetValue(pivotName.c_str());
	}
	else
	{
		pgProp->SetValue(L"");
	}
}

void PHModel::Name::ToModel(PropGridProperty* pgProp)
{
	const ComponentSet& components = thePropManager->GetTargets<ComponentSet>();

	wstring value(pgProp->GetValue().bstrVal);
	for (auto& component : components)
	{
		component->SetName(value);
	}
}
// Name ---------------------------------------------------------------------------------------------------------------

} // namespace hsui
