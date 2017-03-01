#pragma once

#include <hsmoVariableBlock.h>
#include <hsprBlockPresenter.h>

namespace hspr {

class BPVariableBlock : public BlockPresenter
{
public:
	BPVariableBlock(xsc::Block* block, ILevelView* levelView);
	virtual ~BPVariableBlock();

	// ================================================================================================================
	private: friend Module;
	static void InitBPVariableBlock();
	static void TermBPVariableBlock();
	// ================================================================================================================
};

} // namespace hspr
