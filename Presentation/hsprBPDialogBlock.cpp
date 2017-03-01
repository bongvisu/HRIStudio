#include "hsprPrecompiled.h"
#include "hsprBPDialogBlock.h"
#include "hsprResource.h"
#include <xscWicBitmap.h>
#include <xscD2DRenderer.h>

using namespace std;
using namespace xsc;
using namespace hsmo;

namespace hspr {

// ====================================================================================================================
static BlockPresenter* CreateBPDialogBlock(Block* block, ILevelView* levelView)
{
	return new BPDialogBlock(block, levelView);
}

void BPDialogBlock::InitBPDialogBlock()
{
	const wchar_t* rtti = XSC_RTTI(DialogBlock).GetName();

	msFactory[rtti] = CreateBPDialogBlock;

	WicBitmap wicBitmap;
	std::pair<ID2D1Bitmap1*, ID2D1Bitmap1*> bitmaps;

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_BLOCK_DIALOG);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &bitmaps.first);
	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_BLOCK_DIALOG_C);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &bitmaps.second);

	msBlockBitmaps[rtti] = bitmaps;
}

void BPDialogBlock::TermBPDialogBlock()
{
	const wchar_t* rtti = XSC_RTTI(DialogBlock).GetName();

	msBlockBitmaps[rtti].first->Release();
	msBlockBitmaps[rtti].second->Release();
}
// ====================================================================================================================

BPDialogBlock::BPDialogBlock(Block* block, ILevelView* levelView)
	:
	BlockPresenter(block, levelView)
{
}

BPDialogBlock::~BPDialogBlock()
{
}

} // namespace hspr
