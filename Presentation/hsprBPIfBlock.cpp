#include "hsprPrecompiled.h"
#include "hsprBPIfBlock.h"
#include "hsprResource.h"
#include <xscWicBitmap.h>
#include <xscD2DRenderer.h>

using namespace std;
using namespace xsc;
using namespace hsmo;

namespace hspr {

// ====================================================================================================================
static ID2D1Bitmap1* gsDecoT;
static ID2D1Bitmap1* gsDecoF;

static BlockPresenter* CreateBPIfBlock(Block* block, ILevelView* levelView)
{
	return new BPIfBlock(block, levelView);
}

void BPIfBlock::InitBPIfBlock()
{
	const wchar_t* rtti = XSC_RTTI(IfBlock).GetName();

	msFactory[rtti] = CreateBPIfBlock;

	WicBitmap wicBitmap;
	std::pair<ID2D1Bitmap1*, ID2D1Bitmap1*> bitmaps;

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_BLOCK_IF);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &bitmaps.first);
	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_BLOCK_IF_C);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &bitmaps.second);

	msBlockBitmaps[rtti] = bitmaps;

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_DECO_PORT_T);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsDecoT);
	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_DECO_PORT_F);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsDecoF);
}

void BPIfBlock::TermBPIfBlock()
{
	const wchar_t* rtti = XSC_RTTI(IfBlock).GetName();

	gsDecoT->Release();
	gsDecoF->Release();

	msBlockBitmaps[rtti].first->Release();
	msBlockBitmaps[rtti].second->Release();
}
// ====================================================================================================================

BPIfBlock::BPIfBlock(Block* block, ILevelView* levelView)
	:
	BlockPresenter(block, levelView)
{
}

BPIfBlock::~BPIfBlock()
{
}

void BPIfBlock::Render(ID2D1DeviceContext* context, double t)
{
	auto block = static_cast<Block*>(mComponent);
	auto& geometry = block->GetGeometry();
	auto rect(geometry.ToRectF());

	auto imageSize = mBitmap->GetPixelSize();
	int imageWidth = static_cast<int>(imageSize.width);
	int imageHeight = static_cast<int>(imageSize.height);

	static const int PORT_DECO_SIZE = 11;
	static const int DECO_OFFSET = 3;
	int leftOver;
	D2D1_RECT_F contentRect;

	const auto& outputPorts = block->GetOutputPorts();
	int numOutputPorts = static_cast<int>(outputPorts.size());
	assert(numOutputPorts == 2);

	solidBrush->SetColor(NORMAL_COLOR);

	switch (outputPorts[0]->GetAttachSide())
	{
	case MPRectangle::EDGE_L:
		contentRect.left = rect.left + DECO_OFFSET;
		contentRect.right = contentRect.left + PORT_DECO_SIZE;

		// true
		{
			const auto& location = outputPorts[0]->GetLocation();
			contentRect.top = location.y - 6.0f;
			contentRect.bottom = contentRect.top + 11;
			context->DrawBitmap(gsDecoT, contentRect);
		}
		// false
		{
			const auto& location = outputPorts[1]->GetLocation();
			contentRect.top = location.y - 6.0f;
			contentRect.bottom = contentRect.top + 11;
			context->DrawBitmap(gsDecoF, contentRect);
		}

		leftOver = geometry.GetWidth() - DECO_OFFSET - PORT_DECO_SIZE;
		contentRect.left = contentRect.right + ((leftOver - imageWidth) / 2);
		contentRect.right = contentRect.left + imageWidth;
		contentRect.top = rect.top + ((geometry.GetHeight() - imageHeight) / 2);
		contentRect.bottom = contentRect.top + imageHeight;
		context->DrawBitmap(mBitmap, contentRect);
		break;

	case MPRectangle::EDGE_R:
		contentRect.left = rect.right - DECO_OFFSET - PORT_DECO_SIZE;
		contentRect.right = contentRect.left + PORT_DECO_SIZE;

		// true
		{
			const auto& location = outputPorts[0]->GetLocation();
			contentRect.top = location.y - 6.0f;
			contentRect.bottom = contentRect.top + PORT_DECO_SIZE;
			context->DrawBitmap(gsDecoT, contentRect);
		}
		// false
		{
			const auto& location = outputPorts[1]->GetLocation();
			contentRect.top = location.y - 6.0f;
			contentRect.bottom = contentRect.top + PORT_DECO_SIZE;
			context->DrawBitmap(gsDecoF, contentRect);
		}

		leftOver = geometry.GetWidth() - DECO_OFFSET - PORT_DECO_SIZE;
		contentRect.left = rect.left + ((leftOver - imageWidth) / 2);
		contentRect.right = contentRect.left + imageWidth;
		contentRect.top = rect.top + ((geometry.GetHeight() - imageHeight) / 2);
		contentRect.bottom = contentRect.top + imageHeight;
		context->DrawBitmap(mBitmap, contentRect);
		break;

	case MPRectangle::EDGE_T:
		contentRect.top = rect.top + DECO_OFFSET;
		contentRect.bottom = contentRect.top + PORT_DECO_SIZE;

		// true
		{
			const auto& location = outputPorts[0]->GetLocation();
			contentRect.left = location.x - 6.0f;
			contentRect.right = contentRect.left + PORT_DECO_SIZE;
			context->DrawBitmap(gsDecoT, contentRect);
		}
		// false
		{
			const auto& location = outputPorts[1]->GetLocation();
			contentRect.left = location.x - 6.0f;
			contentRect.right = contentRect.left + PORT_DECO_SIZE;
			context->DrawBitmap(gsDecoF, contentRect);
		}

		contentRect.left = rect.left + ((geometry.GetWidth() - imageWidth) / 2);
		contentRect.right = contentRect.left + imageWidth;
		leftOver = geometry.GetHeight() - DECO_OFFSET - PORT_DECO_SIZE;
		contentRect.top = contentRect.bottom + ((leftOver - imageHeight) / 2);
		contentRect.bottom = contentRect.top + imageHeight;
		context->DrawBitmap(mBitmap, contentRect);
		break;

	case MPRectangle::EDGE_B:
		contentRect.top = rect.bottom - DECO_OFFSET - PORT_DECO_SIZE;
		contentRect.bottom = contentRect.top + PORT_DECO_SIZE;

		// true
		{
			const auto& location = outputPorts[0]->GetLocation();
			contentRect.left = location.x - 6.0f;
			contentRect.right = contentRect.left + PORT_DECO_SIZE;
			context->DrawBitmap(gsDecoT, contentRect);
		}
		// false
		{
			const auto& location = outputPorts[1]->GetLocation();
			contentRect.left = location.x - 6.0f;
			contentRect.right = contentRect.left + PORT_DECO_SIZE;
			context->DrawBitmap(gsDecoF, contentRect);
		}

		contentRect.left = rect.left + ((geometry.GetWidth() - imageWidth) / 2);
		contentRect.right = contentRect.left + imageWidth;
		leftOver = geometry.GetHeight() - DECO_OFFSET - PORT_DECO_SIZE;
		contentRect.top = rect.top + ((leftOver - imageHeight) / 2);
		contentRect.bottom = contentRect.top + imageHeight;
		context->DrawBitmap(mBitmap, contentRect);
		break;
	}

	// Name -----------------------------------------------------------------------------------------------------------
	contentRect.top = contentRect.bottom - 11;
	contentRect.bottom = contentRect.top + 10;
	contentRect.left -= 150;
	contentRect.right += 150;
	const auto& name = block->GetName();
	context->DrawText(name.c_str(), static_cast<UINT32>(name.length()), textFormatCentered, contentRect, solidBrush);
	// ----------------------------------------------------------------------------------------------------------------

	RenderCommon(context, t, rect);
}

} // namespace hspr
