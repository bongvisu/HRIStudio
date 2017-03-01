#include "hsprPrecompiled.h"
#include "hsprBPEventBlock.h"
#include "hsprResource.h"
#include <xscWicBitmap.h>
#include <xscD2DRenderer.h>

using namespace std;
using namespace xsc;
using namespace hsmo;

namespace hspr {

// ====================================================================================================================
static BlockPresenter* CreateBPEventBlock(Block* block, ILevelView* levelView)
{
	return new BPEventBlock(block, levelView);
}

void BPEventBlock::InitBPEventBlock()
{
	const wchar_t* rtti = XSC_RTTI(EventBlock).GetName();

	msFactory[rtti] = CreateBPEventBlock;

	WicBitmap wicBitmap;
	std::pair<ID2D1Bitmap1*, ID2D1Bitmap1*> bitmaps;

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_BLOCK_EVENT);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &bitmaps.first);
	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_BLOCK_EVENT_C);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &bitmaps.second);

	msBlockBitmaps[rtti] = bitmaps;
}

void BPEventBlock::TermBPEventBlock()
{
	const wchar_t* rtti = XSC_RTTI(EventBlock).GetName();

	msBlockBitmaps[rtti].first->Release();
	msBlockBitmaps[rtti].second->Release();
}
// ====================================================================================================================

BPEventBlock::BPEventBlock(Block* block, ILevelView* levelView)
	:
	BlockPresenter(block, levelView)
{
}

BPEventBlock::~BPEventBlock()
{
}

} // namespace hspr
