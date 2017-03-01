#pragma once

#include <xscCommand.h>
#include <xscBlock.h>
#include <hsmoModule.h>

namespace hsmo {

class HSMO_API SetBlockLocationCommand : public xsc::Command
{
public:
	SetBlockLocationCommand(xsc::BlockSet&& blocks, xsc::MPVector::Dimension dim, int value);
	virtual ~SetBlockLocationCommand();

protected:
	virtual bool Exec() override;

protected:
	xsc::BlockSet mBlocks;
	xsc::MPVector::Dimension mDim;
	int mValue;
};

} // namespace hsmo
