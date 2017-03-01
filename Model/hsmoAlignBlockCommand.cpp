#include "hsmoPrecompiled.h"
#include "hsmoAlignBlockCommand.h"

using namespace std;
using namespace xsc;

namespace hsmo {

AlignBlockCommand::AlignBlockCommand(Level* level, BlockSet&& blocks,
	const Block* pivotBlock, MPVector::Direction direction)
	:
	mLevel(level),
	mBlocks(std::move(blocks)),
	mPivot(pivotBlock),
	mDirection(direction)
{
}

AlignBlockCommand::~AlignBlockCommand()
{
}

bool AlignBlockCommand::Exec()
{
	mUndoStates.push_back(mLevel->SaveState());

	int targetValue;
	auto& geometry = mPivot->GetGeometry();
	if (mDirection == MPVector::NEGATIVE_X)
	{
		targetValue = geometry.l;
	}
	else if (mDirection == MPVector::POSITIVE_X)
	{
		targetValue = geometry.r;
	}
	else if (mDirection == MPVector::POSITIVE_Y)
	{
		targetValue = geometry.t;
	}
	else
	{
		targetValue = geometry.b;
	}
	mLevel->AlignBlocks(mBlocks, mDirection, targetValue);
	
	mRedoStates.push_back(mLevel->SaveState());

	mBlocks = std::move(BlockSet());
	return true;
}

} // namespace hsmo
