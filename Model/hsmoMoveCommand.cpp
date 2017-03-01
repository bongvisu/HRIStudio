#include "hsmoPrecompiled.h"
#include "hsmoMoveCommand.h"

using namespace std;
using namespace xsc;

namespace hsmo {

MoveCommand::MoveCommand(Level* level, ComponentSet&& components, const MPVector& delta)
{
	mLevel = level;
	mComponents = std::move(components);
	mDelta = delta;
}

MoveCommand::~MoveCommand()
{
}

bool MoveCommand::Exec()
{
	mUndoStates.push_back(mLevel->SaveState());

	mLevel->MoveComponents(mComponents, mDelta);
	
	mRedoStates.push_back(mLevel->SaveState());

	mComponents = std::move(ComponentSet());
	return true;
}

} // namespace hsmo
