#include "hsprPrecompiled.h"
#include "hsprBPSwitchBlock.h"
#include "hsprResource.h"
#include <xscWicBitmap.h>
#include <xscD2DRenderer.h>

using namespace std;
using namespace xsc;
using namespace hsmo;

namespace hspr {

// ====================================================================================================================
static ID2D1Bitmap1* gsDecoH;
static ID2D1Bitmap1* gsDecoV;

static BlockPresenter* CreateBPSwitchBlock(Block* block, ILevelView* levelView)
{
	return new BPSwitchBlock(block, levelView);
}

void BPSwitchBlock::InitBPSwitchBlock()
{
	const wchar_t* rtti = XSC_RTTI(SwitchBlock).GetName();

	msFactory[rtti] = CreateBPSwitchBlock;

	WicBitmap wicBitmap;
	std::pair<ID2D1Bitmap1*, ID2D1Bitmap1*> bitmaps;

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_BLOCK_SWITCH);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &bitmaps.first);
	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_BLOCK_SWITCH_C);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &bitmaps.second);

	msBlockBitmaps[rtti] = bitmaps;

	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_DECO_CASE_H);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsDecoH);
	wicBitmap.LoadFromResource(hspr::Module::handle, HSPR_IDR_DECO_CASE_V);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsDecoV);
}

void BPSwitchBlock::TermBPSwitchBlock()
{
	const wchar_t* rtti = XSC_RTTI(SwitchBlock).GetName();

	gsDecoH->Release();
	gsDecoV->Release();

	msBlockBitmaps[rtti].first->Release();
	msBlockBitmaps[rtti].second->Release();
}
// ====================================================================================================================

BPSwitchBlock::BPSwitchBlock(Block* block, ILevelView* levelView)
	:
	BlockPresenter(block, levelView)
{
}

BPSwitchBlock::~BPSwitchBlock()
{
}

void BPSwitchBlock::Render(ID2D1DeviceContext* context, double t)
{
	Block* block = static_cast<Block*>(mComponent);
	const auto& geometry = block->GetGeometry();
	auto rect(geometry.ToRectF());
	
	auto imageSize = mBitmap->GetPixelSize();
	int imageWidth = static_cast<int>(imageSize.width);
	int imageHeight = static_cast<int>(imageSize.height);

	static const int PORT_DECO_SIZE = 42;
	static const int DECO_OFFSET = 3;
	int leftOver;
	D2D1_RECT_F contentRect;
	D2D1_RECT_F textRect;
	D2D1_MATRIX_3X2_F transform;
	context->GetTransform(&transform);
	auto rotate = D2D1::Matrix3x2F::Rotation(90);
	D2D1_MATRIX_3X2_F translate = D2D1::Matrix3x2F::Identity();

	const auto& outputPorts = block->GetOutputPorts();
	int numOutputPorts = static_cast<int>(outputPorts.size());
	assert(numOutputPorts > SwitchBlock::MIN_NUM_CASES);
	const auto pivotPort = outputPorts[0];

	solidBrush->SetColor(NORMAL_COLOR);
	switch (pivotPort->GetAttachSide())
	{
	case MPRectangle::EDGE_L:
		{
			contentRect.left = rect.left + DECO_OFFSET;
			contentRect.right = contentRect.left + PORT_DECO_SIZE;
			for (int i = 0; i < numOutputPorts; i++)
			{
				const auto& location = outputPorts[i]->GetLocation();
				contentRect.top = location.y - 6.0f;
				contentRect.bottom = contentRect.top + 11;
				context->DrawBitmap(gsDecoH, contentRect);

				if (i < (numOutputPorts - 1))
				{
					swprintf_s(msTextBuf, L"CASE %02d", i + 1);
				}
				else
				{
					swprintf_s(msTextBuf, L"DEFAULT");
				}
				context->DrawText(msTextBuf, static_cast<UINT32>(wcslen(msTextBuf)),
					textFormatCentered, contentRect, solidBrush);
			}

			leftOver = geometry.GetWidth() - PORT_DECO_SIZE - DECO_OFFSET;
			contentRect.left = contentRect.right + (leftOver - imageWidth) / 2;
			contentRect.right = contentRect.left + imageWidth;
			contentRect.top = rect.top + ((geometry.GetHeight() - imageHeight) / 2);
			contentRect.bottom = contentRect.top + imageHeight;
			context->DrawBitmap(mBitmap, contentRect);

			contentRect.top = contentRect.bottom - 11;
			contentRect.bottom = contentRect.top + 10;
			contentRect.left -= 150;
			contentRect.right += 150;
			const auto& name = block->GetName();
			context->DrawText(name.c_str(), static_cast<UINT32>(name.length()),
				textFormatCentered, contentRect, solidBrush);
		}
		break;

	case MPRectangle::EDGE_R:
		{
			contentRect.left = rect.right - PORT_DECO_SIZE - DECO_OFFSET;
			contentRect.right = contentRect.left + PORT_DECO_SIZE;
			for (int i = 0; i < numOutputPorts; i++)
			{
				const auto& location = outputPorts[i]->GetLocation();
				contentRect.top = location.y - 6.0f;
				contentRect.bottom = contentRect.top + 11;
				context->DrawBitmap(gsDecoH, contentRect);

				if (i < (numOutputPorts - 1))
				{
					swprintf_s(msTextBuf, L"CASE %02d", i + 1);
				}
				else
				{
					swprintf_s(msTextBuf, L"DEFAULT");
				}
				context->DrawText(msTextBuf, static_cast<UINT32>(wcslen(msTextBuf)),
					textFormatCentered, contentRect, solidBrush);
			}

			leftOver = geometry.GetWidth() - PORT_DECO_SIZE - DECO_OFFSET;
			contentRect.left = rect.left + ((leftOver - imageWidth) / 2);
			contentRect.right = contentRect.left + imageWidth;
			contentRect.top = rect.top + ((geometry.GetHeight() - imageHeight) / 2);;
			contentRect.bottom = contentRect.top + imageHeight;
			context->DrawBitmap(mBitmap, contentRect);

			contentRect.top = contentRect.bottom - 11;
			contentRect.bottom = contentRect.top + 10;
			contentRect.left -= 150;
			contentRect.right += 150;
			const auto& name = block->GetName();
			context->DrawText(name.c_str(), static_cast<UINT32>(name.length()),
				textFormatCentered, contentRect, solidBrush);
		}
		break;
	
	case MPRectangle::EDGE_T:
		{
			contentRect.top = rect.top + DECO_OFFSET;
			contentRect.bottom = contentRect.top + PORT_DECO_SIZE;
			for (int i = 0; i < numOutputPorts; i++)
			{
				const auto& location = outputPorts[i]->GetLocation();
				contentRect.left = location.x - 6.0f;
				contentRect.right = contentRect.left + 11;
				context->DrawBitmap(gsDecoV, contentRect);

				if (i < (numOutputPorts - 1))
				{
					swprintf_s(msTextBuf, L"CASE %02d", i + 1);
				}
				else
				{
					swprintf_s(msTextBuf, L"DEFAULT");
				}

				textRect.left = 0;
				textRect.right = textRect.left + PORT_DECO_SIZE;
				textRect.top = 0;
				textRect.bottom = textRect.top + 11;

				translate._31 = static_cast<FLOAT>(contentRect.right);
				translate._32 = static_cast<FLOAT>(contentRect.top);
				auto translated = translate * transform;
				context->SetTransform(rotate * translated);

				context->DrawText(msTextBuf, static_cast<UINT32>(wcslen(msTextBuf)),
					textFormatCentered, textRect, solidBrush);

				context->SetTransform(transform);
			}

			leftOver = geometry.GetHeight() - PORT_DECO_SIZE - DECO_OFFSET;
			contentRect.left = rect.left + ((geometry.GetWidth() - imageWidth) / 2);
			contentRect.right = contentRect.left + imageWidth;
			contentRect.top = contentRect.bottom + (leftOver - imageHeight) / 2;
			contentRect.bottom = contentRect.top + imageHeight;
			context->DrawBitmap(mBitmap, contentRect);

			contentRect.top = contentRect.bottom - 11;
			contentRect.bottom = contentRect.top + 10;
			contentRect.left -= 150;
			contentRect.right += 150;
			const auto& name = block->GetName();
			context->DrawText(name.c_str(), static_cast<UINT32>(name.length()),
				textFormatCentered, contentRect, solidBrush);
		}
		break;

	case MPRectangle::EDGE_B:
		{
			contentRect.top = rect.bottom - DECO_OFFSET - PORT_DECO_SIZE;
			contentRect.bottom = contentRect.top + PORT_DECO_SIZE;
			for (int i = 0; i < numOutputPorts; i++)
			{
				const auto& location = outputPorts[i]->GetLocation();
				contentRect.left = location.x - 6.0f;
				contentRect.right = contentRect.left + 11;
				context->DrawBitmap(gsDecoV, contentRect);

				if (i < (numOutputPorts - 1))
				{
					swprintf_s(msTextBuf, L"CASE %02d", i + 1);
				}
				else
				{
					swprintf_s(msTextBuf, L"DEFAULT");
				}

				textRect.left = 0;
				textRect.right = textRect.left + PORT_DECO_SIZE;
				textRect.top = 0;
				textRect.bottom = textRect.top + 11;

				translate._31 = static_cast<FLOAT>(contentRect.right);
				translate._32 = static_cast<FLOAT>(contentRect.top);
				auto translated = translate * transform;
				context->SetTransform(rotate * translated);

				context->DrawText(msTextBuf, static_cast<UINT32>(wcslen(msTextBuf)),
					textFormatCentered, textRect, solidBrush); 

				context->SetTransform(transform);
			}

			leftOver = geometry.GetHeight() - PORT_DECO_SIZE - DECO_OFFSET;
			contentRect.left = rect.left + ((geometry.GetWidth() - imageWidth) / 2);
			contentRect.right = contentRect.left + imageWidth;
			contentRect.top = rect.top + (leftOver - imageHeight) / 2;
			contentRect.bottom = contentRect.top + imageHeight;
			context->DrawBitmap(mBitmap, contentRect);

			contentRect.top = contentRect.bottom - 11;
			contentRect.bottom = contentRect.top + 10;
			contentRect.left -= 150;
			contentRect.right += 150;
			const auto& name = block->GetName();
			context->DrawText(name.c_str(), static_cast<UINT32>(name.length()),
				textFormatCentered, contentRect, solidBrush);
		}
		break;
	}

	RenderCommon(context, t, rect);	
}

} // namespace hspr
