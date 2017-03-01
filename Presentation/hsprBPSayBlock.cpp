#include "hsprPrecompiled.h"
#include "hsprBPSayBlock.h"
#include "hsprResource.h"
#include <xscWicBitmap.h>
#include <xscD2DRenderer.h>

using namespace std;
using namespace xsc;
using namespace hsmo;

namespace hspr {

// ====================================================================================================================
static BlockPresenter* CreateBPSayBlock(Block* block, ILevelView* levelView)
{
	return new BPSayBlock(block, levelView);
}

void BPSayBlock::InitBPSayBlock()
{
	const wchar_t* rtti = XSC_RTTI(SayBlock).GetName();

	msFactory[rtti] = CreateBPSayBlock;

	WicBitmap wicBitmap;
	std::pair<ID2D1Bitmap1*, ID2D1Bitmap1*> bitmaps;

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_BLOCK_SAY);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &bitmaps.first);
	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_BLOCK_SAY_C);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &bitmaps.second);

	msBlockBitmaps[rtti] = bitmaps;
}

void BPSayBlock::TermBPSayBlock()
{
	const wchar_t* rtti = XSC_RTTI(SayBlock).GetName();

	msBlockBitmaps[rtti].first->Release();
	msBlockBitmaps[rtti].second->Release();
}
// ====================================================================================================================

BPSayBlock::BPSayBlock(Block* block, ILevelView* levelView)
	:
	BlockPresenter(block, levelView)
{
}

BPSayBlock::~BPSayBlock()
{
}

} // namespace hspr
