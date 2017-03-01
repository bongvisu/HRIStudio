#pragma once

#include <hsmoModuleOutBlock.h>
#include <hsprBlockPresenter.h>

namespace hspr {

class BPModuleOutBlock : public BlockPresenter
{
public:
	BPModuleOutBlock(xsc::Block* block, ILevelView* levelView);
	virtual ~BPModuleOutBlock();

	virtual void Render(ID2D1DeviceContext* context, double t) override;

	// ================================================================================================================
	private: friend Module;
	static void InitBPModuleOutBlock();
	static void TermBPModuleOutBlock();
	// ================================================================================================================
};

} // namespace hspr
