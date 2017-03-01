#pragma once

#include <xscCommand.h>
#include <xscLevel.h>
#include <hsmoModule.h>

namespace hsmo {

class HSMO_API CutCommand : public xsc::Command
{
public:
	CutCommand(xsc::Level* level, xsc::ComponentSet&& components);
	virtual ~CutCommand();

protected:
	virtual bool Exec() override;

protected:
	xsc::Level* mLevel;
	xsc::ComponentSet mComponents;
};

} // namespace hsmo
