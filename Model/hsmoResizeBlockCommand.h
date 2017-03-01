#pragma once

#include <xscCommand.h>
#include <xscLevel.h>
#include <hsmoModule.h>

namespace hsmo {

class HSMO_API ResizeBlockCommand : public xsc::Command
{
public:
	ResizeBlockCommand(xsc::Level* level, xsc::Block* block, const xsc::MPRectangle& geometry);
	virtual ~ResizeBlockCommand();

protected:
	virtual bool Exec() override;

protected:
	xsc::Level* mLevel;
	xsc::BlockSet mBlock;
	xsc::MPRectangle mGeometry;
};

} // namespace hsmo
