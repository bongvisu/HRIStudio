#include "hsmoPrecompiled.h"
#include "hsmoAddBlockCommand.h"
#include "hsmoModel.h"
#include "hsmoModuleBlock.h"
#include "hsmoLogMediator.h"

using namespace std;
using namespace xsc;

namespace hsmo {

AddBlockCommand::AddBlockCommand(Level* level, const wchar_t* rttiName, MPVector center)
	:
	mLevel(level),
	mRttiName(rttiName),
	mCenter(center)
{
	mSuperLevel = nullptr;
}

AddBlockCommand::~AddBlockCommand()
{
}

bool AddBlockCommand::Exec()
{
	if (mRttiName == XSC_RTTI(ModuleInBlock).GetName() ||
		mRttiName == XSC_RTTI(ModuleOutBlock).GetName())
	{
		if (mLevel->IsDerivedFrom(XSC_RTTI(Model)))
		{
			LogItem logItem;
			logItem.senderName = mLevel->GetName();
			logItem.senderID = mLevel->GetID();
			logItem.message = L"Module I/O blocks are not allowed in the model level.";
			theLogMediator->TransferLogItem(std::move(logItem));
			return false;
		}

		assert(mLevel->IsDerivedFrom(XSC_RTTI(ModuleLevel)));
		Component* parent = mLevel->GetParent();
		assert(parent->IsDerivedFrom(XSC_RTTI(ModuleBlock)));
		mSuperLevel = static_cast<Level*>(parent->GetLevel());
	}

	mUndoStates.reserve(1 + (mSuperLevel ? 1 : 0));
	if (mSuperLevel)
	{
		mUndoStates.push_back(mSuperLevel->SaveState());
	}
	mUndoStates.push_back(mLevel->SaveState());

	mLevel->AddBlock(mRttiName, mCenter);

	mRedoStates.reserve(1 + (mSuperLevel ? 1 : 0));
	if (mSuperLevel)
	{
		mRedoStates.push_back(mSuperLevel->SaveState());
	}
	mRedoStates.push_back(mLevel->SaveState());

	return true;
}

} // namespace hsmo
