#pragma once

#include <xscCommand.h>
#include <xscLevel.h>
#include <hsmoModule.h>

namespace hsmo {

class HSMO_API AddLinkCommand : public xsc::Command
{
public:
	AddLinkCommand(xsc::Level* level, xsc::OutputPort* source, xsc::InputPort* target);
	virtual ~AddLinkCommand();

protected:
	virtual bool Exec() override;

protected:
	xsc::Level* mLevel;
	xsc::OutputPort* mSource;
	xsc::InputPort* mTarget;
};

} // namespace hsmo
