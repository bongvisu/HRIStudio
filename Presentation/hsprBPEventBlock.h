#pragma once

#include <hsmoEventBlock.h>
#include <hsprBlockPresenter.h>

namespace hspr {

class BPEventBlock : public BlockPresenter
{
public:
	BPEventBlock(xsc::Block* block, ILevelView* levelView);
	virtual ~BPEventBlock();

	// ================================================================================================================
	private: friend Module;
	static void InitBPEventBlock();
	static void TermBPEventBlock();
	// ================================================================================================================
};

} // namespace hspr
