#include "hsmoPrecompiled.h"
#include "hsmoSetBlockDimensionCommand.h"
#include <xscLevel.h>

using namespace std;
using namespace xsc;

namespace hsmo {

SetBlockDimensionCommand::SetBlockDimensionCommand(
	BlockSet&& blocks,
	MPVector::Dimension dim,
	int value)
{
	mBlocks = std::move(blocks);
	mDim = dim;
	mValue = value;
}

SetBlockDimensionCommand::~SetBlockDimensionCommand()
{
}

bool SetBlockDimensionCommand::Exec()
{
	LevelToBlockSet levelToBlocks;
	LevelToBlockSet::iterator iter;
	for (auto block : mBlocks)
	{
		Level* level = static_cast<Level*>(block->GetLevel());
		iter = levelToBlocks.find(level);
		if (iter == levelToBlocks.end())
		{
			BlockSet blocks;
			blocks.insert(block);
			levelToBlocks[level] = std::move(blocks);
		}
		else
		{
			iter->second.insert(block);
		}
	}

	mUndoStates.reserve(levelToBlocks.size());
	for (auto& item : levelToBlocks)
	{
		mUndoStates.push_back(item.first->SaveState());
	}

	MPVector::Axis dim;
	int width = 0;
	int height = 0;
	if (mDim == MPVector::Dimension::X)
	{
		dim = MPVector::AXIS_X;
		width = mValue;
	}
	else
	{
		dim = MPVector::AXIS_Y;
		height = mValue;
	}

	for (auto& item : levelToBlocks)
	{
		item.first->ModifyBlockDimension(item.second, dim, width, height);
	}

	mRedoStates.reserve(levelToBlocks.size());
	for (auto& item : levelToBlocks)
	{
		mRedoStates.push_back(item.first->SaveState());
	}

	mBlocks = std::move(BlockSet());
	return true;
}

} // namespace hsmo
