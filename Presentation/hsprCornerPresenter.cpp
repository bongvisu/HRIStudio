#include "hsprPrecompiled.h"
#include "hsprCornerPresenter.h"
#include "hsprIAlignable.h"

using namespace std;
using namespace xsc;
using namespace hsmo;

namespace hspr {

static const int BLOCK_CORNER_SIZE = 6;

CornerPresenter::CornerPresenter(Block* block, Presenter* parent, ILevelView* levelView, MPRectangle::Corner corner)
	:
	Presenter(block, parent, levelView),
	mCorner(corner),
	mHot(false)
{
}

CornerPresenter::~CornerPresenter()
{
}

PresenterType CornerPresenter::GetPresenterType() const
{
	return PRESENTER_TYPE_CORNER;
}

IVisual* CornerPresenter::GetVisualInterface() const
{
	return static_cast<IVisual*>(const_cast<CornerPresenter*>(this));
}

MPRectangle::Corner CornerPresenter::GetCorner() const
{
	return mCorner;
}

void CornerPresenter::SetHot(bool hot)
{
	mHot = hot;
}

bool CornerPresenter::GetHot() const
{
	return mHot;
}

void CornerPresenter::UpdateBound()
{
	const Block* block = static_cast<Block*>(mComponent);
	const MPRectangle& blockGeometry = block->GetGeometry();

	switch (mCorner)
	{
	case MPRectangle::CORNER_LT:
		mBound.l = blockGeometry.l - BLOCK_CORNER_SIZE;
		mBound.t = blockGeometry.t - BLOCK_CORNER_SIZE;
		break;

	case MPRectangle::CORNER_LB:
		mBound.l = blockGeometry.l - BLOCK_CORNER_SIZE;
		mBound.t = blockGeometry.b;
		break;

	case MPRectangle::CORNER_RT:
		mBound.l = blockGeometry.r;
		mBound.t = blockGeometry.t - BLOCK_CORNER_SIZE;
		break;

	case MPRectangle::CORNER_RB:
		mBound.l = blockGeometry.r;
		mBound.t = blockGeometry.b;
		break;
	}
	mBound.r = mBound.l + BLOCK_CORNER_SIZE;
	mBound.b = mBound.t + BLOCK_CORNER_SIZE;
}

Presenter* CornerPresenter::FindHit(const xsc::MPVector& point) const
{
	if (mBound.Contains(point))
	{
		return const_cast<CornerPresenter*>(this);
	}
	return nullptr;
}

void CornerPresenter::Render(ID2D1DeviceContext* context, double t)
{
	if (mHot)
	{
		solidBrush->SetColor(HOT_COLOR);
	}
	else
	{
		auto alignable = mParent->GetAlignableInterface();
		if (alignable->GetPrimary())
		{
			solidBrush->SetColor(PRIMARY_COLOR);
		}
		else
		{
			solidBrush->SetColor(SELECTED_COLOR);
		}
	}
	context->FillRectangle(mBound.ToRectF(), solidBrush);
}

} // namespace hspr
