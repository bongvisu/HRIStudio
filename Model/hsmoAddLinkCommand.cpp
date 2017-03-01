#include "hsmoPrecompiled.h"
#include "hsmoAddLinkCommand.h"

using namespace std;
using namespace xsc;

namespace hsmo {

AddLinkCommand::AddLinkCommand(Level* level, OutputPort* source, InputPort* target)
	:
	mLevel(level),
	mSource(source),
	mTarget(target)
{
}

AddLinkCommand::~AddLinkCommand()
{
}

bool AddLinkCommand::Exec()
{
	mUndoStates.push_back(mLevel->SaveState());

	mLevel->AddLink(mSource, mTarget);

	mRedoStates.push_back(mLevel->SaveState());

	return true;
}

} // namespace hsmo
