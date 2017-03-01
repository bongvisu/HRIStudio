#pragma once

#include <hsmoIfBlock.h>
#include <hsprBlockPresenter.h>

namespace hspr {

class BPIfBlock : public BlockPresenter
{
public:
	BPIfBlock(xsc::Block* block, ILevelView* levelView);
	virtual ~BPIfBlock();

	virtual void Render(ID2D1DeviceContext* context, double t) override;

	// ================================================================================================================
	private: friend Module;
	static void InitBPIfBlock();
	static void TermBPIfBlock();
	// ================================================================================================================
};

} // namespace hspr
