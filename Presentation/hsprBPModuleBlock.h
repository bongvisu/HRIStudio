#pragma once

#include <hsmoModuleBlock.h>
#include <hsprBlockPresenter.h>

namespace hspr {

class BPModuleBlock : public BlockPresenter
{
public:
	BPModuleBlock(xsc::Block* block, ILevelView* levelView);
	virtual ~BPModuleBlock();

	virtual void Render(ID2D1DeviceContext* context, double t) override;

	// ================================================================================================================
	private: friend Module;
	static void InitBPModuleBlock();
	static void TermBPModuleBlock();
	// ================================================================================================================
};

} // namespace hspr
