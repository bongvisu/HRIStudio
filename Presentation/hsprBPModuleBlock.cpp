#include "hsprPrecompiled.h"
#include "hsprBPModuleBlock.h"
#include "hsprResource.h"
#include <xscWicBitmap.h>
#include <xscD2DRenderer.h>

using namespace std;
using namespace xsc;
using namespace hsmo;

namespace hspr {

// ====================================================================================================================
static BlockPresenter* CreateBPModuleBlock(Block* block, ILevelView* levelView)
{
	return new BPModuleBlock(block, levelView);
}

void BPModuleBlock::InitBPModuleBlock()
{
	const wchar_t* rtti = XSC_RTTI(ModuleBlock).GetName();

	msFactory[rtti] = CreateBPModuleBlock;

	WicBitmap wicBitmap;
	std::pair<ID2D1Bitmap1*, ID2D1Bitmap1*> bitmaps;

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_BLOCK_MODULE);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &bitmaps.first);
	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_BLOCK_MODULE_C);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &bitmaps.second);

	msBlockBitmaps[rtti] = bitmaps;
}

void BPModuleBlock::TermBPModuleBlock()
{
	const wchar_t* rtti = XSC_RTTI(ModuleBlock).GetName();

	msBlockBitmaps[rtti].first->Release();
	msBlockBitmaps[rtti].second->Release();
}
// ====================================================================================================================

BPModuleBlock::BPModuleBlock(Block* block, ILevelView* levelView)
	:
	BlockPresenter(block, levelView)
{
}

BPModuleBlock::~BPModuleBlock()
{
}

void BPModuleBlock::Render(ID2D1DeviceContext* context, double t)
{
	auto* block = static_cast<ModuleBlock*>(mComponent);
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

	contentRect.top = contentRect.bottom - 11;
	contentRect.bottom = contentRect.top + 10;
	contentRect.left -= 150;
	contentRect.right += 150;
	const auto& name = block->GetName();
	context->DrawText(name.c_str(), static_cast<UINT32>(name.length()), textFormatCentered, contentRect, solidBrush);

	static const FLOAT TEXT_BOX_WIDTH = 20;
	static const FLOAT TEXT_BOX_HALF_WIDTH = 10;
	static const FLOAT TEXT_BOX_HEIGHT = 12;
	static const FLOAT TEXT_BOX_HALF_HEIGHT = 6;

	const auto& inputPorts = block->GetInputPorts();
	int numPorts = static_cast<int>(inputPorts.size());
	for (int i = 0; i < numPorts; ++i)
	{
		auto location = inputPorts[i]->GetLocation();
		IDWriteTextFormat* textFormat;
		switch (inputPorts[i]->GetAttachSide())
		{
		case MPRectangle::EDGE_L:
			contentRect.left = location.x + 3.0f;
			contentRect.right = contentRect.left + TEXT_BOX_WIDTH;
			contentRect.top = location.y - TEXT_BOX_HALF_HEIGHT;
			contentRect.bottom = contentRect.top + TEXT_BOX_HEIGHT;
			textFormat = Presenter::textFormatLeading;
			break;

		case MPRectangle::EDGE_R:
			contentRect.right = location.x - 3.0f;
			contentRect.left = contentRect.right - TEXT_BOX_WIDTH;
			contentRect.top = location.y - TEXT_BOX_HALF_HEIGHT;
			contentRect.bottom = contentRect.top + TEXT_BOX_HEIGHT;
			textFormat = Presenter::textFormatTrailing;
			break;

		case MPRectangle::EDGE_T:
			contentRect.left = location.x - TEXT_BOX_HALF_WIDTH - 1;
			contentRect.right = contentRect.left + TEXT_BOX_WIDTH;
			contentRect.top = location.y + 1.0f;
			contentRect.bottom = contentRect.top + TEXT_BOX_HEIGHT;
			textFormat = Presenter::textFormatCentered;
			break;

		case MPRectangle::EDGE_B:
			contentRect.left = location.x - TEXT_BOX_HALF_WIDTH - 1;
			contentRect.right = contentRect.left + TEXT_BOX_WIDTH;
			contentRect.bottom = static_cast<FLOAT>(location.y);
			contentRect.top = contentRect.bottom - TEXT_BOX_HEIGHT;
			textFormat = Presenter::textFormatCentered;
			break;
		}

		swprintf_s(msTextBuf, L"%02d", i + 1);
		context->DrawText(msTextBuf, static_cast<UINT32>(wcslen(msTextBuf)), textFormat, contentRect, solidBrush);
	}

	const auto& outputPorts = block->GetOutputPorts();
	numPorts = static_cast<int>(outputPorts.size());
	for (int i = 0; i < numPorts; ++i)
	{
		auto location = outputPorts[i]->GetLocation();
		IDWriteTextFormat* textFormat;
		switch (outputPorts[i]->GetAttachSide())
		{
		case MPRectangle::EDGE_L:
			contentRect.left = location.x + 3.0f;
			contentRect.right = contentRect.left + TEXT_BOX_WIDTH;
			contentRect.top = location.y - TEXT_BOX_HALF_HEIGHT;
			contentRect.bottom = contentRect.top + TEXT_BOX_HEIGHT;
			textFormat = Presenter::textFormatLeading;
			break;

		case MPRectangle::EDGE_R:
			contentRect.right = location.x - 3.0f;
			contentRect.left = contentRect.right - TEXT_BOX_WIDTH;
			contentRect.top = location.y - TEXT_BOX_HALF_HEIGHT;
			contentRect.bottom = contentRect.top + TEXT_BOX_HEIGHT;
			textFormat = Presenter::textFormatTrailing;
			break;

		case MPRectangle::EDGE_T:
			contentRect.left = location.x - TEXT_BOX_HALF_WIDTH - 1;
			contentRect.right = contentRect.left + TEXT_BOX_WIDTH;
			contentRect.top = location.y + 1.0f;
			contentRect.bottom = contentRect.top + TEXT_BOX_HEIGHT;
			textFormat = Presenter::textFormatCentered;
			break;

		case MPRectangle::EDGE_B:
			contentRect.left = location.x - TEXT_BOX_HALF_WIDTH - 1;
			contentRect.right = contentRect.left + TEXT_BOX_WIDTH;
			contentRect.bottom = static_cast<FLOAT>(location.y);
			contentRect.top = contentRect.bottom - TEXT_BOX_HEIGHT;
			textFormat = Presenter::textFormatCentered;
			break;
		}

		swprintf_s(msTextBuf, L"%02d", i + 1);
		context->DrawText(msTextBuf, static_cast<UINT32>(wcslen(msTextBuf)), textFormat, contentRect, solidBrush);
	}

	RenderCommon(context, t, rect);
}

} // namespace hspr
