#pragma once

#include <xscCommand.h>
#include <xscLevel.h>
#include <hsmoModule.h>

namespace hsmo {

class HSMO_API RotateBlockCommand : public xsc::Command
{
public:
	RotateBlockCommand(xsc::Level* level, xsc::BlockSet&& blocks);
	virtual ~RotateBlockCommand();

protected:
	virtual bool Exec() override;

protected:
	xsc::Level* mLevel;
	xsc::BlockSet mBlocks;
};

} // namespace hsmo
