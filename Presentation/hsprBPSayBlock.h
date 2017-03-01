#pragma once

#include <hsmoSayBlock.h>
#include <hsprBlockPresenter.h>

namespace hspr {

class BPSayBlock : public BlockPresenter
{
public:
	BPSayBlock(xsc::Block* block, ILevelView* levelView);
	virtual ~BPSayBlock();

	// ================================================================================================================
	private: friend Module;
	static void InitBPSayBlock();
	static void TermBPSayBlock();
	// ================================================================================================================
};

} // namespace hspr
