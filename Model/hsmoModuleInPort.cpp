#include "hsmoPrecompiled.h"
#include "hsmoModuleInPort.h"
#include "hsmoModuleInBlock.h"

using namespace std;
using namespace xsc;

namespace hsmo {

// ====================================================================================================================
void ModuleInPort::InitModuleInPort()
{
	XSC_REGISTER_FACTORY(hsmo, ModuleInPort)
}

void ModuleInPort::TermModuleInPort()
{
}
// ====================================================================================================================

XSC_IMPLEMENT_DYNCREATE(hsmo, ModuleInPort, InputPort)

ModuleInPort::ModuleInPort()
	:
	mCompanionBlock(nullptr)
{
}

ModuleInPort::~ModuleInPort()
{
}

void ModuleInPort::SetComapionBlock(ModuleIOBlock* companionBlock)
{
	mCompanionBlock = companionBlock;
}

ModuleIOBlock* ModuleInPort::GetCompanionBlock() const
{
	return mCompanionBlock;
}

// Cloning ------------------------------------------------------------------------------------------------------------
Component* ModuleInPort::Clone(ComponentToComponent& remapper)
{
	auto cloned = static_cast<ModuleInPort*>(InputPort::Clone(remapper));
	
	cloned->mCompanionBlock = mCompanionBlock;
	
	return cloned;
}

void ModuleInPort::Remap(const ComponentToComponent& remapper)
{
	InputPort::Remap(remapper);

	ComponentToComponent::const_iterator iter;
	iter = remapper.find(mCompanionBlock);
	assert(iter != remapper.end());
	assert(iter->second->IsDerivedFrom(XSC_RTTI(ModuleInBlock)));
	mCompanionBlock = static_cast<ModuleIOBlock*>(iter->second);
}
// --------------------------------------------------------------------------------------------------------------------

// Stream -------------------------------------------------------------------------------------------------------------
bool ModuleInPort::Serialize(Stream& stream)
{
	if (false == InputPort::Serialize(stream))
	{
		return false;
	}

	assert(mCompanionBlock);
	stream.BeginOutputSection(L"CompanionBlock", true, true);
	stream.SetSimpleValaue(mCompanionBlock->FormatID());
	stream.EndOutputSection(false, false);

	return true;
}

bool ModuleInPort::Deserialize(Stream& stream)
{
	if (false == InputPort::Deserialize(stream))
	{
		return false;
	}

	int numElements = stream.BeginInputSection(L"CompanionBlock");
	if (numElements)
	{
		stream.ActivateItem(0);

		wstring value;
		stream.GetTextValue(value);
		mCompanionBlock = reinterpret_cast<ModuleIOBlock*>(std::stoull(value));

		stream.DeactivateItem();
	}
	else
	{
		stream.EndInputSection();
		return false; // mandatory
	}
	stream.EndInputSection();

	return true;
}

bool ModuleInPort::DoPostloadProcessing(Stream& stream)
{
	if (false == InputPort::DoPostloadProcessing(stream))
	{
		return false;
	}
	
	auto component = stream.FromID(reinterpret_cast<ID>(mCompanionBlock));
	if (nullptr == component)
	{
		return false;
	}
	if (false == component->IsDerivedFrom(XSC_RTTI(ModuleInBlock)))
	{
		return false;
	}
	mCompanionBlock = static_cast<ModuleIOBlock*>(component);

	return true;
}
// --------------------------------------------------------------------------------------------------------------------

} // namespace hsmo
