#pragma once

#include <xscCommand.h>
#include <xscLevel.h>

#include <hsmoModule.h>

namespace hsmo {

class CustomCommand;
typedef xsc::Pointer<CustomCommand> CustomCommandPtr;

class HSMO_API CustomCommand : public xsc::Command
{
public:
	CustomCommand(xsc::Level* level);
	virtual ~CustomCommand();

	void Finish();

protected:
	virtual bool Exec() override;

protected:
	xsc::Level* mLevel;
};

} // namespace hsmo
