#include "hsmoPrecompiled.h"
#include "hsmoSetBlockLocationCommand.h"
#include <xscLevel.h>

using namespace std;
using namespace xsc;

namespace hsmo {

SetBlockLocationCommand::SetBlockLocationCommand(
	BlockSet&& blocks,
	MPVector::Dimension dim,
	int value)
{
	mBlocks = std::move(blocks);
	mDim = dim;
	mValue = value;
}

SetBlockLocationCommand::~SetBlockLocationCommand()
{
}

bool SetBlockLocationCommand::Exec()
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

	for (auto& item : levelToBlocks)
	{
		if (mDim == MPVector::Dimension::X)
		{
			item.first->AlignBlocks(item.second, MPVector::NEGATIVE_X, mValue);
		}
		else
		{
			item.first->AlignBlocks(item.second, MPVector::POSITIVE_Y, mValue);
		}
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
