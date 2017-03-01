#include "hsmoPrecompiled.h"
#include "hsmoRemoveCommand.h"
#include "hsmoModuleBlock.h"

using namespace std;
using namespace xsc;

namespace hsmo {

RemoveCommand::RemoveCommand(Level* level, ComponentSet&& components)
{
	mLevel = level;
	mComponents = std::move(components);

	mSuperLevel = nullptr;
}

RemoveCommand::~RemoveCommand()
{
}

bool RemoveCommand::Exec()
{
	if (mLevel->IsDerivedFrom(XSC_RTTI(ModuleLevel)))
	{
		mSuperLevel = static_cast<Level*>(mLevel->GetParent()->GetLevel());
	}

	mUndoStates.reserve(1 + (mSuperLevel ? 1 : 0));
	if (mSuperLevel)
	{
		mUndoStates.push_back(mSuperLevel->SaveState());
	}
	mUndoStates.push_back(mLevel->SaveState());

	mLevel->RemoveComponents(mComponents);

	mRedoStates.reserve(1 + (mSuperLevel ? 1 : 0));
	if (mSuperLevel)
	{
		mRedoStates.push_back(mSuperLevel->SaveState());
	}
	mRedoStates.push_back(mLevel->SaveState());

	mComponents = std::move(ComponentSet());
	return true;
}

} // namespace hsmo
