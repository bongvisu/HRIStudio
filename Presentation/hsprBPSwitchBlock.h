#pragma once

#include <hsmoSwitchBlock.h>
#include <hsprBlockPresenter.h>

namespace hspr {

class BPSwitchBlock : public BlockPresenter
{
public:
	BPSwitchBlock(xsc::Block* block, ILevelView* levelView);
	virtual ~BPSwitchBlock();

	virtual void Render(ID2D1DeviceContext* context, double t) override;

	// ================================================================================================================
	private: friend Module;
	static void InitBPSwitchBlock();
	static void TermBPSwitchBlock();
	// ================================================================================================================
};

} // namespace hspr
