#pragma once

#include <xscCommand.h>
#include <xscLevel.h>
#include <hsmoModule.h>

namespace hsmo {

class HSMO_API AlignBlockCommand : public xsc::Command
{
public:
	AlignBlockCommand(xsc::Level* level, xsc::BlockSet&& blocks,
		const xsc::Block* pivot, xsc::MPVector::Direction direction);
	virtual ~AlignBlockCommand();

protected:
	virtual bool Exec() override;

protected:
	xsc::Level* mLevel;
	xsc::BlockSet mBlocks;
	const xsc::Block* mPivot;
	xsc::MPVector::Direction mDirection;
};

} // namespace hsmo
