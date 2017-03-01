#pragma once

#include <xscMemento.h>

namespace xsc {

class Command;
typedef Pointer<Command> CommandPtr;

class XSC_API Command : public Object
{
public:
	virtual ~Command();

	virtual bool Exec() = 0;

	void Undo();
	void Redo();

protected:
	Command();

protected:
	MementoPtrVec mUndoStates;
	MementoPtrVec mRedoStates;
};

} // namespace xsc
