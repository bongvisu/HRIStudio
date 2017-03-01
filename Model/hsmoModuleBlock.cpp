#include "hsmoPrecompiled.h"
#include "hsmoModuleBlock.h"

using namespace std;
using namespace xsc;

namespace hsmo {

// ====================================================================================================================
void ModuleBlock::InitModuleBlock()
{
	XSC_REGISTER_FACTORY(hsmo, ModuleBlock)
}

void ModuleBlock::TermModuleBlock()
{
}
// ====================================================================================================================

XSC_IMPLEMENT_DYNCREATE(hsmo, ModuleBlock, HriBlock)

ModuleBlock::ModuleBlock()
{
	mName = L"Module";
}

ModuleBlock::~ModuleBlock()
{
}

void ModuleBlock::OnConstruct(const MPVector& center)
{
	mLevel = new ModuleLevel;
	mLevel->SetParent(this);
	mLevel->SetName(mName);

	mGeometry.l = center.x - 50;
	mGeometry.t = center.y - 50;
	mGeometry.r = mGeometry.l + 101;
	mGeometry.b = mGeometry.t + 101;
}

void ModuleBlock::SetName(const wstring& name)
{
	Block::SetName(name);
	mLevel->SetName(name);
}

ModuleLevel* ModuleBlock::GetModuleLevel() const
{
	return mLevel.Get();
}

bool ModuleBlock::IsChild(const Component* component) const
{
	if (Block::IsChild(component))
	{
		return true;
	}
	if (component == mLevel.Get())
	{
		return true;
	}
	return mLevel->IsChild(component);
}

bool ModuleBlock::Contains(const Component* component) const
{
	if (Block::Contains(component))
	{
		return true;
	}
	return mLevel->Contains(component);
}

void ModuleBlock::GetChildren(ComponentVec& children, bool deep) const
{
	Block::GetChildren(children, deep);

	children.push_back(mLevel.Get());
	if (deep)
	{
		mLevel->GetChildren(children, deep);
	}
}

void ModuleBlock::GetChildren(ComponentSet& children, bool deep) const
{
	Block::GetChildren(children, deep);

	children.insert(mLevel.Get());
	if (deep)
	{
		mLevel->GetChildren(children, deep);
	}
}

int ModuleBlock::GetIndexFor(const Port* port) const
{
	if (port->GetPortType() == PORT_TYPE_INPUT)
	{
		int numPorts = static_cast<int>(mInputPorts.size());
		for (int i = 0; i < numPorts; ++i)
		{
			if (mInputPorts[i] == port)
			{
				return i;
			}
		}
	}
	else
	{
		int numPorts = static_cast<int>(mOutputPorts.size());
		for (int i = 0; i < numPorts; ++i)
		{
			if (mOutputPorts[i] == port)
			{
				return i;
			}
		}
	}

	assert(false);
	return 0;
}

void ModuleBlock::HandleIOBlockAddition(const ModuleIOBlockSet& ioBlocks)
{
	Port* port;
	ComponentSet newComponents;
	for (auto ioBlock : ioBlocks)
	{
		if (ioBlock->GetCompatiblePortType() == PORT_TYPE_INPUT)
		{
			port = AddInputPort(MPRectangle::EDGE_L, XSC_RTTI(ModuleInPort));
		}
		else
		{
			port = AddOutputPort(MPRectangle::EDGE_R, XSC_RTTI(ModuleOutPort));
		}
		newComponents.insert(port);

		IModuleIOPort* ioPort = dynamic_cast<IModuleIOPort*>(port);
		ioPort->SetComapionBlock(ioBlock);
		ioBlock->SetCompanionPort(port);
	}
	
	UpdatePortGeometry();
	UpdateConnectedLinks();

	NotifyChildAddition(newComponents);
	NotifyNodalGeometryChange();

	ILevel* level = dynamic_cast<ILevel*>(GetLevel());
	level->OnChildGeometryChange();
}

void ModuleBlock::HandleIOBlockRemoval(const ModuleIOBlockSet& ioBlocks)
{
	PortSet ports;
	ports.reserve(ioBlocks.size());
	for (auto ioBlock : ioBlocks)
	{
		ports.insert(ioBlock->GetCompanionPort());
	}

	ILevel* level = dynamic_cast<ILevel*>(GetLevel());
	level->HandlePortRemoval(ports);
	
	NotifyChildRemoval(reinterpret_cast<const ComponentSet&>(ports));
	RemovePorts(ports);

	UpdatePortGeometry();
	UpdateConnectedLinks();

	NotifyNodalGeometryChange();
	level->OnChildGeometryChange();
}

// Cloning ------------------------------------------------------------------------------------------------------------
Component* ModuleBlock::Clone(ComponentToComponent& remapper)
{
	auto cloned = static_cast<ModuleBlock*>(Block::Clone(remapper));

	cloned->mLevel = static_cast<ModuleLevel*>(mLevel->Clone(remapper));
	cloned->mLevel->SetParent(cloned);
	
	return cloned;
}

void ModuleBlock::Remap(const ComponentToComponent& remapper)
{
	Block::Remap(remapper);
	mLevel->Remap(remapper);
}
// --------------------------------------------------------------------------------------------------------------------

// Stream -------------------------------------------------------------------------------------------------------------
bool ModuleBlock::Serialize(Stream& stream)
{
	if (false == Block::Serialize(stream))
	{
		return false;
	}

	bool result;
	stream.BeginOutputSection(L"ModuleLevel", true, true);
	result = mLevel->Serialize(stream);
	stream.EndOutputSection(true, true);
	return result;
}

bool ModuleBlock::Deserialize(Stream& stream)
{
	if (false == Block::Deserialize(stream))
	{
		return false;
	}

	bool result;
	int numElements = stream.BeginInputSection(L"ModuleLevel");
	if (numElements)
	{
		mLevel = new ModuleLevel;
		mLevel->SetParent(this);

		stream.ActivateItem(0);
		result = mLevel->Deserialize(stream);
		stream.DeactivateItem();
	}
	else
	{
		result = false;
	}
	stream.EndInputSection();
	return result;
}

bool ModuleBlock::DoPostloadProcessing(Stream& stream)
{
	if (false == Block::DoPostloadProcessing(stream))
	{
		return false;
	}

	if (false == mLevel->DoPostloadProcessing(stream))
	{
		return false;
	}

	for (auto port : mInputPorts)
	{
		if (false == port->IsDerivedFrom(XSC_RTTI(ModuleInPort)))
		{
			return false;
		}
		auto inPort = static_cast<ModuleInPort*>(port);
		auto companionBlock = inPort->GetCompanionBlock();
		if (companionBlock->GetCompanionPort() != inPort)
		{
			return false;
		}
	}
	for (auto port : mOutputPorts)
	{
		if (false == port->IsDerivedFrom(XSC_RTTI(ModuleOutPort)))
		{
			return false;
		}
		auto outPort = static_cast<ModuleOutPort*>(port);
		auto companionBlock = outPort->GetCompanionBlock();
		if (companionBlock->GetCompanionPort() != outPort)
		{
			return false;
		}
	}

	return true;
}
// --------------------------------------------------------------------------------------------------------------------

} // namespace hsmo
