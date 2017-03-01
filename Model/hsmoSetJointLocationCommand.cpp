#include "hsmoPrecompiled.h"
#include "hsmoSetJointLocationCommand.h"
#include <xscLevel.h>

using namespace std;
using namespace xsc;

namespace hsmo {

SetJointLocationCommand::SetJointLocationCommand(
	LinkJointSet&& joints,
	MPVector::Dimension dim,
	int value)
{
	mJoints = std::move(joints);
	mDim = dim;
	mValue = value;
}

SetJointLocationCommand::~SetJointLocationCommand()
{
}

bool SetJointLocationCommand::Exec()
{
	LevelToJointSet levelToJoints;
	LevelToJointSet::iterator iter;
	for (auto joint : mJoints)
	{
		Level* level = static_cast<Level*>(joint->GetLevel());
		iter = levelToJoints.find(level);
		if (iter == levelToJoints.end())
		{
			LinkJointSet joints;
			joints.insert(joint);
			levelToJoints[level] = std::move(joints);
		}
		else
		{
			iter->second.insert(joint);
		}
	}

	mUndoStates.reserve(levelToJoints.size());
	for (auto& item : levelToJoints)
	{
		mUndoStates.push_back(item.first->SaveState());
	}

	for (auto& item : levelToJoints)
	{
		item.first->MofifyJointLocation(item.second, mDim, mValue);
	}

	mRedoStates.reserve(levelToJoints.size());
	for (auto& item : levelToJoints)
	{
		mRedoStates.push_back(item.first->SaveState());
	}

	mJoints = std::move(LinkJointSet());
	return true;
}

} // namespace hsmo
