#include "hsmoPrecompiled.h"
#include "hsmoResizeBlockCommand.h"

using namespace std;
using namespace xsc;

namespace hsmo {

ResizeBlockCommand::ResizeBlockCommand(
	Level* level,
	Block* block,
	const xsc::MPRectangle& geometry)
{
	mLevel = level;
	mBlock.insert(block);
	mGeometry = geometry;
}

ResizeBlockCommand::~ResizeBlockCommand()
{
}

bool ResizeBlockCommand::Exec()
{
	mUndoStates.push_back(mLevel->SaveState());

	mLevel->ModifyBlockGeometry(mBlock, mGeometry);

	mRedoStates.push_back(mLevel->SaveState());

	return true;
}

} // namespace hsmo
