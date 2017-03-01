#include "hsprPrecompiled.h"
#include "hsprBPModuleInBlock.h"
#include "hsprResource.h"
#include <xscWicBitmap.h>
#include <xscD2DRenderer.h>

using namespace std;
using namespace xsc;
using namespace hsmo;

namespace hspr {

// ====================================================================================================================
static BlockPresenter* CreateBPModuleInBlock(Block* block, ILevelView* levelView)
{
	return new BPModuleInBlock(block, levelView);
}

void BPModuleInBlock::InitBPModuleInBlock()
{
	const wchar_t* rtti = XSC_RTTI(ModuleInBlock).GetName();

	msFactory[rtti] = CreateBPModuleInBlock;

	WicBitmap wicBitmap;
	std::pair<ID2D1Bitmap1*, ID2D1Bitmap1*> bitmaps;

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_BLOCK_MODULE_IN);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &bitmaps.first);
	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_BLOCK_MODULE_IN_C);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &bitmaps.second);

	msBlockBitmaps[rtti] = bitmaps;
}

void BPModuleInBlock::TermBPModuleInBlock()
{
	const wchar_t* rtti = XSC_RTTI(ModuleInBlock).GetName();

	msBlockBitmaps[rtti].first->Release();
	msBlockBitmaps[rtti].second->Release();
}
// ====================================================================================================================

BPModuleInBlock::BPModuleInBlock(Block* block, ILevelView* levelView)
	:
	BlockPresenter(block, levelView)
{
}

BPModuleInBlock::~BPModuleInBlock()
{
}

void BPModuleInBlock::Render(ID2D1DeviceContext* context, double t)
{
	auto* block = static_cast<ModuleInBlock*>(mComponent);
	const auto& geometry = block->GetGeometry();
	const auto rect(geometry.ToRectF());
	D2D1_RECT_F contentRect;

	auto imageSize = mBitmap->GetPixelSize();
	int imageWidth = static_cast<int>(imageSize.width);
	int imageHeight = static_cast<int>(imageSize.height);

	solidBrush->SetColor(NORMAL_COLOR);

	contentRect.left = rect.left + (geometry.GetWidth() - imageWidth) / 2;
	contentRect.top = rect.top + (geometry.GetHeight() - imageHeight) / 2;
	contentRect.right = contentRect.left + imageWidth;
	contentRect.bottom = contentRect.top + imageHeight;
	context->DrawBitmap(mBitmap, &contentRect);
	const auto copied(contentRect);

	contentRect.top = contentRect.bottom - 11;
	contentRect.bottom = contentRect.top + 10;
	contentRect.left -= 150;
	contentRect.right += 150;
	const auto& name = block->GetName();
	context->DrawText(name.c_str(), static_cast<UINT32>(name.length()), textFormatCentered, contentRect, solidBrush);

	int index = block->GetIndex();
	swprintf_s(msTextBuf, L"%02d", index + 1);
	contentRect.left = copied.left + 14;
	contentRect.top = copied.top + 18;
	contentRect.right = copied.right - 6;
	contentRect.bottom = contentRect.top + 14;
	context->DrawText(msTextBuf, static_cast<UINT32>(wcslen(msTextBuf)), textFormatCentered, contentRect, solidBrush);

	RenderCommon(context, t, rect);
}

} // namespace hspr
