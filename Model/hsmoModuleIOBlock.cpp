#include "hsmoPrecompiled.h"
#include "hsmoModuleIOBlock.h"
#include "hsmoIModuleBlock.h"
#include "hsmoIModuleIOPort.h"

using namespace std;
using namespace xsc;

namespace hsmo {

XSC_IMPLEMENT_DYNAMIC(hsmo, ModuleIOBlock, HriBlock)

ModuleIOBlock::ModuleIOBlock()
	:
	mModuleBlock(nullptr),
	mCompanionPort(nullptr)
{
}

ModuleIOBlock::~ModuleIOBlock()
{
}

void ModuleIOBlock::SetCompanionPort(Port* port)
{
	mCompanionPort = port;
}

Port* ModuleIOBlock::GetCompanionPort() const
{
	return mCompanionPort;
}

int ModuleIOBlock::GetIndex() const
{
	if (mModuleBlock == nullptr)
	{
		mModuleBlock = dynamic_cast<IModuleBlock*>(mCompanionPort->GetParent());
	}
	return mModuleBlock->GetIndexFor(mCompanionPort);
}

// Cloning ------------------------------------------------------------------------------------------------------------
Component* ModuleIOBlock::Clone(ComponentToComponent& remapper)
{
	auto cloned = static_cast<ModuleIOBlock*>(Block::Clone(remapper));

	cloned->mCompanionPort = mCompanionPort;

	return cloned;
}

void ModuleIOBlock::Remap(const ComponentToComponent& remapper)
{
	Block::Remap(remapper);

	ComponentToComponent::const_iterator iter;
	iter = remapper.find(mCompanionPort);
	assert(iter != remapper.end());
	assert(iter->second->IsDerivedFrom(XSC_RTTI(Port)));
	mCompanionPort = static_cast<Port*>(iter->second);
}
// --------------------------------------------------------------------------------------------------------------------

// Stream -------------------------------------------------------------------------------------------------------------
bool ModuleIOBlock::Serialize(Stream& stream)
{
	if (false == Block::Serialize(stream))
	{
		return false;
	}

	assert(mCompanionPort);
	stream.BeginOutputSection(L"CompanionPort", true, true);
	stream.SetSimpleValaue(mCompanionPort->FormatID());
	stream.EndOutputSection(false, false);

	return true;
}

bool ModuleIOBlock::Deserialize(Stream& stream)
{
	if (false == Block::Deserialize(stream))
	{
		return false;
	}

	int numElements = stream.BeginInputSection(L"CompanionPort");
	if (numElements)
	{
		stream.ActivateItem(0);

		wstring value;
		stream.GetTextValue(value);
		mCompanionPort = reinterpret_cast<Port*>(std::stoull(value));

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

bool ModuleIOBlock::DoPostloadProcessing(Stream& stream)
{
	if (false == Block::DoPostloadProcessing(stream))
	{
		return false;
	}
	
	Component* component = stream.FromID(reinterpret_cast<ID>(mCompanionPort));
	if (nullptr == component)
	{
		return false;
	}
	if (false == component->IsDerivedFrom(XSC_RTTI(Port)))
	{
		return false;
	}
	Port* port = static_cast<Port*>(component);
	if (nullptr == dynamic_cast<IModuleIOPort*>(port))
	{
		return false;
	}
	if (GetCompatiblePortType() != port->GetPortType())
	{
		return false;
	}
	mCompanionPort = port;

	return true;
}
// --------------------------------------------------------------------------------------------------------------------

} // namespace hsmo
