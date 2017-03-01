#include "hsmoPrecompiled.h"
#include "hsmoModuleLevel.h"
#include "hsmoIModuleBlock.h"

using namespace std;
using namespace xsc;

namespace hsmo {

// ====================================================================================================================
void ModuleLevel::InitModuleLevel()
{
	XSC_REGISTER_FACTORY(hsmo, ModuleLevel)
}

void ModuleLevel::TermModuleLevel()
{
}
// ====================================================================================================================

XSC_IMPLEMENT_DYNCREATE(hsmo, ModuleLevel, Level)

ModuleLevel::ModuleLevel()
{
}

ModuleLevel::~ModuleLevel()
{
}

void ModuleLevel::GetHierarchicalPath(wstring& path) const
{
	Level::GetHierarchicalPath(path);
	path += L"/" + mName;
}

void ModuleLevel::AddBlock(const wstring& rttiName, const MPVector& center)
{
	if (rttiName == XSC_RTTI(ModuleInBlock).GetName() ||
		rttiName == XSC_RTTI(ModuleOutBlock).GetName())
	{
		Block* block = NewBlock(rttiName);
		block->Construct(center);

		ModuleIOBlockSet ioBlocks;
		ioBlocks.insert(static_cast<ModuleIOBlock*>(block));
		IModuleBlock* moduleBlock = dynamic_cast<IModuleBlock*>(mParent);
		moduleBlock->HandleIOBlockAddition(ioBlocks);

		ComponentSet newComponents;
		newComponents.insert(block);
		NotifyChildAddition(newComponents);
	
		NotifyNodalGeometryChange();
	}
	else
	{
		Level::AddBlock(rttiName, center);
	}
}

void ModuleLevel::RemoveComponents(const ComponentSet& components)
{
	ModuleIOBlockSet ioBlocks;
	for (auto component : components)
	{
		if (component->IsDerivedFrom(XSC_RTTI(ModuleIOBlock)))
		{
			ioBlocks.insert(static_cast<ModuleIOBlock*>(component));
		}
	}

	if (ioBlocks.size())
	{
		IModuleBlock* moduleBlock = dynamic_cast<IModuleBlock*>(mParent);
		moduleBlock->HandleIOBlockRemoval(ioBlocks);
	}

	Level::RemoveComponents(components);
}

} // namespace hsmo
