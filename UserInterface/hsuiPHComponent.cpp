#include "hsuiPrecompiled.h"
#include "hsuiPHComponent.h"
#include "hsuiPropManager.h"

using namespace std;
using namespace xsc;
using namespace hsmo;

namespace hsui {

const wchar_t* PHComponent::GetDisplayType() const
{
	return L"Component";
}

void PHComponent::Setup()
{
	PHObject::Setup();

	PropGridProperty* pgProp;
	PropGridProperty* pgGroup;
	PropGridCtrl& view = thePropManager->GetView();
	
	pgGroup = new PropGridProperty(L"Identity", IDENTITY_KEY);
	{
		// ------------------------------------------------------------------------------------------------------------
		pgProp = new PropGridProperty(L"Type", GetDisplayType());
		pgProp->Enable(FALSE);
		pgGroup->AddSubItem(pgProp);
		// ------------------------------------------------------------------------------------------------------------

		// ------------------------------------------------------------------------------------------------------------
		CString id;
		const auto& components = thePropManager->GetTargets<ComponentSet>();
		if (components.size() == 1)
		{
			id.Format(L"%llu", (*components.begin())->GetID());
		}
		pgProp = new PropGridProperty(L"ID", id);
		pgProp->Enable(FALSE);
		pgGroup->AddSubItem(pgProp);
		// ------------------------------------------------------------------------------------------------------------
	}
	view.AddProperty(pgGroup);
}

} // namespace hsui
