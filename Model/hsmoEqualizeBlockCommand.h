#pragma once

#include <xscCommand.h>
#include <xscLevel.h>
#include <hsmoModule.h>

namespace hsmo {

class HSMO_API EqualizeBlockCommand : public xsc::Command
{
public:
	EqualizeBlockCommand(xsc::Level* level, xsc::BlockSet&& blocks, const xsc::Block* pivot, xsc::MPVector::Axis dim);
	virtual ~EqualizeBlockCommand();

protected:
	virtual bool Exec() override;

protected:
	xsc::Level* mLevel;
	xsc::BlockSet mBlocks;
	const xsc::Block* mPivot;
	xsc::MPVector::Axis mDim;
};

} // namespace hsmo
