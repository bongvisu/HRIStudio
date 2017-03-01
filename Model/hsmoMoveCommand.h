#pragma once

#include <xscCommand.h>
#include <xscLevel.h>
#include <hsmoModule.h>

namespace hsmo {

class HSMO_API MoveCommand : public xsc::Command
{
public:
	MoveCommand(xsc::Level* level, xsc::ComponentSet&& components, const xsc::MPVector& delta);
	virtual ~MoveCommand();

protected:
	virtual bool Exec() override;

protected:
	xsc::Level* mLevel;
	xsc::ComponentSet mComponents;
	xsc::MPVector mDelta;
};

} // namespace hsmo
