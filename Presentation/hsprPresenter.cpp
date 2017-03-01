#include "hsprPrecompiled.h"
#include "hsprPresenter.h"
#include "hsprIPointSnapTarget.h"
#include <xscDWrite.h>
#include <xscD2DRenderer.h>

using namespace std;
using namespace xsc;
using namespace hsmo;

namespace hspr {

// ====================================================================================================================
const D2D1_COLOR_F Presenter::HOT_COLOR			= { 255.0f / 255.0f, 165.0f / 255.0f, 0.0f / 255.0f, 1.0f };
const D2D1_COLOR_F Presenter::SELECTED_COLOR	= { 51.0f / 255.0f, 153.0f / 255.0f, 255.0f / 255.0f, 1.0f };
const D2D1_COLOR_F Presenter::PRIMARY_COLOR		= { 0 / 255.0f, 0 / 255.0f, 255 / 255.0f, 1.0f };
const D2D1_COLOR_F Presenter::NORMAL_COLOR		= { 40 / 255.0f, 45 / 255.0f, 50 / 255.0f, 1.0f };
const D2D1_COLOR_F Presenter::TEAL_COLOR		= { 0.3f, 0.7f, 0.7f, 1 };
const D2D1_COLOR_F Presenter::PURPLE_COLOR		= { 0.7f, 0.3f, 0.7f, 1.0f };

IDWriteTextFormat* Presenter::textFormatTitle;
IDWriteTextFormat* Presenter::textFormatLeading;
IDWriteTextFormat* Presenter::textFormatCentered;
IDWriteTextFormat* Presenter::textFormatTrailing;

ID2D1SolidColorBrush*	Presenter::solidBrush;
ID2D1StrokeStyle1*		Presenter::dashedStrokeStyle;
ID2D1StrokeStyle1*		Presenter::dashDottedStrokeStyle;

wchar_t Presenter::msTextBuf[Presenter::TEXT_BUF_SIZE];

void Presenter::InitPresenter()
{
	DWRITE_TRIMMING trimming;
	trimming.granularity = DWRITE_TRIMMING_GRANULARITY_NONE;
	trimming.delimiter = 0;
	trimming.delimiterCount = 0;

	DWrite::factory->CreateTextFormat(
		L"Cambria", nullptr,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		10.0f, L"", &textFormatTitle);
	textFormatTitle->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	textFormatTitle->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	textFormatTitle->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
	textFormatTitle->SetTrimming(&trimming, nullptr);

	DWrite::factory->CreateTextFormat(
		L"D2Coding", nullptr,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		10.0f, L"", &textFormatLeading);
	textFormatLeading->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	textFormatLeading->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	textFormatLeading->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
	textFormatLeading->SetTrimming(&trimming, nullptr);

	DWrite::factory->CreateTextFormat(
		L"D2Coding", nullptr,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		10.0f, L"", &textFormatCentered);
	textFormatCentered->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	textFormatCentered->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	textFormatCentered->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
	textFormatCentered->SetTrimming(&trimming, nullptr);

	DWrite::factory->CreateTextFormat(
		L"D2Coding", nullptr,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		10.0f, L"", &textFormatTrailing);
	textFormatTrailing->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
	textFormatTrailing->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	textFormatTrailing->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
	textFormatTrailing->SetTrimming(&trimming, nullptr);
	
	D2D1_STROKE_STYLE_PROPERTIES1 strokeProp;
	strokeProp.startCap = D2D1_CAP_STYLE_FLAT;
	strokeProp.endCap = D2D1_CAP_STYLE_FLAT;
	strokeProp.dashCap = D2D1_CAP_STYLE_FLAT;
	strokeProp.lineJoin = D2D1_LINE_JOIN_MITER;
	strokeProp.miterLimit = 7.0f;
	strokeProp.dashStyle = D2D1_DASH_STYLE_CUSTOM;
	strokeProp.dashOffset = 0.5f;
	strokeProp.transformType = D2D1_STROKE_TRANSFORM_TYPE_NORMAL;
	FLOAT dashes[] = { 1.0f, 1.0f };
	D2DRenderer::factory->CreateStrokeStyle(strokeProp, dashes, 2, &dashedStrokeStyle);

	FLOAT dashDots[] = { 4.0f, 1.0f, 1.0f, 1.0f };
	strokeProp.dashOffset = 0.0f;
	D2DRenderer::factory->CreateStrokeStyle(strokeProp, dashDots, 4, &dashDottedStrokeStyle);

	D2DRenderer::resourcingContext->CreateSolidColorBrush(NORMAL_COLOR, &solidBrush);
}

void Presenter::TermPresenter()
{
	solidBrush->Release();
	dashDottedStrokeStyle->Release();
	dashedStrokeStyle->Release();

	textFormatTrailing->Release();
	textFormatCentered->Release();
	textFormatLeading->Release();
	textFormatTitle->Release();
}
// ====================================================================================================================

const int Presenter::POINTER_ACTION_LATENCY = 5;

Presenter::Presenter(Component* component, Presenter* parent, ILevelView* levelView)
	:
	mComponent(component),
	mParent(parent),
	mLevelView(levelView)
{
}

Presenter::~Presenter()
{
}

void Presenter::OnChildAddition(const ComponentSet&)
{
	// to be overriden.
}

void Presenter::OnChildRemoval(const ComponentSet&)
{
	// to be overriden.
}

void Presenter::OnPropertyChange(Entity::PropID)
{
	// to be overriden.
}

void Presenter::OnNodalGeometryChange()
{
	UpdateBound();
}

Component* Presenter::GetComponent() const
{
	return mComponent;
}

Presenter* Presenter::GetParent() const
{
	return mParent;
}

IVisual* Presenter::GetVisualInterface() const
{
	return nullptr;
}

ISelectable* Presenter::GetSelectableInterface() const
{
	return nullptr;
}

IAlignable* Presenter::GetAlignableInterface() const
{
	return nullptr;
}

bool Presenter::IsChild(Presenter* presenter) const
{
	if (mChildren.find(presenter) != mChildren.end())
	{
		return true;
	}

	for (auto& child : mChildren)
	{
		if (child->IsChild(presenter))
		{
			return true;
		}
	}

	return false;
}

bool Presenter::Contains(Presenter* presenter) const
{
	if (this == presenter)
	{
		return true;
	}

	if (mChildren.find(presenter) != mChildren.end())
	{
		return true;
	}

	for (auto& child : mChildren)
	{
		if (child->IsChild(presenter))
		{
			return true;
		}
	}

	return false;
}

void Presenter::GetChildren(PresenterVec& children) const
{
	for (auto& child : mChildren)
	{
		children.push_back(child.Get());
		child->GetChildren(children);
	}
}

void Presenter::GetChildren(PresenterSet& children) const
{
	for (auto& child : mChildren)
	{
		children.insert(child.Get());
		child->GetChildren(children);
	}
}

const MPRectangle& Presenter::GetBound() const
{
	return mBound;
}

void Presenter::UpdateBound()
{
	// stub
}

Presenter* Presenter::FindHit(const MPVector&) const
{
	// stub
	return nullptr;
}

void Presenter::CollectWith(const MPRectangle&, PresenterSet&)
{
	// stub
}

void Presenter::BuildSnapTargets(PresenterVec&, PresenterVec&)
{
	// stub
}

Presenter::SnapResult Presenter::FindSnap(const MPVector&, const PresenterVec&, const PresenterVec&) const
{
	// stub
	return SnapResult();
}

void Presenter::Render(ID2D1DeviceContext*, double)
{
	// stub
}

void Presenter::DrawMoveGhost(ID2D1DeviceContext*)
{
	// stub
}

void Presenter::DrawSnapEffect(ID2D1DeviceContext*)
{
	// stub
}

void Presenter::DrawSelectionRect(ID2D1DeviceContext* context, const MPRectangle& rect)
{
	solidBrush->SetColor(NORMAL_COLOR);
	context->DrawRectangle(rect.ToRectF(), solidBrush, 1, dashedStrokeStyle);
}

Presenter::SnapResult::SnapResult()
{
	presenter = nullptr;
}

void Presenter::SnapResult::Clear()
{
	presenter = nullptr;
	offset.MakeZero();
}

} // namespace hspr
