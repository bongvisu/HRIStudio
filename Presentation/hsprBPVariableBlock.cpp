#include "hsprPrecompiled.h"
#include "hsprBPVariableBlock.h"
#include "hsprResource.h"
#include <xscWicBitmap.h>
#include <xscD2DRenderer.h>

using namespace std;
using namespace xsc;
using namespace hsmo;

namespace hspr {

// ====================================================================================================================
static BlockPresenter* CreateBPVariableBlock(Block* block, ILevelView* levelView)
{
	return new BPVariableBlock(block, levelView);
}

void BPVariableBlock::InitBPVariableBlock()
{
	const wchar_t* rtti = XSC_RTTI(VariableBlock).GetName();

	msFactory[rtti] = CreateBPVariableBlock;

	WicBitmap wicBitmap;
	std::pair<ID2D1Bitmap1*, ID2D1Bitmap1*> bitmaps;

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_BLOCK_VARIABLE);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &bitmaps.first);
	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_BLOCK_VARIABLE_C);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &bitmaps.second);

	msBlockBitmaps[rtti] = bitmaps;
}

void BPVariableBlock::TermBPVariableBlock()
{
	const wchar_t* rtti = XSC_RTTI(VariableBlock).GetName();

	msBlockBitmaps[rtti].first->Release();
	msBlockBitmaps[rtti].second->Release();
}
// ====================================================================================================================

BPVariableBlock::BPVariableBlock(Block* block, ILevelView* levelView)
	:
	BlockPresenter(block, levelView)
{
}

BPVariableBlock::~BPVariableBlock()
{
}

} // namespace hspr
