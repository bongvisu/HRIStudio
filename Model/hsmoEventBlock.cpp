#include "hsmoPrecompiled.h"
#include "hsmoEventBlock.h"

using namespace std;
using namespace xsc;

namespace hsmo {

XSC_IMPLEMENT_DYNCREATE(hsmo, EventBlock, HriBlock)

// ====================================================================================================================
void EventBlock::InitEventBlock()
{
	XSC_REGISTER_FACTORY(hsmo, EventBlock)
}

void EventBlock::TermEventBlock()
{
}
// ====================================================================================================================

EventBlock::EventBlock()
{
	mName = L"Event";
	mEvent = FACE_DETECTED;
}

EventBlock::~EventBlock()
{
}

void EventBlock::OnConstruct(const MPVector& center)
{
	mGeometry.l = center.x - 27;
	mGeometry.t = center.y - 27;
	mGeometry.r = mGeometry.l + 55;
	mGeometry.b = mGeometry.t + 55;

	AddOutputPort(MPRectangle::EDGE_R);
}

void EventBlock::SetHriEvent(HriEvent hriEvent)
{
	mEvent = hriEvent;
}

HriEvent EventBlock::GetHriEvent() const
{
	return mEvent;
}

// Code Generation ----------------------------------------------------------------------------------------------------
wstring EventBlock::GetEntryCode() const
{
	// this should never happen
	assert(false);
	return L"";
}

bool EventBlock::GenerateCode(wstring& code, int level)
{
	code += L'\n';

	wstring indentation;
	for (int i = 0; i < level; i++)
	{
		indentation += L'\t';
	}

	// trigger line
	code += indentation;
	code += L"+ ";
	code += HRI_EVENT_CODES[mEvent];
	code += L"\n";

	// response lines
	code += indentation;
	code += L"-";

	auto& joints = mOutputPorts[0]->GetConnectionJoints();
	if (joints.size())
	{
		for (auto joint : joints)
		{
			auto parent = joint->GetParent();
			assert(parent->IsDerivedFrom(XSC_RTTI(Link)));
			auto link = static_cast<Link*>(parent);
			auto dest = link->GetDestinationPort();
			assert(dest);

			auto destBlock = static_cast<const HriBlock*>(dest->GetParent());
			code += L" ";
			code += destBlock->GetEntryCode();
		}
	}
	else
	{
		code += L" {topic=random}";
	}

	code += L"\n";
	return true;
}
// --------------------------------------------------------------------------------------------------------------------

// Cloning ------------------------------------------------------------------------------------------------------------
Component* EventBlock::Clone(ComponentToComponent& remapper)
{
	auto cloned = static_cast<EventBlock*>(HriBlock::Clone(remapper));

	cloned->mEvent = mEvent;

	return cloned;
}
// --------------------------------------------------------------------------------------------------------------------

// Stream -------------------------------------------------------------------------------------------------------------
bool EventBlock::Serialize(Stream& stream)
{
	if (false == HriBlock::Serialize(stream))
	{
		return false;
	}

	stream.BeginOutputSection(L"HriEvent", true, true);
	swprintf_s(msGenericTextBuf, L"%u", mEvent);
	stream.SetSimpleValaue(msGenericTextBuf);
	stream.EndOutputSection(false, false);

	return true;
}

bool EventBlock::Deserialize(Stream& stream)
{
	if (false == HriBlock::Deserialize(stream))
	{
		return false;
	}

	wstring value;
	int numElements;
	bool failed = false;

	numElements = stream.BeginInputSection(L"HriEvent");
	if (numElements)
	{
		stream.ActivateItem(0);
		stream.GetTextValue(value);
		mEvent = static_cast<HriEvent>(std::stoul(value));
		stream.DeactivateItem();
	}
	else
	{
		assert(false);
		failed = true;
	}
	stream.EndInputSection();
	if (failed)
	{
		return false;
	}

	return true;
}
// --------------------------------------------------------------------------------------------------------------------

} // namespace hsmo
