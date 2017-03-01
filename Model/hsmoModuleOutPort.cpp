#include "hsmoPrecompiled.h"
#include "hsmoModuleOutPort.h"
#include "hsmoModuleOutBlock.h"

using namespace std;
using namespace xsc;

namespace hsmo {

// ====================================================================================================================
void ModuleOutPort::InitModuleOutPort()
{
	XSC_REGISTER_FACTORY(hsmo, ModuleOutPort)
}

void ModuleOutPort::TermModuleOutPort()
{
}
// ====================================================================================================================

XSC_IMPLEMENT_DYNCREATE(hsmo, ModuleOutPort, OutputPort)

ModuleOutPort::ModuleOutPort()
	:
	mCompanionBlock(nullptr)
{
}

ModuleOutPort::~ModuleOutPort()
{
}

void ModuleOutPort::SetComapionBlock(ModuleIOBlock* companionBlock)
{
	mCompanionBlock = companionBlock;
}

ModuleIOBlock* ModuleOutPort::GetCompanionBlock() const
{
	return mCompanionBlock;
}

// Cloning ------------------------------------------------------------------------------------------------------------
Component* ModuleOutPort::Clone(ComponentToComponent& remapper)
{
	auto cloned = static_cast<ModuleOutPort*>(OutputPort::Clone(remapper));
	
	cloned->mCompanionBlock = mCompanionBlock;
	
	return cloned;
}

void ModuleOutPort::Remap(const ComponentToComponent& remapper)
{
	OutputPort::Remap(remapper);

	ComponentToComponent::const_iterator iter;
	iter = remapper.find(mCompanionBlock);
	assert(iter != remapper.end());
	assert(iter->second->IsDerivedFrom(XSC_RTTI(ModuleOutBlock)));
	mCompanionBlock = static_cast<ModuleIOBlock*>(iter->second);
}
// --------------------------------------------------------------------------------------------------------------------

// Stream -------------------------------------------------------------------------------------------------------------
bool ModuleOutPort::Serialize(Stream& stream)
{
	if (false == OutputPort::Serialize(stream))
	{
		return false;
	}

	assert(mCompanionBlock);
	stream.BeginOutputSection(L"CompanionBlock", true, true);
	stream.SetSimpleValaue(mCompanionBlock->FormatID());
	stream.EndOutputSection(false, false);

	return true;
}

bool ModuleOutPort::Deserialize(Stream& stream)
{
	if (false == OutputPort::Deserialize(stream))
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
		return false;
	}
	stream.EndInputSection();

	return true;
}

bool ModuleOutPort::DoPostloadProcessing(Stream& stream)
{
	if (false == OutputPort::DoPostloadProcessing(stream))
	{
		return false;
	}
	
	auto* component = stream.FromID(reinterpret_cast<ID>(mCompanionBlock));
	if (nullptr == component)
	{
		return false;
	}
	if (false == component->IsDerivedFrom(XSC_RTTI(ModuleOutBlock)))
	{
		return false;
	}
	mCompanionBlock = static_cast<ModuleIOBlock*>(component);

	return true;
}
// --------------------------------------------------------------------------------------------------------------------

} // namespace hsmo
