#pragma once

#include <xscCommand.h>
#include <xscLevel.h>
#include <hsmoModule.h>

namespace hsmo {

class HSMO_API CopyCommand : public xsc::Command
{
public:
	CopyCommand(xsc::Level* level, xsc::ComponentSet&& components);
	virtual ~CopyCommand();

protected:
	virtual bool Exec() override;

protected:
	xsc::Level* mLevel;
	xsc::ComponentSet mComponents;

private:
	friend class CutCommand;
};

} // namespace hsmo
