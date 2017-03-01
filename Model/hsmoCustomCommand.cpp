#include "hsmoPrecompiled.h"
#include "hsmoCustomCommand.h"
#include "hsmoModel.h"

using namespace std;
using namespace xsc;

namespace hsmo {

CustomCommand::CustomCommand(Level* level)
	:
	mLevel(level)
{
	mUndoStates.push_back(mLevel->SaveState());
}

CustomCommand::~CustomCommand()
{
}

bool CustomCommand::Exec()
{
	assert(false);
	return false;
}

void CustomCommand::Finish()
{
	mRedoStates.push_back(mLevel->SaveState());

	static_cast<Model*>(mLevel->GetModel())->EnrollCustomCommand(this);
}

} // namespace hsmo
