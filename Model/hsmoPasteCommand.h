#pragma once

#include <xscCommand.h>
#include <xscLevel.h>
#include <hsmoModule.h>

namespace hsmo {

class HSMO_API PasteCommand : public xsc::Command
{
public:
	PasteCommand(xsc::Level* level);
	virtual ~PasteCommand();

protected:
	virtual bool Exec() override;

protected:
	xsc::Level* mLevel;
};

} // namespace hsmo
