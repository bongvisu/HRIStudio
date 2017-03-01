#include "hsmoPrecompiled.h"
#include "hsmoCutCommand.h"
#include "hsmoRemoveCommand.h"
#include "hsmoCopyCommand.h"

using namespace std;
using namespace xsc;

namespace hsmo {

CutCommand::CutCommand(Level* level, ComponentSet&& components)
{
	mLevel = level;
	mComponents = std::move(components);
}

CutCommand::~CutCommand()
{
}

bool CutCommand::Exec()
{
	ComponentSet components(mComponents);
	RemoveCommand removeCommand(mLevel, std::move(components));
	removeCommand.Exec();

	mUndoStates = std::move(removeCommand.mUndoStates);
	mRedoStates = std::move(removeCommand.mRedoStates);

	CopyCommand copyCommand(mLevel, std::move(mComponents));
	copyCommand.Exec();

	return true;
}

} // namespace hsmo
