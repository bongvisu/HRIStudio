#pragma once

#include <xscCommand.h>
#include <xscLevel.h>
#include <hsmoModule.h>

namespace hsmo {

class HSMO_API RemoveCommand : public xsc::Command
{
public:
	RemoveCommand(xsc::Level* level, xsc::ComponentSet&& components);
	virtual ~RemoveCommand();

protected:
	virtual bool Exec() override;

protected:
	xsc::Level* mLevel;
	xsc::Level* mSuperLevel;
	xsc::ComponentSet mComponents;

private:
	friend class CutCommand;
};

} // namespace hsmo
