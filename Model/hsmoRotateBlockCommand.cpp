#include "hsmoPrecompiled.h"
#include "hsmoRotateBlockCommand.h"

using namespace std;
using namespace xsc;

namespace hsmo {

RotateBlockCommand::RotateBlockCommand(Level* level, BlockSet&& blocks)
	:
	mLevel(level),
	mBlocks(std::move(blocks))
{
}

RotateBlockCommand::~RotateBlockCommand()
{
}

bool RotateBlockCommand::Exec()
{
	mUndoStates.push_back(mLevel->SaveState());

	mLevel->RotateBlocks(mBlocks);
	
	mRedoStates.push_back(mLevel->SaveState());

	mBlocks = std::move(BlockSet());
	return true;
}

} // namespace hsmo
