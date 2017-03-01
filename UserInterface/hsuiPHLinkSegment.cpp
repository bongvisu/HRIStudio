#include "hsuiPrecompiled.h"
#include "hsuiPHLinkSegment.h"
#include "hsuiPropManager.h"

using namespace std;
using namespace xsc;
using namespace hsmo;

namespace hsui {

const wchar_t* PHLinkSegment::GetDisplayType() const
{
	return L"Segment";
}

void PHLinkSegment::Setup()
{
	PHComponent::Setup();

	PropGridProperty* pgProp;
	PropGridProperty* pgGroup;
	PropGridCtrl& view = thePropManager->GetView();

	pgGroup = new PropGridProperty(L"Geometry");
	{
		pgProp = new PropGridProperty(L"Src. Location", L"", nullptr, AsData(&mSourceLocation));
		pgGroup->AddSubItem(pgProp);

		pgProp = new PropGridProperty(L"Dst. Location", L"", nullptr, AsData(&mDestinationLocation));
		pgGroup->AddSubItem(pgProp);
	}
	view.AddProperty(pgGroup);

	ReflectChange(LinkSegment::PROP_SOURCE_LOCATION);
	ReflectChange(LinkSegment::PROP_DESTINATION_LOCATION);
}

void PHLinkSegment::ReflectChange(Entity::PropID prop)
{
	if (prop == LinkSegment::PROP_SOURCE_LOCATION)
	{
		PropGridCtrl& view = thePropManager->GetView();
		PropGridProperty* pgProp;

		pgProp = view.FindItemByData(AsData(&mSourceLocation));
		mSourceLocation.FromModel(pgProp);
	}
	else if (prop == LinkSegment::PROP_DESTINATION_LOCATION)
	{
		PropGridCtrl& view = thePropManager->GetView();
		PropGridProperty* pgProp;

		pgProp = view.FindItemByData(AsData(&mDestinationLocation));
		mDestinationLocation.FromModel(pgProp);
	}
	else
	{
		PHComponent::ReflectChange(prop);
	}
}

// SourceLocation -----------------------------------------------------------------------------------------------------
void PHLinkSegment::SourceLocation::FromModel(PropGridProperty* pgProp)
{
	pgProp->Enable(FALSE);

	const LinkSegmentSet& segments = thePropManager->GetTargets<LinkSegmentSet>();
	LinkSegmentSet::iterator iter = segments.begin();
	const MPVector& pivotLocation = (*iter)->GetSourceLocation();

	bool sameAll = true;
	while (++iter != segments.end())
	{
		const MPVector& otherLocation = (*iter)->GetSourceLocation();
		if (otherLocation != pivotLocation)
		{
			sameAll = false;
			break;
		}
	}
	if (sameAll)
	{
		CString value;
		value.Format(L"%d, %d", pivotLocation.x, pivotLocation.y);
		pgProp->SetValue(value);
	}
	else
	{
		pgProp->SetValue(L"");
	}
}

void PHLinkSegment::SourceLocation::ToModel(PropGridProperty*)
{
	// Read-only
}
// SourceLocation -----------------------------------------------------------------------------------------------------

// DestinationLocation ------------------------------------------------------------------------------------------------
void PHLinkSegment::DestinationLocation::FromModel(PropGridProperty* pgProp)
{
	pgProp->Enable(FALSE);

	const LinkSegmentSet& segments = thePropManager->GetTargets<LinkSegmentSet>();
	LinkSegmentSet::iterator iter = segments.begin();
	const MPVector& pivotLocation = (*iter)->GetDestinationLocation();

	bool sameAll = true;
	while (++iter != segments.end())
	{
		const MPVector& otherLocation = (*iter)->GetDestinationLocation();
		if (otherLocation != pivotLocation)
		{
			sameAll = false;
			break;
		}
	}
	if (sameAll)
	{
		CString value;
		value.Format(L"%d, %d", pivotLocation.x, pivotLocation.y);
		pgProp->SetValue(value);
	}
	else
	{
		pgProp->SetValue(L"");
	}
}

void PHLinkSegment::DestinationLocation::ToModel(PropGridProperty*)
{
	// Read-only
}
// DestinationLocation ------------------------------------------------------------------------------------------------

} // namespace hsui
