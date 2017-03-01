#pragma once

#include <hsmoDialogBlock.h>
#include <hsprBlockPresenter.h>

namespace hspr {

class BPDialogBlock : public BlockPresenter
{
public:
	BPDialogBlock(xsc::Block* block, ILevelView* levelView);
	virtual ~BPDialogBlock();

	// ================================================================================================================
	private: friend Module;
	static void InitBPDialogBlock();
	static void TermBPDialogBlock();
	// ================================================================================================================
};

} // namespace hspr
