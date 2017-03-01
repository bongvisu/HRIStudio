#pragma once

#include <hsmoModuleInBlock.h>
#include <hsprBlockPresenter.h>

namespace hspr {

class BPModuleInBlock : public BlockPresenter
{
public:
	BPModuleInBlock(xsc::Block* block, ILevelView* levelView);
	virtual ~BPModuleInBlock();

	virtual void Render(ID2D1DeviceContext* context, double t) override;

	// ================================================================================================================
	private: friend Module;
	static void InitBPModuleInBlock();
	static void TermBPModuleInBlock();
	// ================================================================================================================
};

} // namespace hspr
