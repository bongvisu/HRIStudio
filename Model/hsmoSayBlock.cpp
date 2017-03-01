#include "hsmoPrecompiled.h"
#include "hsmoSayBlock.h"

using namespace std;
using namespace xsc;

namespace hsmo {

XSC_IMPLEMENT_DYNCREATE(hsmo, SayBlock, HriBlock)

// ====================================================================================================================
void SayBlock::InitSayBlock()
{
	XSC_REGISTER_FACTORY(hsmo, SayBlock)
}

void SayBlock::TermSayBlock()
{
}
// ====================================================================================================================

SayBlock::SayBlock()
{
	mName = L"Say";
}

SayBlock::~SayBlock()
{
}

void SayBlock::OnConstruct(const MPVector& center)
{
	mGeometry.l = center.x - 27;
	mGeometry.t = center.y - 27;
	mGeometry.r = mGeometry.l + 55;
	mGeometry.b = mGeometry.t + 55;

	AddInputPort(MPRectangle::EDGE_L);
	AddOutputPort(MPRectangle::EDGE_R);
}

void SayBlock::SetCodeText(const wstring& text)
{
	mCodeText = text;
}

const wstring& SayBlock::GetCodeText() const
{
	return mCodeText;
}

// Code Generation ----------------------------------------------------------------------------------------------------
wstring SayBlock::GetEntryCode() const
{
	swprintf_s(msGenericTextBuf, L"{topic=SAY_%llu}{@init}", mID);
	return msGenericTextBuf;
}

bool SayBlock::GenerateCode(wstring& code, int level)
{
	code += L'\n';

	// topic start
	swprintf_s(msGenericTextBuf, L"> topic SAY_%llu", mID);
	code += msGenericTextBuf;
	code += L"\n";

	// init line
	code += L"\t+ init";
	code += L"\n";

	// response
	code += L"\t- ";
	auto linearizedCode(mCodeText);
	std::replace(linearizedCode.begin(), linearizedCode.end(), L'\n', L' ');
	code += linearizedCode;

	// transition
	code += L"\n\t^";
	auto& joints = mOutputPorts[0]->GetConnectionJoints();
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
	if (joints.size() == 0)
	{
		code += L" {topic=random}";
	}

	// topic end
	code += L"\n";
	code += L"< topic";

	code += L'\n';
	return true;
}
// --------------------------------------------------------------------------------------------------------------------

// Cloning ------------------------------------------------------------------------------------------------------------
Component* SayBlock::Clone(ComponentToComponent& remapper)
{
	auto cloned = static_cast<SayBlock*>(HriBlock::Clone(remapper));

	cloned->mCodeText = mCodeText;

	return cloned;
}
// --------------------------------------------------------------------------------------------------------------------

// Stream -------------------------------------------------------------------------------------------------------------
bool SayBlock::Serialize(Stream& stream)
{
	if (false == HriBlock::Serialize(stream))
	{
		return false;
	}

	stream.BeginOutputSection(L"CodeText", true, true);
	stream.SetCDataValue(mCodeText.c_str());
	stream.EndOutputSection(false, false);

	return true;
}

bool SayBlock::Deserialize(Stream& stream)
{
	if (false == HriBlock::Deserialize(stream))
	{
		return false;
	}

	wstring value;
	int numElements;

	numElements = stream.BeginInputSection(L"CodeText");
	if (numElements)
	{
		stream.ActivateItem(0);
		stream.GetTextValue(mCodeText);
		stream.DeactivateItem();
	}
	stream.EndInputSection();

	return true;
}
// --------------------------------------------------------------------------------------------------------------------

} // namespace hsmo
