#include "hsmoPrecompiled.h"
#include "hsmoEqualizeBlockCommand.h"

using namespace std;
using namespace xsc;

namespace hsmo {

EqualizeBlockCommand::EqualizeBlockCommand(
	Level* level,
	BlockSet&& blocks,
	const Block* pivot,
	MPVector::Axis dim)
	:
	mLevel(level),
	mBlocks(std::move(blocks)),
	mPivot(pivot),
	mDim(dim)
{
}

EqualizeBlockCommand::~EqualizeBlockCommand()
{
}

bool EqualizeBlockCommand::Exec()
{
	mUndoStates.push_back(mLevel->SaveState());

	const auto& geometry = mPivot->GetGeometry();
	int width = geometry.GetWidth();
	int height = geometry.GetHeight();
	mLevel->ModifyBlockDimension(mBlocks, mDim, width, height);

	mRedoStates.push_back(mLevel->SaveState());

	mBlocks = std::move(BlockSet());
	return true;
}

} // namespace hsmo
