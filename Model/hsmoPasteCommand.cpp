#include "hsmoPrecompiled.h"
#include "hsmoPasteCommand.h"
#include "hsmoModelManager.h"

using namespace std;
using namespace xsc;

namespace hsmo {

PasteCommand::PasteCommand(Level* level)
{
	mLevel = level;
}

PasteCommand::~PasteCommand()
{
}

bool PasteCommand::Exec()
{
	if (theModelManager->mClipboard.size() == 0)
	{
		return false;
	}

	mUndoStates.push_back(mLevel->SaveState());

	ComponentSet components;
	components.reserve(theModelManager->mClipboard.size());
	ComponentToComponent remapper;
	for (auto component : theModelManager->mClipboard)
	{
		components.insert(component->Clone(remapper));
	}
	for (auto component : components)
	{
		component->Remap(remapper);
	}
	mLevel->InsertComponents(components);

	mRedoStates.push_back(mLevel->SaveState());

	return true;
}

} // namespace hsmo
